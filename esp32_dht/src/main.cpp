#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

const char* ssid     = "WiFi-SSID";
const char* password = "WiFi-Password";
const char* serverURL = "http://localhost:5000/data"; // Change to your server URL

#define DHT_PIN  4
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

unsigned long lastSend = 0;
#define SEND_INTERVAL 5000

void sendData(float temp, float hum) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"temperature\":" + String(temp, 1) +
                ",\"humidity\":"    + String(hum, 1) + "}";

  int code = http.POST(body);

  if (code == 200) {
    Serial.println("✓ Sent to DB");
  } else {
    Serial.printf("✗ Error: %d\n", code);
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() - lastSend > SEND_INTERVAL) {
    lastSend = millis();

    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("Error reading DHT11!");
      return;
    }

    Serial.printf("T=%.1f°C  H=%.1f%%\n", temp, hum);
    sendData(temp, hum);
  }
}