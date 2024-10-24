#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // Display which does not send AC

int xdir = 1;
int ydir = 1;
int textPosX = 0;
int textPosY = 10;
int bounceSpeedX = 2;
int bounceSpeedY = 1;

void setup() {
  u8g.setFont(u8g_font_timB10);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(12, OUTPUT);

  u8g.firstPage();
  do {
    for (int y = 0; y < 64; y++) {
      for (int x = 0; x < 128; x++) {
        u8g.drawPixel(x, y);
      }
    }
  } while (u8g.nextPage());
  delay(2000);
}


void loop() {

  if (((textPosX == 0) && (textPosY == 10)) || ((textPosX == 0) && (textPosY == 64)) || ((textPosX == 100) && (textPosY == 10)) || ((textPosX == 100) && (textPosY == 64))) {
    digitalWrite(12, HIGH);
    for (int i = 10; i > 0; i--) {
      oledWrite(String(i), 32, 50);
      delay(400);
    }
    digitalWrite(12, LOW);
  }

  do {
    delay(1);
    bounceX(0, 100);
    bounceY(10, 64);
    oledWrite("DVD", textPosX, textPosY);
  } while (digitalRead(4) != LOW);
  do {
    delay(1);
    bounceX(0, 90);
    bounceY(10, 64);
    oledWrite("Logo", textPosY, textPosX);
  } while (digitalRead(5) != LOW);
}

void oledWrite(String text, int xpos, int ypos) {
  u8g.firstPage();

  do {
    u8g.drawStr(xpos, ypos, text.c_str());
  } while (u8g.nextPage());
}

void bounceX(int min, int max) {
  if (textPosX <= min) {
    xdir = 1;
  } else if (textPosX >= max) {
    xdir = 0;
  }
  if (xdir == 1) {
    textPosX = textPosX + bounceSpeedX;
  } else if (xdir == 0) {
    textPosX = textPosX - bounceSpeedX;
  }
}

void bounceY(int min, int max) {
  if (textPosY <= min) {
    ydir = 1;
  } else if (textPosY >= max) {
    ydir = 0;
  }
  if (ydir == 1) {
    textPosY = textPosY + bounceSpeedY;
  } else if (ydir == 0) {
    textPosY = textPosY - bounceSpeedY;
  }
}