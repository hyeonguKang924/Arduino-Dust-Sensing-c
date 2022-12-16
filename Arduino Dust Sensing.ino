#include "notes.h"
#include "CoffinDance.h"
int buzzerPin = 8;
int buzzPin = 13;
int looptune = 5;
unsigned long previousMillis = 0;
bool ledState = LOW;

//----- 핀 설정
#define PIN_LCD_EN 12 //LCD EN핀
#define PIN_LCD_RS 13 //LCD RS핀
#define PIN_LCD_VE 6 //LCD 콘트라스트핀
#define PIN_LCD_D0 5 //LCD D0핀
#define PIN_LCD_D1 4 //LCD D1핀
#define PIN_LCD_D2 3 //LCD D2핀
#define PIN_LCD_D3 2 //LCD D3핀

#define PIN_DUST_OUT A0 // 미세먼지 센서 데이터 출력
#define PIN_DUST_LED 11 // 미세먼지 센서 적외선 LED

#define PIN_DHT_OUT 7 // 온습도계 핀 설정

#define PIN 10 //디지털 입력 핀 설정
#define NUMPIXELS 4 //Neopixel LED 소자 수(LED가 24개라면, 24작성)
#define BRIGHTNESS 180 //밝기 설정 0(어둡게) ~ 255(밝게)까지 임의로 설정 가능

#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); //LCD 설정

#include <LiquidCrystal.h>
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D0, PIN_LCD_D1, PIN_LCD_D2, PIN_LCD_D3);

#include <DHT.h>
DHT dht(PIN_DHT_OUT, DHT11);

void setup() { //pinMode 및 Serial 초기화
  looptune = looptune * 72;
  pinMode(buzzPin, OUTPUT); 
  delay(3000);
  digitalWrite(buzzPin, HIGH); //미세먼지 센서 적외선 LED 초기값 설정(부논리)
  delay(1000);

  Serial.begin(9600);
  pinMode(PIN_DUST_LED, OUTPUT);

  digitalWrite(PIN_DUST_LED, HIGH);

  analogWrite(PIN_LCD_VE, 120);      // LCD 디스플레이 콘트라스트 조절
  lcd.begin(16, 2);

  dht.begin();

  pinMode(8, OUTPUT); //부저 핀 설정

  strip.setBrightness(BRIGHTNESS);    
  strip.begin();                                
  strip.show();      
}
float read_dust() {
  digitalWrite(PIN_DUST_LED, LOW);
  delayMicroseconds(280);
  int dustval = analogRead(PIN_DUST_OUT);
  delayMicroseconds(40);
  digitalWrite(PIN_DUST_LED, HIGH);
  delayMicroseconds(9680);
  float voltage = dustval * (5.0 / 1024.0);
  float dust_ug = (0.17 * voltage - 0.1) * 1000;
  return dust_ug >= 0 ? dust_ug : 0;
}

void print_lcd() {
  float dust_ug = read_dust();
  int dht_temperature = dht.readTemperature();
  int dht_humidity = dht.readHumidity();

  Serial.print("Temperature: ");
  Serial.print(dht_temperature);
  Serial.print(" C\nHumidity: ");
  Serial.print(dht_humidity);
  Serial.print(" %\nDust: ");
  Serial.print(dust_ug);
  Serial.println(" ug");

  lcd.clear();          // LCD 내용 삭제
  lcd.setCursor(0, 0);  // 0, 0으로 커서 설정
  lcd.print("T: ");     // 내용 출력
  lcd.print(dht_temperature);
  lcd.print(" C ");
  lcd.print("H: ");
  lcd.print(dht_humidity);
  lcd.print(" %");
  lcd.setCursor(0, 1);
  lcd.print("Dust: ");
  lcd.print(dust_ug);
  lcd.print("ug");

    if(dust_ug < 15) {         // 미세먼지가 15ug미만일 경우
    strip.setPixelColor(1, 255, 0, 0);  // 빨강
    delay(500); noTone(8);delay(500);tone(8, 0);
  } else if(dust_ug < 30) {  // 미세먼지가 15ug이상, 30ug미만일 경우
    strip.setPixelColor(1, 255, 128, 0);  // 주황
    delay(500); noTone(8);delay(500);tone(8, 0);
  } else if (dust_ug < 40) {        // 미세먼지가 30ug이상, 40ug미만일 경우
    strip.setPixelColor(1, 255, 255, 0); // 노랑
    delay(500); noTone(8);delay(500);tone(8, 0);
  } else if (dust_ug < 50){                   // 미세먼지가 40ug이상, 50ug미만일 경우
    strip.setPixelColor(1, 0, 255, 0);  // 초록
    delay(500); noTone(8);delay(500);tone(8, 0);
  } else if (dust_ug < 75){                   // 미세먼지가 50ug이상, 75ug미만일 경우
    strip.setPixelColor(1, 0, 255, 255); // 하늘
    delay(500); noTone(8);delay(500);tone(8, 120);
  } else if (dust_ug < 100){                   // 미세먼지가 75ug이상, 100ug미만일 경우
    strip.setPixelColor(1, 0, 0, 255);  // 남색
    delay(500); noTone(8);delay(500);tone(8, 360);
  } else if (dust_ug < 150){                   // 미세먼지가 100ug이상, 150ug미만일 경우
    strip.setPixelColor(1, 127, 0, 255); // 보라

    delay(500); noTone(8);delay(500);tone(8, 1080);
  } else (dust_ug > 151);{                   // 미세먼지가 151ug 이상일 경우
    strip.setPixelColor(1, 153, 76, 0); //갈색
  }
    for (int i = 0; i < looptune; i++)
    {
      blinkMe(); //For the indication
      int noteDuration = 750 / 4;
      tone(buzzerPin, CoffinDance[i % 72], noteDuration);
      int delayTime = noteDuration * 1.30;
      delay(delayTime);
    }
}

void loop() {
  strip.begin(); // 네오led 실행
  print_lcd();  // lcd 실행
  strip.show(); // 네오led 보여주
  delay(2000);
}

void blinkMe()
{
  if (millis() - previousMillis >= 200)
  {
    previousMillis = millis();

    if (ledState == LOW)
    {
      ledState = HIGH;
    }
    else
    {
      ledState = LOW;
    }
    digitalWrite(buzzPin, ledState);
  }

}
