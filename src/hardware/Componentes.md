# Hardware e Plataforma Utilizados

Este documento detalha o hardware essencial utilizado para a Prova de Conceito (PoC) do Monitor de Qualidade do Ar, incluindo plataformas de desenvolvimento, sensores e atuadores.

## 1. Plataforma de Desenvolvimento
| Componente | Tipo | Descrição |
| :--- | :--- | :--- |
| **ESP32 DevKit V1** | Microcontrolador | Cérebro do projeto. Escolhido por sua capacidade **Dual-Core** e conectividade Wi-Fi integrada, essencial para gerenciar a leitura simultânea dos sensores e a pilha de comunicação MQTT. |
| **Protoboard (Breadboard)** | Suporte | Utilizada para fazer as conexões temporárias e a distribuição de energia (3V3 e GND) entre o ESP32 e os periféricos na simulação do Wokwi. |

## 2. Sensores Adaptados (PoC)

| Componente | Tipo | Pino de Conexão (ESP32) | Função na Simulação |
| :--- | :--- | :--- | :--- |
| **Sensor de Gás (MQ2)** | Analógico | GPIO 34 (AOUT) | Atua como **indicador genérico de poluição** (CO2 e VOCs). A leitura analógica é usada para determinar o risco de segurança em relação ao LIMIAR_ALERTA. |
| **Sensor DHT22** | Digital | GPIO 4 (DATA) | Fornece dados de **Temperatura e Umidade**, monitorando o conforto ambiental. |

## 3. Atuadores (Sistema de Alerta Triplo)

| Componente | Tipo | Pino de Conexão (ESP32) | Função do Atuador |
| :--- | :--- | :--- | :--- |
| **LED Vermelho** | Saída Digital | GPIO 15 | Alerta visual de **Alto Risco** (gás acima do limiar). |
| **LED Verde** | Saída Digital | GPIO 2 | Indicador visual de **Ambiente Seguro**. |
| **Buzzer** | Saída Digital | GPIO 14 | Alerta sonoro de **Emergência**, crucial para notificação imediata. |
| **Resistores** | Suporte | 220Omega (típico) | Usados em série com os LEDs para limitar a corrente e evitar queimar os diodos. |

## 4. Notas sobre o Projeto Físico

* **(Medidas de Peças/Caixas):** Para a implementação física, o protótipo exigirá um gabinete com aberturas de ventilação para os sensores.
* **(Impressão 3D):** O projeto de uma caixa customizada (gabinete) seria aprimorado utilizando **impressão 3D** para garantir o posicionamento ideal dos sensores e a proteção do circuito.

