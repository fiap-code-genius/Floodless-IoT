# Sistema IoT de Monitoramento de Enchentes

Este projeto implementa um sistema IoT para monitoramento e alerta de enchentes, utilizando sensores para medir temperatura, umidade, nível de água e intensidade de chuva. O sistema utiliza um ESP32 como microcontrolador principal, comunicação MQTT para transmissão dos dados e Node-RED para visualização e processamento das informações.

## Equipe
   - Wesley Sena dos Santos - RM: 558043
   - Vanessa Yukari Iwamoto - RM: 558092
   - Samara Victoria Ferraz dos Santos - RM: 558719

## Visão Geral do Sistema

O sistema é composto por três partes principais:
1. **Hardware (ESP32 + Sensores)**: Coleta dados do ambiente
2. **Broker MQTT**: Gerencia a comunicação entre dispositivos
3. **Node-RED**: Interface de visualização e processamento

### Componentes de Hardware

- ESP32 DevKit V1
- Sensor DHT22 (Temperatura e Umidade)
- Sensor Ultrassônico HC-SR04 (Nível da Água)
- Sensor de Chuva Analógico
- Display LCD I2C 16x2
- LED e Buzzer para alertas
- Botão para alternar modo de simulação

## Configuração no Wokwi

### Criando o Projeto

