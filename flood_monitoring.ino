#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

// Definição dos pinos
#define DHTPIN 23
#define DHTTYPE DHT22
#define TRIGGER_PIN 5
#define ECHO_PIN 18
#define LED_PIN 2
#define BUZZER_PIN 23
#define RAIN_DO 19
#define RAIN_AO 34
#define BTN_MODE 15  // Botão para alternar entre modo real e simulado

// Configurações do WiFi
const char* ssid = "Wokwi-GUEST";    // Nome da rede WiFi do Wokwi
const char* password = "";            // Senha vazia para rede do Wokwi

// Configurações do MQTT
const char* mqtt_server = "test.mosquitto.org";  // Broker público Mosquitto
const int mqtt_port = 1883;
const char* mqtt_user = "";     // Não precisa de autenticação
const char* mqtt_password = ""; // Não precisa de autenticação

// Tópicos MQTT
const char* topic_data = "flood/sensors/data/device001";
const char* topic_alerts = "flood/alerts/status/device001";
const char* topic_system = "flood/system/status/device001";

// Inicialização dos objetos
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient espClient;
PubSubClient client(espClient);

// Variáveis globais
float temperature = 24.0;
float humidity = 40.0;
float water_level = 0.0;
int rain_intensity = 0;
bool emergency = false;
bool simulation_mode = false;  // Modo de operação (false = real, true = simulação)
unsigned long lastMsg = 0;
const int MSG_INTERVAL = 2000;  // Intervalo de envio de mensagens (2 segundos)

// Variáveis para simulação
unsigned long simulationTimer = 0;
const int SIMULATION_INTERVAL = 5000; // 5 segundos para cada mudança
int simulationState = 0;

// Limiares de alerta
const float WATER_LEVEL_ALERT = 15.0;  // cm
const int RAIN_INTENSITY_ALERT = 500;   // Valor analógico

void setup() {
  Serial.begin(115200);
  
  // Configuração dos pinos
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RAIN_DO, INPUT);
  pinMode(RAIN_AO, INPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);  // Botão para alternar modo

  // Inicialização dos sensores
  dht.begin();
  
  // Inicialização do LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Iniciando...");

  // Conexão WiFi
  setup_wifi();
  
  // Configuração MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Processa mensagens recebidas do MQTT se necessário
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("conectado");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

float readWaterLevel() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;  // Converte para centímetros
}

void checkSimulationMode() {
  static bool lastModeState = true;
  bool currentModeState = digitalRead(BTN_MODE);
  
  if (lastModeState == true && currentModeState == false) {  // Botão pressionado
    simulation_mode = !simulation_mode;  // Alterna o modo
    Serial.println("Modo alterado para: " + String(simulation_mode ? "Simulação" : "Real"));
    
    // Mostra no LCD por 2 segundos
    lcd.clear();
    lcd.print("Modo: ");
    lcd.print(simulation_mode ? "Simulacao" : "Real");
    delay(2000);
  }
  lastModeState = currentModeState;
}

void activateAlerts() {
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000);  // 1kHz
  delay(500);
  noTone(BUZZER_PIN);
}

void deactivateAlerts() {
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
}

void simulateValues() {
  unsigned long now = millis();
  if (now - simulationTimer > SIMULATION_INTERVAL) {
    simulationTimer = now;
    simulationState = (simulationState + 1) % 6; // 6 estados diferentes

    switch(simulationState) {
      case 0: // Normal
        temperature = random(20, 25);
        humidity = random(40, 60);
        water_level = random(30, 40);
        rain_intensity = random(0, 100);
        emergency = false;
        break;
        
      case 1: // Temperatura Alta
        temperature = random(35, 40);
        humidity = random(30, 40);
        water_level = random(25, 35);
        rain_intensity = random(0, 100);
        emergency = false;
        break;
        
      case 2: // Chuva Forte
        temperature = random(18, 22);
        humidity = random(80, 90);
        water_level = random(20, 30);
        rain_intensity = random(600, 800);
        emergency = false;
        break;
        
      case 3: // Nível de Água Alto
        temperature = random(20, 25);
        humidity = random(70, 80);
        water_level = random(5, 10); // Mais próximo do sensor = água mais alta
        rain_intensity = random(400, 600);
        emergency = false;
        break;
        
      case 4: // Situação de Emergência
        temperature = random(22, 26);
        humidity = random(85, 95);
        water_level = random(3, 8);
        rain_intensity = random(800, 1000);
        emergency = true;
        break;
        
      case 5: // Voltando ao Normal
        temperature = random(20, 24);
        humidity = random(50, 60);
        water_level = random(25, 35);
        rain_intensity = random(100, 200);
        emergency = false;
        break;
    }

    Serial.println("Estado da simulação: " + String(simulationState));
    Serial.println("Temperatura: " + String(temperature));
    Serial.println("Umidade: " + String(humidity));
    Serial.println("Nível da água: " + String(water_level));
    Serial.println("Intensidade da chuva: " + String(rain_intensity));
    Serial.println("Emergência: " + String(emergency));
    Serial.println("-------------------");
  }
}

void readSensors() {
  // Lê valores reais dos sensores
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  water_level = readWaterLevel();
  rain_intensity = analogRead(RAIN_AO);
  
  // Verifica condições de emergência com base nos sensores
  emergency = (water_level < WATER_LEVEL_ALERT || rain_intensity > RAIN_INTENSITY_ALERT);
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(humidity, 0);
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  lcd.print("Agua:");
  lcd.print(water_level, 1);
  lcd.print("cm");
}

void publishData() {
  StaticJsonDocument<200> doc;
  
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["water_level"] = water_level;
  doc["rain_intensity"] = rain_intensity;
  doc["alerts"]["flood_risk"] = water_level < WATER_LEVEL_ALERT;
  doc["alerts"]["heavy_rain"] = rain_intensity > RAIN_INTENSITY_ALERT;
  doc["alerts"]["emergency"] = emergency;
  
  char buffer[200];
  serializeJson(doc, buffer);
  client.publish(topic_data, buffer);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Verifica o botão de modo
  checkSimulationMode();

  unsigned long now = millis();
  if (now - lastMsg > MSG_INTERVAL) {
    lastMsg = now;
    
    // Decide entre leitura real ou simulação
    if (simulation_mode) {
      simulateValues();
    } else {
      readSensors();
    }
    
    // Verifica condições de alerta
    if (emergency) {
      activateAlerts();
    } else {
      deactivateAlerts();
    }
    
    // Atualiza display LCD
    updateLCD();
    
    // Publica dados via MQTT
    publishData();
  }
} 