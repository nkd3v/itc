#include "Arduino.h"
#include <EEPROM.h>
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "TimerOne.h"

const uint8_t xPin = A0;
const uint8_t yPin = A1;
const uint8_t zPin = A2;

const uint8_t ldrPin = A7;

const uint8_t btnLPin = 2;
const uint8_t btnMPin = 3;
const uint8_t btnRPin = 4;

const uint8_t speakerPin = 8;

const unsigned int successFreq = 700;
const unsigned int failFreq = 200;
const unsigned int beepFreq = 500;
const unsigned int sleepFreq = 5;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// 'menu', 128x32px
const unsigned char menu [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xc0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x08, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x10, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x20, 0x04, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x40, 0x04, 0x00, 0x40, 0x00, 0x30, 0x18, 0x7e, 0x30, 0x42, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x40, 0x04, 0x00, 0x40, 0x00, 0x30, 0x18, 0x40, 0x38, 0x42, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x04, 0x00, 0x20, 0x00, 0x38, 0x38, 0x40, 0x38, 0x42, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x04, 0x00, 0x20, 0x00, 0x28, 0x28, 0x40, 0x2c, 0x42, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x2c, 0x68, 0x40, 0x24, 0x42, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x24, 0x48, 0x7c, 0x26, 0x42, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x24, 0x48, 0x40, 0x22, 0x42, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00, 0x26, 0x88, 0x40, 0x21, 0x42, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x07, 0xfc, 0x10, 0x00, 0x23, 0x88, 0x40, 0x21, 0xc2, 0x04, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x23, 0x88, 0x40, 0x20, 0xc3, 0x0c, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x21, 0x08, 0x7e, 0x20, 0xc0, 0xf0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x40, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x40, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x20, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x06, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xc0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'shark', 128x32px
const unsigned char shark [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x3f, 0x80, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x08, 0x10, 0xff, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x08, 0x10, 0x00, 0x78, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x08, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x0c, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xff, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0xf0, 0x01, 0xc0, 0x1c, 0x00, 0x00, 0x00, 0xcc, 0x40, 0x03, 0x61, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x00, 0x40, 0x10, 0x00, 0x00, 0x01, 0x98, 0xc0, 0x03, 0xff, 0x01, 0x00, 0x00, 
  0x00, 0x01, 0xf8, 0x00, 0x63, 0xf0, 0x00, 0x00, 0x01, 0x11, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 
  0x00, 0x00, 0x0f, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x01, 0x11, 0x00, 0xe0, 0x00, 0x00, 0x80, 0x00, 
  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x0f, 0x8c, 0x00, 0x00, 0x80, 0x00, 
  0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x11, 0x03, 0xf8, 0x00, 0x00, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0x99, 0x82, 0x3d, 0x80, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x20, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x88, 0x80, 0x67, 0x00, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x20, 0x00, 0x00, 0x00, 0x03, 0xcf, 0x00, 0x04, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x20, 0x3f, 0xe0, 0x3f, 0x80, 0x00, 0x00, 0x07, 0xe9, 0x00, 0x04, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x03, 0xc0, 0x00, 0x0c, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x20, 0x60, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x60, 0x40, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xc1, 0xc0, 0x00, 0x00, 0x00, 0x07, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x18, 0x38, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

class Button {
private:
  uint8_t pin;
  unsigned long lastToggleTime;
  unsigned long delayTime = 200;

public:
  Button(uint8_t t_pin) : pin(t_pin) {}
  void begin() { pinMode(pin, INPUT_PULLUP); }

  boolean isPressed() {
    int pressed = !digitalRead(pin);
    if (millis() - lastToggleTime > delayTime && pressed) {
      lastToggleTime = millis();
      return true;
    }
    return false;
  }
};

Button btnL(btnLPin);
Button btnM(btnMPin);
Button btnR(btnRPin);

boolean timePaused = false;
int eeAddress = 0;

enum AlarmType {
  WAKE_UP,
  HOMEWORK,
  MEETING,
  OTHER
};

const char *alarmTypeMsg[] = {
  "Wake Up!",
  "Homework!",
  "Meeting!",
  "Alarmed!"
};

const char *alarmTypeSelectionMsg[] = {
  "Wake Up",
  "Homework",
  "Meeting",
  "Other"
};

struct Alarm {
  AlarmType type;
  unsigned long time;
};

struct Clock {
  unsigned long time;
  Alarm alarm[5];
  int nMaxAlarm = 5;
  byte nAlarmSet;
  unsigned long stopwatchTime;
} clock;

unsigned long prevPrintTime;
unsigned long printDuration;
boolean printLocked() { return millis() - prevPrintTime < printDuration; }

unsigned long prevDrawTime;
unsigned long drawDuration;
boolean drawLocked() { return millis() - prevDrawTime < drawDuration; }

void printToBuffer(const char* txt, int x, int y, unsigned long duration = 0) {
  if (printLocked() || drawLocked()) return;

  prevPrintTime = millis();
  printDuration = duration;

  OLED.clearDisplay();
  OLED.setCursor(x, y);
  OLED.println(txt);
}

void drawToBuffer(const unsigned char* bitmap, int x, int y, unsigned long duration = 0) {
  if (drawLocked() || printLocked()) return;

  prevDrawTime = millis();
  drawDuration = duration;

  OLED.clearDisplay();
  OLED.drawBitmap(x, y, bitmap, 128, 32, 1);
}

void displayTime(unsigned long time) {
  unsigned long sec = (time % 3600) % 60;
  unsigned long min = (time % 3600) / 60;
  unsigned long hour = time / 3600;

  char timeStr[10];
  sprintf(timeStr, "%02lu:%02lu:%02lu", hour, min, sec);

  printToBuffer(timeStr, 15, 10);
}

boolean alarmActive = false;

void playAlarm(unsigned long startTime, const char *msg) {
  if ((millis() - startTime) % 400 > 200) {
    printToBuffer(msg, 15, 10);
    tone(speakerPin, beepFreq);
  } else {
    printToBuffer("", 15, 10);
    noTone(speakerPin);
  }
}

void checkAlarm() {
  static Alarm livedAlarm;
  static unsigned int alarmStartTime;
  if (!alarmActive) {
    for (int i = 0; i < clock.nAlarmSet; i++) {
      if (clock.time == clock.alarm[i].time) {
        alarmActive = true;
        alarmStartTime = millis();
        livedAlarm = clock.alarm[i];
      }
    }
  } else {
    playAlarm(alarmStartTime, alarmTypeMsg[livedAlarm.type]);
  }
}

void updateTime() {
  clock.time = (clock.time + 1) % 86400;

  if (clock.time % 60 == 0)
    EEPROM.put(eeAddress, clock);
}

const size_t sampleSize = 6;

int ReadAxis(int axisPin) {
	long reading = 0;
	delay(1);
	for (int i = 0; i < sampleSize; i++) {
	  reading += analogRead(axisPin);
	}
	return reading/sampleSize;
}

void updateDisplayConfig() {

  if (analogRead(ldrPin) > 480) {
    OLED.dim(true);
  } else {
    OLED.dim(false);
  }

  float xAccel = (ReadAxis(xPin) - 507) / 112.;
  
  if (xAccel > 0.3) {
    OLED.setRotation(0);
  } else if (xAccel < -0.3) {
    OLED.setRotation(2);
  }
  Serial.println(xAccel);

}

void updateState() {

  static enum STATE {
    DISPLAY_TIME,
    SET_TIME,
    ALARM_MENU,
    STOPWATCH,
    MENU,
    ADD_ALARM,
    REMOVE_ALARM
  } currentState;

  switch (currentState) {

    case DISPLAY_TIME: {
      static boolean turnOffDisplay = false;

      checkAlarm();

      if (alarmActive) {
        if (btnL.isPressed() || btnM.isPressed() || btnR.isPressed()) {
          alarmActive = false;
          noTone(speakerPin);
        }
        break;
      }

      if (btnL.isPressed()) {
        turnOffDisplay = !turnOffDisplay;
        if (turnOffDisplay == true) {
          tone(speakerPin, sleepFreq, 200);
        }
      }

      if (turnOffDisplay) {
        OLED.clearDisplay();
        OLED.display();
        break;
      }

      if (btnM.isPressed()) {
        drawToBuffer(menu, 0, 0, 500);
        currentState = MENU;
        break;
      }

      if (btnR.isPressed()) {
        EEPROM.put(eeAddress, clock);
        printToBuffer("Saved!", 15, 10, 500);
        tone(speakerPin, successFreq, 100);
      }

      displayTime(clock.time);

      break;
    }

    case MENU: {
      static int selectedOption = 0;
      const char* modeName[] = { "Show Time", "Set Time", "Set Alarm", "Stopwatch" };
      const size_t nMode = sizeof(modeName) / sizeof(modeName[0]);

      if (btnL.isPressed()) {
        currentState = DISPLAY_TIME;
        selectedOption = 0;
        break;
      }

      if (btnM.isPressed()) {
        currentState = (STATE)selectedOption;
        selectedOption = 0;
        break;
      }

      if (btnR.isPressed()) {
        selectedOption = (selectedOption + 1) % nMode;
      }
      
      printToBuffer(modeName[selectedOption], 15, 10);

      break;
    }

    case SET_TIME: {
      static boolean firstInit = true;
      static unsigned long timeToSet;
      if (firstInit) {
        timeToSet = clock.time;
        firstInit = false;
      }

      static enum { HOUR, MIN, SEC } setMode = HOUR;

      if (btnL.isPressed()) {
        if (setMode == HOUR) {
          currentState = MENU;
          firstInit = true;
        } else if (setMode == MIN) {
          setMode = HOUR;
        } else if (setMode == SEC) {
          setMode = MIN;
        }
      }

      unsigned long hour = timeToSet / 3600;
      unsigned long min = (timeToSet % 3600) / 60;
      unsigned long sec = timeToSet % 60;

      if (btnM.isPressed()) {
        if (setMode == HOUR)
          setMode = MIN;
        else if (setMode == MIN)
          setMode = SEC;
        else if (setMode == SEC) {
          clock.time = timeToSet;
          EEPROM.put(eeAddress, clock);

          setMode = HOUR;
          currentState = DISPLAY_TIME;
          firstInit = true;

          tone(speakerPin, successFreq, 200);
          printToBuffer("Time set!", 15, 10, 1000);
          break;
        }
      }

      if (btnR.isPressed()) {
        if      (setMode == HOUR) hour = (hour + 1) % 24;
        else if (setMode == MIN)  min = (min + 1) % 60;
        else if (setMode == SEC)  sec = (sec + 1) % 60;
      }

      timeToSet = hour * 3600 + min * 60 + sec;

      displayTime(timeToSet);

      if (setMode == HOUR) {
        OLED.drawLine(15, 28, 36, 28, WHITE);
      } else if (setMode == MIN) {
        OLED.drawLine(51, 28, 72, 28, WHITE);
      } else if (setMode == SEC) {
        OLED.drawLine(87, 28, 108, 28, WHITE);
      }

      break;
    }

    case ALARM_MENU: {
      int nOptions = clock.nAlarmSet > 0 ? 2 : 1;
      static int selectedOption = 0;
      const char* modeName[] = { "Add", "Remove" };

      if (btnL.isPressed()) {
        currentState = MENU;
        selectedOption = 0;
        break;
      }

      if (btnM.isPressed()) {
        if (selectedOption == 0) {
          if (clock.nAlarmSet == clock.nMaxAlarm) {
            tone(speakerPin, failFreq, 200);
            printToBuffer("MemFulled!", 5, 10, 1000);
            break;
          }
          
          currentState = ADD_ALARM;
        } else if (selectedOption == 1) {
          currentState = REMOVE_ALARM;
        }

        selectedOption = 0;
        break;
      }

      if (btnR.isPressed()) {
        selectedOption = (selectedOption + 1) % nOptions;
      }
      
      printToBuffer(modeName[selectedOption], 15, 10);

      break;
    }

    case ADD_ALARM: {
      static boolean firstInit = true;

      static enum { HOUR, MIN, SEC, TYPE } setMode = HOUR;

      static Alarm alarmToSet;

      if (firstInit) {
        alarmToSet = {};
        setMode = {};
        firstInit = false;
      }

      if (btnL.isPressed()) {
        if (setMode == HOUR) {
          currentState = ALARM_MENU;
          firstInit = true;
          break;
        } else if (setMode == MIN) {
          setMode = HOUR;
        } else if (setMode == SEC) {
          setMode = MIN;
        } else if (setMode == TYPE) {
          setMode = SEC;
        }
      }

      if (btnM.isPressed()) {
        if (setMode == HOUR) {
          setMode = MIN;
        }
        else if (setMode == MIN) {
          setMode = SEC;
        }
        else if (setMode == SEC) {
          setMode = TYPE;
        } else if (setMode == TYPE) {
          clock.alarm[clock.nAlarmSet] = alarmToSet;
          clock.nAlarmSet++;

          EEPROM.put(eeAddress, clock);

          tone(speakerPin, successFreq, 200);
          printToBuffer("Alarm Set!", 7, 10, 1000);

          currentState = DISPLAY_TIME;
          firstInit = true;
          break;
        }
      }

      unsigned long hour = alarmToSet.time / 3600;
      unsigned long min = (alarmToSet.time % 3600) / 60;
      unsigned long sec = alarmToSet.time % 60;

      if (btnR.isPressed()) {
        if      (setMode == HOUR) hour = (hour + 1) % 24;
        else if (setMode == MIN)  min = (min + 1) % 60;
        else if (setMode == SEC)  sec = (sec + 1) % 60;
        else if (setMode == TYPE) {
          uint8_t iType = (alarmToSet.type + 1) % 4;
          alarmToSet.type = (AlarmType)iType;
        }
      }

      alarmToSet.time = hour * 3600 + min * 60 + sec;

      if (setMode != TYPE) {
        displayTime(alarmToSet.time);
      } else {
        printToBuffer(alarmTypeSelectionMsg[alarmToSet.type], 15, 10);
      }

      if (setMode == HOUR) {
        OLED.drawLine(15, 28, 36, 28, WHITE);
      } else if (setMode == MIN) {
        OLED.drawLine(51, 28, 72, 28, WHITE);
      } else if (setMode == SEC) {
        OLED.drawLine(87, 28, 108, 28, WHITE);
      }

      break;
    }

    case REMOVE_ALARM: {
      static int selectedOption = 0;

      if (btnL.isPressed()) {
        selectedOption = 0;
        currentState = ALARM_MENU;
        break;
      }

      if (btnM.isPressed()) {
        for (int i = selectedOption; i < clock.nAlarmSet - 1; i++) {
          clock.alarm[i] = clock.alarm[i + 1];
        }
        clock.nAlarmSet--;
        EEPROM.put(eeAddress, clock);

        tone(speakerPin, successFreq, 200);
        printToBuffer("Removed!", 10, 10, 1000);

        selectedOption = 0;
        currentState = DISPLAY_TIME;
        break;
      }

      if (btnR.isPressed() && clock.nAlarmSet > 1) {
        selectedOption = (selectedOption + 1) % clock.nAlarmSet;
      }

      displayTime(clock.alarm[selectedOption].time);

      break;
    }

    case STOPWATCH: {

      static boolean stopwatchPaused = true;
      static unsigned long prevTime = millis();
      
      unsigned long deltaTime = millis() - prevTime;
      prevTime = millis();

      if (!stopwatchPaused) {
        clock.stopwatchTime += deltaTime;
      }

      if (btnL.isPressed()) {
        currentState = MENU;
        break;
      }

      if (btnM.isPressed()) {
        stopwatchPaused = true;
        clock.stopwatchTime = 0;

        EEPROM.put(eeAddress, clock);
      }

      if (btnR.isPressed()) {
        stopwatchPaused = !stopwatchPaused;

        if (stopwatchPaused) {
          EEPROM.put(eeAddress, clock);
        }
      }

      displayTime(clock.stopwatchTime * 60 / 1000);
    }

    default:
      break;
  }
  
  OLED.display();
}

void setup() {
  
  Serial.begin(9600);

  if(!OLED.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  pinMode(speakerPin, OUTPUT);

  btnL.begin();
  btnM.begin();
  btnR.begin();
  
  EEPROM.get(eeAddress, clock);

  Timer1.initialize(1E6);
  Timer1.attachInterrupt(updateTime);

  drawToBuffer(shark, 0, 0, 2000);

  OLED.setTextColor(WHITE);
  OLED.setTextSize(2);
  OLED.setRotation(2);
}

void loop() {

  updateDisplayConfig();
  updateState();

}