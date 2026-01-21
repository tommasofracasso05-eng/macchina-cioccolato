#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

// --- CONFIGURAZIONE WI-FI ---
const char* ssid = "TIM_7xTW2h";
const char* password = "6VYFJ622PV";

const char* host = "macchina-cioccolato.onrender.com";
const int port = 80;

// HC-SR04
const int trigPin = 12;
const int echoPin = 13;

// Variabili
bool operatorPresent = false;
String machineStatus = "STOPPED";
unsigned long previousMillis = 0;
const long interval = 5000;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, host, port);

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.print("Connessione Wi-Fi...");
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connesso!");
}

long getDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    long distanza = getDistanceCM();
    operatorPresent = (distanza < 50); // sotto 50 cm considerato vicino
    Serial.println("Distanza operatore: " + String(distanza) + " cm");

    // Simuliamo temperatura/umidità per ora
    int temp = random(20, 30);
    int hum = random(40, 60);

    sendData(temp, hum, machineStatus, operatorPresent);
    getCommand();
  }
}

void sendData(int temp, int hum, String status, bool operatorPresent) {
  String json = "{";
  json += "\"temperature\":" + String(temp) + ",";
  json += "\"humidity\":" + String(hum) + ",";
  json += "\"status\":\"" + status + "\",";
  json += "\"operatorPresent\":" + String(operatorPresent ? "true" : "false");
  json += "}";

  client.beginRequest();
  client.post("/api/data");
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", json.length());
  client.beginBody();
  client.print(json);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.println("Server risponde: " + String(statusCode) + " " + response);
}

void getCommand() {
  client.get("/api/command");
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  if (statusCode == 200 && response.length() > 0) {
    response.trim();
    Serial.println("Comando ricevuto: " + response);
    if (response == "START") machineStatus = "RUNNING";
    if (response == "STOP") machineStatus = "STOPPED";
  }
}
