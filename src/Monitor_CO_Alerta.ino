#include <WiFi.h>           // Inclui a biblioteca para conectividade Wi-Fi do ESP32
#include <PubSubClient.h>   // Inclui a biblioteca para comunicação MQTT
#include <DHT.h>            // Inclui a biblioteca para o sensor DHT22

/*
 * ===================================================================
 * DOCUMENTAÇÃO DO PROJETO - Monitor de Qualidade do Ar ODS 3
 * ===================================================================
 * Este firmware é a prova de conceito (PoC) para o monitoramento da qualidade do ar 
 * em ambientes internos, utilizando ESP32 e o protocolo MQTT para alertas remotos.
 * Contribui para o ODS 3: Saúde e Bem-Estar.
 */

// --- Credenciais e Configurações de Rede ---
const char* ssid = "Wokwi-GUEST";       // Define o nome da rede Wi-Fi (Padrão Wokwi)
const char* password = "";              // Define a senha (Vazia para Wokwi-GUEST)

// Configurações do Servidor MQTT
const char* mqtt_server = "broker.hivemq.com"; // Endereço do broker MQTT público
const char* mqtt_topic_data = "ods3/qualidade_ar/dados";    // Tópico para dados T/H/Gás (JSON)
const char* mqtt_topic_status = "ods3/qualidade_ar/status";  // Tópico para status de alerta (binário)
const char* clientID = "ESP32_AirMonitor_ODS3"; // ID único do cliente MQTT

// --- Definição dos Pinos de Hardware ---
const int DHT_PIN = 4;        // Pino de dados do DHT22 conectado ao GPIO 4
const int MQ2_PIN = 34;       // Pino AOUT do MQ2 conectado ao GPIO 34 (ADC)
const int LED_VERDE = 2;      // LED Verde (Seguro) conectado ao GPIO 2
const int LED_VERMELHO = 15;  // LED Vermelho (Alerta) conectado ao GPIO 15
const int BUZZER_PIN = 14;    // Buzzer conectado ao GPIO 14

// --- Constantes de Controle e Alerta ---
// Limiar de Gás (MQ2): Valor analógico acima do qual o alerta é acionado (calibrado para PoC)
const int LIMIAR_ALERTA_GAS = 1200; 
// Intervalo entre leituras e envios MQTT (10s para simulação)
const long INTERVALO_LEITURA = 10000; 

// --- Objetos de Comunicação e Sensores ---
WiFiClient espClient;                       // Cria um objeto Wi-Fi para o cliente MQTT
PubSubClient client(espClient);             // Cria o objeto cliente MQTT
DHT dht(DHT_PIN, DHT22);                    // Cria o objeto DHT, especificando pino e modelo

// Variável de controle de tempo (para temporização sem bloqueio)
unsigned long ultimaLeitura = 0;

/* -------------------------------------------------------------------
 * FUNÇÕES AUXILIARES E DE CONEXÃO
 * -------------------------------------------------------------------
 */

// Inicializa e conecta o ESP32 à rede Wi-Fi
void setup_wifi() {
  Serial.print("Conectando a ");           // Exibe mensagem no monitor serial
  Serial.println(ssid);                    // Exibe o nome da rede
  WiFi.begin(ssid, password);              // Inicia a conexão Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {  // Aguarda a conexão ser estabelecida
    delay(500);                            // Pausa de 500ms
    Serial.print(".");                     // Exibe progresso
  }
  Serial.println("\nWiFi conectado!");     // Confirma a conexão
  Serial.print("IP: ");                    // Exibe o endereço IP
  Serial.println(WiFi.localIP());
}

// Reconecta ao Broker MQTT caso a conexão caia
void reconnect() {
  while (!client.connected()) {            // Enquanto não estiver conectado...
    Serial.print("Tentando conexão MQTT...");
    // Tenta conectar usando o clientID
    if (client.connect(clientID)) {
      Serial.println("Conectado ao Broker MQTT!");
    } else {
      Serial.print("Falha, estado: ");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos.");
      delay(5000);                         // Espera 5 segundos antes de tentar reconectar
    }
  }
}

