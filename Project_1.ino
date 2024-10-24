 /*
* Name: clock and temp project
* Author: Melker Willforss, Victor Huke
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include <Servo.h>
#include "U8glib.h"

// Init constants
const int tempPin = A0;
const int buzzer = 5;
const int warningLamp = 4;
const int fanPin = 13;

const int coldThresh = 18;const int hotThresh = 24;  //Temperature threshold constants for easy calibration

// Init global variables
String flavorText = "Startup text";

// construct objects
Servo myServo;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
RTC_DS3231 rtc;
char t[32];

void setup() {
  // init communication
  Serial.begin(9600);
  Wire.begin();

  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Init Hardware
  pinMode(tempPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(warningLamp, OUTPUT);

  myServo.attach(9);

  u8g.setFont(u8g_font_timR24);
  u8g.firstPage();
  do { //Start by filling the screen with white (because why not?)
    for (int y = 0; y < 64; y++) {
      for (int x = 0; x < 128; x++) {
        u8g.drawPixel(x, y);
      }
    }
  } while (u8g.nextPage());
  delay(1000);
}

void loop() {

  //Change flavorText string depending on current temperature, and turn on/off fan if appropriate
  if (getTemp() < coldThresh) {
    flavorText = "Brr! Too cold!";
    digitalWrite(fanPin, LOW);
  } else if (getTemp() > hotThresh) {
    flavorText = "Cool off a bit!";
    digitalWrite(fanPin, HIGH);
  } else {
    flavorText = "You're good!";
    digitalWrite(fanPin, LOW);
  }

  oledWrite(getTime(), flavorText); 
  servoWrite(getTemp());

  //Print getTemp and getTime in the Serial Monitor for debugging
  Serial.println(getTemp());
  Serial.println(getTime());

  //If temperature is too low, beep and flash the warning lamp
  if (getTemp() < coldThresh) {
    tone(buzzer, 1000);
    digitalWrite(warningLamp, HIGH);
    delay(500);
    digitalWrite(warningLamp, LOW);
    noTone(buzzer);
    delay(500);
  } else {
    delay(1000);
  }
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {
  DateTime now = rtc.now();
  return (String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temprature as float
*/
float getTemp() {
  int Vo;
  float R1 = 10000;  // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  //steinhart-hart coeficients for thermistor
  Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  float temp = T - 273.15;                                     //convert Kelvin to Celcius
  return temp;
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text1: String to write to display (used for time); text2: String to write to display below first string (used for flavortext)
*Returns: void
*/
void oledWrite(String text1, String text2) {
  u8g.firstPage();

  do {
    u8g.setFont(u8g_font_timR24);
    u8g.drawStr(((128 - (u8g.drawStr(-10, -10, text1.c_str()))) / 2), 32, text1.c_str());  // ((128 - (u8g.drawStr(-10, -10, text.c_str())))/2) argument ensures text is centered on display

    // Draw second text underneath first text in a smaller font
    u8g.setFont(u8g_font_profont15r);
    u8g.drawStr(((128 - (u8g.drawStr(-10, -10, text2.c_str()))) / 2), 52, text2.c_str());  // ((128 - (u8g.drawStr(-10, -10, text.c_str())))/2) argument ensures text is centered on display
  } while (u8g.nextPage());
}

/*
* takes a temprature value and maps it to corresppnding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/
void servoWrite(float value) {
  value = map(value, 10, 35, 0, 180);

  //"Round" extreme values to 0 or 180 degrees when appropriate
  if (value < 0) {
    myServo.write(0);
  } else if (value > 180) {
    myServo.write(180);
  } else {
    myServo.write(value);
  }
}