#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TM1637Display.h>

#define CLK_PIN 2
#define DIO_PIN 3

TM1637Display display(CLK_PIN, DIO_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C 주소와 LCD 크기에 맞게 설정

const int pressureSensorPin = A0;
const int threshold = 100;

int hours = 0;   // 시 값
int minutes = 0; // 분 값

void setup() {
    lcd.init(); // LCD 초기화
    lcd.backlight(); // 백라이트 활성화
    lcd.setCursor(0, 0);
    lcd.print("How's your back?"); // 초기 메시지
    Serial.begin(9600); // 시리얼 통신 시작
    pinMode(pressureSensorPin, INPUT);
    display.setBrightness(7);  // 밝기 설정 (0에서 7까지)
    display.clear(); 
    display.showNumberDecEx(hours * 100 + minutes, 0b01000000, true);
}

void loop() {
  int pressureValue = analogRead(pressureSensorPin);
  if (pressureValue >= threshold) {
    Serial.println("Pressure input detected.");
    if (Serial.available()) {
        char command = Serial.read(); // 파이썬에서 받은 문자 읽기
        if (command == '0') {
            lcd.clear();
            lcd.setCursor(5, 1);
            lcd.print("GOOD");
            display.showNumberDecEx(hours * 100 + minutes, 0b01000000, true);
            delay(1000);
            minutes++; // 분 증가
            if (minutes >= 60) {
              minutes = 0; // 60분이 되면 분을 0으로 리셋
              hours++;    // 시간 증가
              Serial.println("1 hour later");
              if (hours >= 24) {
                hours = 0;
              }
            }
            } // LCD에 표시할 메시지
        else if (command =='1'){
            lcd.clear();
            lcd.setCursor(5, 1);
            lcd.print("WARNING");
            display.showNumberDecEx(hours * 100 + minutes, 0b01000000, true);
            delay(1000);
            minutes++; // 분 증가
            if (minutes >= 60) {
              minutes = 0; // 60분이 되면 분을 0으로 리셋
              hours++;    // 시간 증가
              Serial.println("1 hour later");
              if (hours >= 24) {
                hours = 0;
              }
            }
        }
        else if (command == '2') {
            lcd.clear(); // LCD 내용 지우기
            lcd.setCursor(4, 1);
            lcd.print("WAKE UP");
            minutes = 0;
            hours = 0;
            display.showNumberDecEx(hours * 100 + minutes, 0b01000000, true);
        }
    }
  }
}
