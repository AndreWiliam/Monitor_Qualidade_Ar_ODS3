#  Monitor de Qualidade do Ar Interno com IoT - Contribuição ao ODS 3

## 🎯 Objetivo e Contexto (Requisito i)

Este projeto desenvolve um **Monitor Inteligente de Qualidade do Ar** para Ambientes Fechados (**ODS 3: Saúde e Bem-Estar**). O sistema utiliza um microcontrolador ESP32 para coletar dados ambientais e o protocolo MQTT para emitir **alertas preventivos de risco** em tempo real.

O projeto foi validado em um ambiente de simulação (Wokwi), comprovando a arquitetura IoT e a lógica de alerta.

---

## 🛠️ Hardware e Plataforma Utilizados (Requisito iii)

A documentação detalhada dos componentes e suas conexões está disponível na pasta `hardware/`.

| Componente | Tipo | Função na PoC (Prova de Conceito) |
| :--- | :--- | :--- |
| **Plataforma** | ESP32 DevKit V1 | Controle e gerenciamento da comunicação Wi-Fi. |
| **Sensor Adaptado** | MQ2 (Gás) | Simula a poluição do ar (CO2/VOCs) e atua como indicador primário de risco. |
| **Sensor Adaptado** | DHT22 | Mede a Temperatura e Umidade do ambiente. |
| **Atuador Principal** | Buzzer e LEDs | Sistema de alerta triplo (visual, sonoro e remoto). |

---

## 📡 Protocolos e Interfaces de Comunicação (Requisitos iv e v)

O projeto cumpre os requisitos de comunicação via internet e utiliza MQTT como protocolo de transporte principal.

* **Nível de Transporte/Rede:** **TCP/IP** (base para a conectividade Wi-Fi do ESP32).
* **Protocolo IoT:** **MQTT** (Message Queuing Telemetry Transport). O Broker utilizado para testes é o `broker.hivemq.com`.

### Interfaces MQTT (Tópicos Publicados)

O ESP32 publica mensagens em dois tópicos distintos para diferentes finalidades:

| Tópico | Conteúdo | Finalidade |
| :--- | :--- | :--- |
| `ods3/qualidade_ar/dados` | **JSON com Telemetria** ($\text{T}$, $\text{H}$, $\text{Valor}_{\text{Gás}}$) | Análise histórica e visualização de gráficos em dashboards. |
| `ods3/qualidade_ar/status` | **Mensagem de Status Crítico** (`ALERTA_GAS_ALTO` ou `AMBIENTE_SEGURO`) | Gatilho para **notificações push** imediatas em aplicativos móveis. |

---

## ⚙️ Guia Rápido de Uso e Reprodução (Requisito i)

### Como Reproduzir

1.  **Código-Fonte:** O firmware completo (`Monitor_CO_Alerta.ino`) com documentação linha a linha está na pasta `src/`.
2.  **Bibliotecas:** Instale as bibliotecas `PubSubClient` e `DHT sensor library` na IDE do Arduino.
3.  **Configuração:** Edite as constantes no código para ajustar suas credenciais de Wi-Fi e o limiar de alerta (`LIMIAR_ALERTA_GAS`).
4.  **Montagem:** O diagrama de conexão física está disponível em `docs/Diagrama_Conexao.png`.

### Lógica de Funcionamento

O sistema opera comparando a leitura analógica do gás (`Valor_Gas`) com o limiar de segurança (**1200**