1. Acesse [Wokwi](https://wokwi.com)
2. Clique em "New Project"
3. Selecione "ESP32"
4. Nomeie o projeto como "IoT-Sensor-Enchente"

### Configurando o Hardware

1. Adicione os componentes necessários:
   - ESP32 DevKit V1 (já incluído por padrão)
   - DHT22 Temperature & Humidity Sensor
   - Ultrasonic Distance Sensor (HC-SR04)
   - LCD 16x2 (I2C)
   - LED (vermelho)
   - Active Buzzer
   - Push Button

2. Configure o `diagram.json`:
   - No Wokwi, clique no arquivo `diagram.json`
   - Apague todo o conteúdo existente
   - Cole o conteúdo do arquivo `diagram.json` disponível neste repositório
   - O diagrama será atualizado automaticamente com todas as conexões

   Alternativamente, você pode usar o seguinte JSON como base e ajustar conforme necessário:
   ```json
   {
     "version": 1,
     "author": "Seu Nome",
     "editor": "wokwi",
     "parts": [
       { "type": "wokwi-esp32-devkit-v1", "id": "esp", "top": 0, "left": 0 },
       { "type": "wokwi-dht22", "id": "dht1", "top": 0, "left": 200 },
       { "type": "wokwi-hc-sr04", "id": "ultrasonic1", "top": 100, "left": 200 },
       { "type": "wokwi-lcd1602", "id": "lcd1", "top": 200, "left": 0 },
       { "type": "wokwi-led", "id": "led1", "top": 300, "left": 0 },
       { "type": "wokwi-buzzer", "id": "bz1", "top": 300, "left": 100 },
       { "type": "wokwi-pushbutton", "id": "btn1", "top": 400, "left": 0 }
     ],
     "connections": [
       [ "esp:TX0", "$serialMonitor:RX", "", [] ],
       [ "esp:RX0", "$serialMonitor:TX", "", [] ],
       [ "esp:23", "dht1:SDA", "green", [] ],
       [ "esp:GND", "dht1:GND", "black", [] ],
       [ "esp:3V3", "dht1:VCC", "red", [] ],
       [ "esp:5", "ultrasonic1:TRIG", "yellow", [] ],
       [ "esp:18", "ultrasonic1:ECHO", "orange", [] ],
       [ "esp:GND", "ultrasonic1:GND", "black", [] ],
       [ "esp:5V", "ultrasonic1:VCC", "red", [] ],
       [ "esp:21", "lcd1:SDA", "blue", [] ],
       [ "esp:22", "lcd1:SCL", "purple", [] ],
       [ "esp:GND", "lcd1:GND", "black", [] ],
       [ "esp:3V3", "lcd1:VCC", "red", [] ],
       [ "esp:2", "led1:A", "red", [] ],
       [ "esp:GND", "led1:C", "black", [] ],
       [ "esp:23", "bz1:+", "red", [] ],
       [ "esp:GND", "bz1:-", "black", [] ],
       [ "esp:15", "btn1:1.r", "green", [] ],
       [ "esp:GND", "btn1:2.r", "black", [] ]
     ]
   }
   ```

3. Configure as bibliotecas em `libraries`:
   ```
   DHT sensor library
   LiquidCrystal I2C
   PubSubClient
   ArduinoJson
   ```

### Configuração do Ambiente

1. No arquivo principal (`flood_monitoring.ino`), configure as credenciais WiFi e MQTT:
   ```cpp
   const char* ssid = "Wokwi-GUEST";
   const char* password = "";
   const char* mqtt_server = "broker.hivemq.com";
   ```

2. Configure os pinos no código conforme o diagrama:
   ```cpp
   #define DHT_PIN 23
   #define TRIGGER_PIN 5
   #define ECHO_PIN 18
   #define LED_PIN 2
   #define BUZZER_PIN 23
   #define MODE_BUTTON_PIN 15
   ```

3. Após isso cole o código na primeira aba do Wokwi com o projeto aberto ao lado do diagram.json

### AVISO!!!!!

Pode haver um erro na simulação onde uma das telas da solução não apareça as informações:

![image](https://github.com/user-attachments/assets/24eaa978-9e01-4441-8f6c-5479b0919f73)


Caso mesmo após apertar o botão de play e os avisos sonoros começarem ele continuar desligado apenas tente parar de rodar e iniciar novamente até funcionar pois isso é problema do próprio Wokwi ;-;


### Diagrama de Conexões

O sistema foi implementado utilizando o simulador Wokwi, que permite testar o funcionamento completo do hardware. Abaixo está o diagrama de conexões dos componentes:


![diagrama_conexoes](https://github.com/user-attachments/assets/861ea969-c194-4aeb-8686-65b5b1595305)



## Configuração do Node-RED


![fluxo_nodered](https://github.com/user-attachments/assets/c6c53eab-a905-41ed-8929-c25a7258a992)



### Interface do Node-RED

O dashboard do Node-RED apresenta as seguintes informações em tempo real:

![dashboard png](https://github.com/user-attachments/assets/3fc8dd55-cc69-4eda-8965-708bb9389529)


Os elementos visuais incluem:
- Medidores (gauges) para:
  - Temperatura (0-50°C)
  - Umidade (0-100%)
  - Nível da Água (0-50cm)
- Indicadores de alertas para:
  - Chuva Forte
  - Emergência

### Instalação e Configuração

1. Instale o Node-RED:
   ```bash
   npm install -g node-red
   ```

2. Instale os nós necessários:
   ```bash
   npm install node-red-dashboard
   ```

3. Importe o fluxo:
   - Abra o Node-RED no navegador (http://localhost:1880)
   - Clique no menu (≡) no canto superior direito
   - Selecione "Import"
   - Cole o conteúdo do arquivo `flow.json` disponível neste repositório
   - Clique em "Import"

4. Acesso ao Dashboard:
   - O dashboard estará disponível em: http://localhost:1880/ui
   - Para acessar de outros dispositivos na rede local, substitua "localhost" pelo IP do computador
   - O dashboard atualiza automaticamente quando novos dados são recebidos

5. Personalizando o Dashboard:
   - No Node-RED, clique em qualquer nó do tipo "gauge" ou "chart"
   - Na aba "Dashboard", você pode:
     - Ajustar escalas dos medidores
     - Modificar cores e temas
     - Reorganizar os widgets
     - Adicionar novos grupos e abas

6. Configuração do Broker MQTT:
   - No nó "mqtt in", configure:
     - Server: broker.hivemq.com
     - Port: 1883
     - Topic: flood/sensors/data/device001

### Fluxo de Dados

O fluxo implementado no Node-RED processa os dados da seguinte forma:

![Fluxo Node-RED](images/fluxo_nodered.png)

1. **MQTT Input**: Recebe dados do tópico "flood/sensors/data/device001"
2. **JSON Parser**: Converte a mensagem JSON recebida
3. **Funções de Processamento**:
   - Temperatura
   - Umidade
   - Nível da Água
   - Intensidade da Chuva
   - Alertas
4. **Elementos Visuais**:
   - Gauges para temperatura, umidade e nível da água
   - Indicadores de alertas

O payload JSON enviado pelo dispositivo segue o formato:
```json
{
  "temperature": 24,
  "humidity": 40,
  "water_level": 399.942,
  "rain_intensity": 158.5,
  "alerts": {
    "flood_risk": false,
    "heavy_rain": true,
    "emergency": true
  }
}
```

## Uso do Sistema

### Modo Real

No modo real, o sistema:
1. Lê dados dos sensores físicos
2. Processa as informações
3. Envia alertas quando necessário
4. Exibe dados no LCD e no dashboard

### Modo Simulação

Para testar diferentes cenários, pressione o botão de modo para ativar a simulação:
1. Alterna entre 6 cenários diferentes
2. Simula condições normais até emergência
3. Atualiza a cada 5 segundos
4. Mantém todas as funcionalidades de alerta

## Estrutura do Código

### ESP32 (flood_monitoring.ino)

O código está organizado em seções:
- Definições e configurações
- Funções de leitura de sensores
- Funções de comunicação MQTT
- Funções de simulação
- Loop principal

### Node-RED (flow.json)

O fluxo do Node-RED implementa:
- Recebimento de dados via MQTT
- Processamento de JSON
- Exibição em dashboard
- Sistema de alertas

## Testes

Para testar o sistema:

1. **Modo Real**:
   - Verifique as conexões dos sensores
   - Monitore as leituras no Serial Monitor
   - Observe o dashboard do Node-RED

2. **Modo Simulação**:
   - Pressione o botão de modo
   - Observe os diferentes cenários
   - Verifique os alertas


## Parâmetros de Configuração

```cpp
// Limiares de alerta
const float WATER_LEVEL_ALERT = 15.0;  // cm
const int RAIN_INTENSITY_ALERT = 500;   // Valor analógico
const float TEMP_ALERT = 30.0;         // °C
const float HUMIDITY_ALERT = 80.0;     // %

// Intervalos
const int UPDATE_INTERVAL = 2000;      // ms
const int SIMULATION_INTERVAL = 5000;  // ms
```


## Contribuição

Sinta-se à vontade para contribuir com o projeto através de:
- Relatórios de bugs
- Sugestões de melhorias
- Pull requests


## Links

 - Demonstrando a solução: https://youtu.be/YfjVY-L0NZc
 - Projeto original: https://wokwi.com/projects/432243921915620353
