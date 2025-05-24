#define BLYNK_TEMPLATE_ID "TMPL2QcSs5ybs"
#define BLYNK_TEMPLATE_NAME "AirGuard"
#define BLYNK_AUTH_TOKEN "mkAYcEeBRFnbJO76OqWpRYi2xX8Hk1hA"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "bsec.h"  

char ssid[] = "Dairo's iPhone";
char pass[] = "Dairo2005";

BlynkTimer timer;
#define buzzerPin 25
bool alarmEnabled = true;

Bsec iaqSensor;

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

    Serial.printf("IAQ: %.1f | %s\n", iaq, status.c_str());

    // Alarm logic
    if (alarmEnabled && iaq >= 150) {
      digitalWrite(buzzerPin, HIGH);
      String status = getAirQualityText(iaq);
      Blynk.logEvent("bad_air", "⚠ Air Quality is " + status + " (IAQ: " + String((int)iaq) + ")");
    } else {
      digitalWrite(buzzerPin, LOW);
    }

  } else {
    Serial.println("BSEC read failed.");
  }
}

String getAirQualityText(float iaq) {
  if (iaq <= 50) return "Good";
  else if (iaq <= 100) return "Moderate";
  else if (iaq <= 150) return "Unhealthy (Sensitive)";
  else if (iaq <= 200) return "Unhealthy";
  else if (iaq <= 300) return "Very Unhealthy";
  else return "Hazardous";
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

  iaqSensor.begin(0x76, Wire);  // or 0x77
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