// Publica os dados de telemetria em formato JSON para o tópico principal
void publish_data(float temp, float hum, int gas_value) {
  char payload_data[150];                  // Buffer para armazenar a string JSON
  
  // Constrói a string JSON com os dados dos sensores
  sprintf(payload_data, "{\"temperatura\":%.2f, \"umidade\":%.2f, \"gas_analogico\":%d}", 
          temp, hum, gas_value);
  
  // Tenta publicar a string JSON no tópico de dados
  if (client.publish(mqtt_topic_data, payload_data)) {
    Serial.print("Dados ambientais enviados (JSON): ");
    Serial.println(payload_data);
  } else {
    Serial.println("Falha no envio do payload de dados.");
  }
}


/* -------------------------------------------------------------------
 * LÓGICA PRINCIPAL DE ALERTA (check_and_actuate)
 * Função central para tomada de decisão, atuação local e envio de status remoto.
 * -------------------------------------------------------------------
 */
void check_and_actuate(int gas_value) {
  String status_mqtt;                      // Variável para armazenar o status a ser enviado
  
  Serial.print("Valor do MQ2: ");
  Serial.println(gas_value);

  if (gas_value > LIMIAR_ALERTA_GAS) {
    // --- Lógica: ALERTA DE SAÚDE ---
    // Atuadores visuais/sonoros: Notificação local
    digitalWrite(LED_VERMELHO, HIGH);      // Acende LED Vermelho
    digitalWrite(LED_VERDE, LOW);          // Apaga LED Verde
    tone(BUZZER_PIN, 1500, 500);           // Toca alarme sonoro (1.5kHz por 0.5s)
    
    // Payload remoto:
    status_mqtt = "ALERTA_GAS_ALTO";
    Serial.println("!!! ALERTA DE SAÚDE: Poluentes elevados.");
  } else {
    // --- Lógica: AMBIENTE SEGURO ---
    // Atuadores visuais/sonoros: Confirmação de segurança
    digitalWrite(LED_VERMELHO, LOW);       // Apaga LED Vermelho
    digitalWrite(LED_VERDE, HIGH);         // Acende LED Verde
    noTone(BUZZER_PIN);                    // Silencia o Buzzer
    
    // Payload remoto:
    status_mqtt = "AMBIENTE_SEGURO";
    Serial.println("Qualidade do ar: Segura e dentro do limite.");
  }

  // Envia o status de alerta/segurança (mensagem crítica via MQTT)
  client.publish(mqtt_topic_status, status_mqtt.c_str());
  Serial.print("Status de Alerta (MQTT) enviado: ");
  Serial.println(status_mqtt);
}


/* -------------------------------------------------------------------
 * FUNÇÃO SETUP (Executada uma vez)
 * -------------------------------------------------------------------
 */
void setup() {
  Serial.begin(115200);                    // Inicializa a comunicação serial
  
  // Configura todos os pinos de atuadores como SAÍDA
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT); 
  
  // Inicializa o sensor DHT
  dht.begin();
  
  // Conecta à rede e configura o broker MQTT
  setup_wifi();
  client.setServer(mqtt_server, 1883);     // Define o endereço e porta do Broker 
}

/* -------------------------------------------------------------------
 * FUNÇÃO LOOP (Executada repetidamente)
 * -------------------------------------------------------------------
 */
void loop() {
  // Garante que o cliente MQTT esteja conectado
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Processa mensagens e mantém a conexão MQTT ativa

  unsigned long tempoAtual = millis();
  
  // Implementação de temporizador para leituras periódicas
  if (tempoAtual - ultimaLeitura >= INTERVALO_LEITURA) {
    ultimaLeitura = tempoAtual;

    // 1. Leitura dos Sensores
    float temperatura = dht.readTemperature(); // Lê temperatura
    float umidade = dht.readHumidity();       // Lê umidade
    int valor_gas = analogRead(MQ2_PIN);      // Lê o valor analógico do MQ2

    // 2. Tratamento de Erro: Ignora o ciclo se o DHT falhar
    if (isnan(temperatura) || isnan(umidade)) {
      Serial.println("Falha na leitura do sensor DHT! Pulando este ciclo.");
      return;
    }

    // 3. Envio de Dados (Payload JSON)
    publish_data(temperatura, umidade, valor_gas);
    
    // 4. Decisão de Alerta e Atuação (Controle)
    check_and_actuate(valor_gas);
  }
}
