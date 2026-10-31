// Вариант 21. Санузел. Система контроля задымления и влажности
// Плата: Arduino Uno (ATmega328P), симулятор Wokwi
// Подключение:
//   Датчик 1 (MQ2, дым 0-100 %)            -> A0 (аналоговый выход AOUT)
//   Датчик 2 (PIR, присутствие 0/1)        -> D2
//   Датчик 3 (DHT22, влажность воздуха)    -> D3
//   Реле U4 (вентилятор)                   -> D7
//   Светодиод D1 (красный) через R1=150 Ом -> D8
// Библиотека: DHT sensor library (Adafruit), см. sim/libraries.txt

#include <DHT.h>

const int PIN_S1    = A0;   // датчик 1: MQ2 (дым)
const int PIN_S2    = 2;    // датчик 2: PIR (присутствие)
const int PIN_S3    = 3;    // датчик 3: DHT22 (влажность)
const int PIN_RELAY = 7;    // реле вентилятора
const int PIN_LED   = 8;    // светодиод-индикатор

const unsigned long T_MS = 1000;  // период опроса T = 1 с
const float S1_MIN = 0.0;         // диапазон датчика 1: 0...100 %
const float S1_MAX = 100.0;
const float THRESHOLD = 25.0;     // порог: дым > 25 %
const int   S1_BASE = 428;        // код АЦП MQ2 в чистом воздухе (калибровка нуля)

DHT dht(PIN_S3, DHT22);

unsigned long n = 0;    // номер измерения
bool alarm = false;     // порог сейчас превышен?

// Датчик 1: MQ2 (дым), пересчёт кода АЦП в проценты.
// MQ2 в чистом воздухе выдаёт ненулевое напряжение (код S1_BASE), поэтому
// шкала 0...100 % строится от S1_BASE до 1023: s1 = мин + (макс - мин) * (код - S1_BASE) / (1023 - S1_BASE)
float readS1() {
  int raw = analogRead(PIN_S1);
  if (raw < S1_BASE) raw = S1_BASE;
  return S1_MIN + (S1_MAX - S1_MIN) * (raw - S1_BASE) / (1023.0 - S1_BASE);
}

// Датчик 2: PIR (присутствие), 1 - движение есть
float readS2() {
  return digitalRead(PIN_S2);
}

// Датчик 3: DHT22 (относительная влажность, %)
float readS3() {
  return dht.readHumidity();   // при сбое вернёт NaN
}

void actuatorOn() {
  digitalWrite(PIN_RELAY, HIGH);
}

void actuatorOff() {
  digitalWrite(PIN_RELAY, LOW);
}

// Условие события: дым выше порога
bool conditionMet(float s1, float s2, float s3) {
  return s1 > THRESHOLD;
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_S2, INPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  dht.begin();
  actuatorOff();
  digitalWrite(PIN_LED, LOW);
}

void loop() {
  n++;

  float s1 = readS1();
  float s2 = readS2();
  float s3 = readS3();

  Serial.print("DATA n=");   Serial.print(n);
  Serial.print(" s1=");      Serial.print(s1, 2);
  Serial.print(" s2=");      Serial.print((int)s2);
  Serial.print(" s3=");      Serial.println(s3, 1);

  bool cond = conditionMet(s1, s2, s3);

  if (cond && !alarm) {            // порог начал выполняться
    alarm = true;
    actuatorOn();
    digitalWrite(PIN_LED, HIGH);
    Serial.print("EVENT n=");  Serial.print(n);
    Serial.println(" type=THRESHOLD");
  } else if (!cond && alarm) {     // порог перестал выполняться
    alarm = false;
    actuatorOff();
    digitalWrite(PIN_LED, LOW);
    Serial.print("EVENT n=");  Serial.print(n);
    Serial.println(" type=NORMAL");
  }

  delay(T_MS);
}
