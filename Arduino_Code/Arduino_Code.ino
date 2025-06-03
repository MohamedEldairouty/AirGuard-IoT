
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_BME680.h>
#include <math.h>

// WiFi credentials:
const char* ssid     = "Dairo";
const char* password = "Dairo2005";

// MQTT Broker details:
const char* mqtt_server   = "te20dd0d.ala.eu-central-1.emqxsl.com";
const int   mqtt_port     = 8883; 
const char* mqtt_user     = "Habiba";
const char* mqtt_password = "123456789";

// MQTT topics
const char* topic_temp        = "bme680/temperature";
const char* topic_humidity    = "bme680/humidity";
const char* topic_pressure    = "bme680/pressure";
const char* topic_gas         = "bme680/gas";
const char* topic_altitude    = "bme680/altitude";
const char* topic_iaq = "bme680/iaq";
const char* topic_iaq_label   = "airguard/iaq_label";
const char* topic_power_control = "airguard/power";


static const char* ca_cert = 
"-----BEGIN CERTIFICATE-----\n"
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
"-----END CERTIFICATE-----\n";

// Display & Sensor setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_BME680 bme;

#define SEALEVEL_PRESSURE_HPA 1013.25
#define BUZZER_PIN 4
#define FAN_PIN    5
#define GAS_THRESHOLD 15.0

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// State variables for manual control
bool manualPower = false;
unsigned long manualStartTime = 0;

int classifyIAQ(float iaq) {
  if (iaq <= 50)       return 0;
  else if (iaq <= 100) return 1;
  else if (iaq <= 150) return 2;
  else if (iaq <= 200) return 3;
  else if (iaq <= 300) return 4;
  else                 return 5;
}

const char* getIAQLabel(int level) {
  switch(level) {
    case 0: return "Good";
    case 1: return "Moderate";
    case 2: return "Unhealthy(S)";
    case 3: return "Unhealthy";
    case 4: return "Very Unhealthy";
    default:return "Dangerous";
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  if (String(topic) == topic_power_control) {
    manualPower = true;
    bool turnOn = (msg == "ON");
    digitalWrite(FAN_PIN, turnOn ? HIGH : LOW);
    digitalWrite(BUZZER_PIN, turnOn ? HIGH : LOW);
  }

  manualPower = true;
  manualStartTime = millis();                


  Serial.print("Received topic: ");
  Serial.print(topic);
  Serial.print(" -> ");
  Serial.println(msg);
}


void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT... ");
    if (mqttClient.connect("esp32_bme680", mqtt_user, mqtt_password)) {
      Serial.println("Connected");
      mqttClient.subscribe(topic_power_control);
    } else {
      Serial.print("Failed (rc=");
      Serial.print(mqttClient.state());
      Serial.println(") trying again...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while (1);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (!bme.begin()) while (1);
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setGasHeater(320, 150);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  espClient.setCACert(ca_cert);
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);

  reconnectMQTT();
}

void loop() {
  if (!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  if (manualPower && millis() - manualStartTime > 10000) {  // after 10 seconds
  manualPower = false;
  }

  if (!bme.performReading()) return;

  float temp     = bme.temperature;
  float humidity = bme.humidity;
  float pressure = bme.pressure / 100.0;
  float gas      = bme.gas_resistance / 1000.0;
  float altitude = 44330.0 * (1.0 - pow(pressure / SEALEVEL_PRESSURE_HPA, 0.1903));
  float iaq      = (100.0 - humidity) + (log10(gas) * 10);
  int iaq_level  = classifyIAQ(iaq);
  const char* iaq_label = getIAQLabel(iaq_level);

  // Auto-control if not overridden
  if (!manualPower) {
  bool shouldTurnOn = (iaq_level >= 3 || gas < GAS_THRESHOLD);
  digitalWrite(FAN_PIN, shouldTurnOn ? HIGH : LOW);
  digitalWrite(BUZZER_PIN, shouldTurnOn ? HIGH : LOW);
}

   display.clearDisplay();

    // 🔹 Title (Smaller to make space)
    display.setTextSize(1);
    display.setCursor(20, 0);
    display.println("AirGuard Monitor");

    // 🔹 Sensor Readings
    display.setCursor(0, 12);
    display.printf("Temp:     %.1f C\n", temp);
    display.printf("Hum:      %.0f %%\n", humidity);
    display.printf("Press:    %.0f hPa\n", pressure);
    display.printf("Gas:      %.1f kOhm\n", gas);
    display.printf("Alt:      %.0f m\n", altitude);
    display.printf("IAQ:      %.0f\n", iaq);

    display.display();

  // Publish MQTT
  char buf[32];
  dtostrf(temp, 6, 2, buf);     mqttClient.publish(topic_temp, buf);
  dtostrf(humidity, 6, 2, buf); mqttClient.publish(topic_humidity, buf);
  dtostrf(pressure, 6, 2, buf); mqttClient.publish(topic_pressure, buf);
  dtostrf(gas, 6, 2, buf);      mqttClient.publish(topic_gas, buf);
  dtostrf(altitude, 6, 2, buf); mqttClient.publish(topic_altitude, buf);
  dtostrf(iaq, 6, 2, buf);      mqttClient.publish(topic_iaq, buf);
  mqttClient.publish(topic_iaq_label, iaq_label);

  delay(3000);
}