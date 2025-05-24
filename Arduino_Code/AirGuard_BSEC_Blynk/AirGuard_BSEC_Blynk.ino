#define BLYNK_TEMPLATE_ID "TMPL2QcSs5ybs"
#define BLYNK_TEMPLATE_NAME "AirGuard"
#define BLYNK_AUTH_TOKEN "mkAYcEeBRFnbJO76OqWpRYi2xX8Hk1hA"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "bsec.h"  

// OLED libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi Credentials
char ssid[] = "Dairo's iPhone";
char pass[] = "Dairo2005";

// Blynk & Sensor
BlynkTimer timer;
Bsec iaqSensor;

// OLED display config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Buzzer pin
#define buzzerPin 25
bool alarmEnabled = true;

void checkIAQ() {
  if (iaqSensor.run()) {
    float iaq = iaqSensor.iaq;
    float temp = iaqSensor.temperature;
    float hum = iaqSensor.humidity;
    float pressure = iaqSensor.pressure / 100.0;
    float co2 = iaqSensor.co2Equivalent;

    // Send to Blynk
    Blynk.virtualWrite(V0, iaq);
    Blynk.virtualWrite(V1, temp);
    Blynk.virtualWrite(V2, hum);
    Blynk.virtualWrite(V3, pressure);

    String status = getAirQualityText(iaq);
    Blynk.virtualWrite(V4, status);

    // Show on OLED
    display.clearDisplay();
    String fullStatus = "Air Status: " + status;

    // Check if air quality is bad
    bool isBadAir = (iaq >= 150);

    // === Blinking Air Status ===
    if (!isBadAir || (millis() / 500) % 2 == 0) {  // Blinks every 500ms
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(fullStatus, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH - w) / 2, 0);
      display.println(fullStatus);
    }

    // === IAQ, Temp, Hum below ===
    display.setTextSize(1);
    display.setCursor(0, 32);
    display.print("IAQ: "); display.println(iaq, 1);
    display.print("Temp: "); display.print(temp, 1); display.println(" C");
    display.print("Hum: "); display.print(hum, 1); display.println(" %");

    display.display();

    // Alarm logic
    if (alarmEnabled && iaq >= 150) {
      digitalWrite(buzzerPin, HIGH);
      Blynk.logEvent("bad_air", "⚠ Air Quality is " + status + " (IAQ: " + String((int)iaq) + ")");
    } else {
      digitalWrite(buzzerPin, LOW);
    }

  } else {
    Serial.println("BSEC read failed.");
  }
}

String getAirQualityText(float iaq) {
  if (iaq <= 50) return "Good :)";
  else if (iaq <= 100) return "Moderate";
  else if (iaq <= 150) return "Unhealthy (Sensitive)";
  else if (iaq <= 200) return "Unhealthy !";
  else if (iaq <= 300) return "Very Unhealthy !!";
  else return "Hazardous !!!";
}

BLYNK_WRITE(V5) {
  alarmEnabled = param.asInt();
  if (!alarmEnabled) digitalWrite(buzzerPin, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Wire.begin();

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // I2C address usually 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("AirGuard Starting...");
  display.display();
  delay(1000);

  // Initialize BME680 with BSEC
  iaqSensor.begin(0x76, Wire);  // Or use 0x77 if your sensor uses that address
  bsec_virtual_sensor_t sensorList[] = {
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_STABILIZATION_STATUS
  };

  iaqSensor.updateSubscription(sensorList, 8, BSEC_SAMPLE_RATE_LP);
  timer.setInterval(3000L, checkIAQ);
}

void loop() {
  Blynk.run();
  timer.run();
}
