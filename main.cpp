#include <Arduino.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

constexpr uint8_t DHTPIN = 2;
constexpr uint8_t UV_PIN = A0;
constexpr uint8_t ANEMOMETER_PIN = A1;

constexpr uint8_t VIN2 = 12;

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

int calculateUV(float mV);

void setup() {
  Serial.begin(9600);

  // Temp/Hum
  dht.begin();

  pinMode(VIN2, OUTPUT);
  digitalWrite(VIN2, 1);

  // Pressao
  if (!bmp.begin()) {
    Serial.println("Erro: BMP180 não encontrado!");
    while (1);
  }
}

void loop() {
   delay(2500); // só pode ler a cada 2 segundos

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  float p = -1;
  bmp.getPressure(&p);
  p *= 0.01; // To mB


  int uvValue = analogRead(UV_PIN);      // 0–1023
  float mv = uvValue * (5.0 /1023.f) * 1000.0f;
  int uv = calculateUV(mv);

  int windValue = analogRead(ANEMOMETER_PIN);
  int windSpeed = map(windValue, 111, 410, 0, 30);

  Serial.print("Pressao: ");
  Serial.print(p, 1);
  Serial.print(" mB Vento: ");
  Serial.print(windSpeed, 1);
  Serial.println("m/s");
  Serial.print("Umidade: ");
  Serial.print(h, 1);
  Serial.print(" %  Temperatura: ");
  Serial.print(t, 1);
  Serial.println(" *C");



  Serial.print("A0: ");
  Serial.print(uvValue);
  Serial.print(" - ");
  Serial.print(mv, 2);
  Serial.println("mV");
  Serial.print("UV Index: ");
  Serial.println(uv, 1);
}

int calculateUV(float mV)
{
      static const float thr[] = {
        0.0f, 227.0f, 318.0f, 408.0f, 503.0f, 606.0f,
        696.0f, 795.0f, 881.0f, 976.0f, 1079.0f, 1170.0f
    };
    const int N = (int)(sizeof(thr)/sizeof(thr[0])); // 12 limites -> 11 degraus

    if (mV < thr[1]) return 0;
    for (int uvi = 1; uvi < N-1; ++uvi) {
        if (mV < thr[uvi+1]) return uvi;
    }
    return 11;
}