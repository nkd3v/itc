#include "Arduino.h"
#include <EEPROM.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "TimerOne.h"

#pragma region
const uint8_t xPin = A0;
const uint8_t yPin = A1;
const uint8_t zPin = A2;

const uint8_t ldrPin = A7;

const uint8_t btnLPin = 2;
const uint8_t btnMPin = 3;
const uint8_t btnRPin = 4;

const uint8_t buzzerPin = 8;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#pragma endregion

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

class Button {
private:
  uint8_t pin;
  unsigned long lastToggleTime;
  unsigned long delayTime = 150;

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

struct Clock {
  unsigned long totalTime;
  unsigned long alarmTime[5];
  byte nAlarmSet;
} clock;

void updateTime() {
  if (!timePaused)
    clock.totalTime++;
}

void printToDisplay(const char* txt, int x, int y) {
  display.clearDisplay();
  display.setCursor(x, y);
  display.println(txt);
  display.display();
}

void displayTime(unsigned long time) {
  unsigned long sec = (time % 3600) % 60;
  unsigned long min = (time % 3600) / 60;
  unsigned long hour = time / 3600;

  char timeStr[10];
  sprintf(timeStr, "%02ld:%02ld:%02ld", hour, min, sec);

  printToDisplay(timeStr, 15, 10);
}

int eeAddress = 0;

void setup() {

  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  pinMode(buzzerPin, OUTPUT);

  btnL.begin();
  btnM.begin();
  btnR.begin();

  EEPROM.get(eeAddress, clock);

  Timer1.initialize(1E6);
  Timer1.attachInterrupt(updateTime);

  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setRotation(2);
}

void loop() {
  if (clock.totalTime % 60 == 0)
    EEPROM.put(eeAddress, clock);

  float xAccel = (analogRead(xPin) - 507) / 112.;
  // float yAccel = (analogRead(yPin) - 514) / 111.;
  // float zAccel = (analogRead(zPin) - 503) / 100.;

  if (xAccel > 0.3) display.setRotation(0);
  else if (xAccel < -0.3) display.setRotation(2);

  static enum STATE {
    DISPLAY_TIME, SET_TIME, ALARM_MENU,
    MENU, ADD_ALARM, EDIT_ALARM, REMOVE_ALARM
  } currState;

  switch (currState) {

  case DISPLAY_TIME: {
    timePaused = false;
    displayTime(clock.totalTime);

    if (btnL.isPressed())
      currState = MENU;

    break;
  }

  case MENU: {
    timePaused = false;

    static int selectedOption = 0;
    const char* modeName[] = { "Show Time", "Set Time", "Set Alarm" };

    if (btnR.isPressed()) 
      selectedOption = (selectedOption + 1) % 3;
    if (btnM.isPressed())
      selectedOption = (selectedOption - 1 + 3) % 3;
    
    printToDisplay(modeName[selectedOption], 15, 10);

    if (btnL.isPressed()) {
      currState = (STATE)selectedOption;
      selectedOption = 0;
    }

    break;
  }

  case SET_TIME: {
    timePaused = true;

    static enum { HOUR, MIN, SEC } setMode = HOUR;
    static unsigned long completedTime = 0;
    static boolean setTimeCompleted = false;

    const unsigned long advancedTime[3] = {3600, 60, 1};

    if (btnM.isPressed())
      clock.totalTime = (clock.totalTime - advancedTime[setMode] + 86400) % 86400;
    if (btnR.isPressed())
      clock.totalTime = (clock.totalTime + advancedTime[setMode]) % 86400;

    if (setMode == HOUR) {
      if (btnL.isPressed())
        setMode = MIN;
    }
    else if (setMode == MIN) {
      if (btnL.isPressed())
        setMode = SEC;
    }
    else if (setMode == SEC) {
      if (btnL.isPressed()) {
        EEPROM.put(eeAddress, clock);
        completedTime = millis();
        setTimeCompleted = true;

        printToDisplay("Time set!", 15, 10);
      }
    }

    if (setTimeCompleted && millis() - completedTime > 1000) {
      setMode = HOUR;
      completedTime = 0;
      setTimeCompleted = false;

      currState = DISPLAY_TIME;
    }

    if (!setTimeCompleted)
      displayTime(clock.totalTime);

    break;
  }

  case ALARM_MENU: {
    timePaused = false;

    static int selectedOption = 0;
    const char* modeName[] = { "Add", "Edit", "Remove" };

    if (btnR.isPressed()) 
      selectedOption = (selectedOption + 1) % 3;
    if (btnM.isPressed())
      selectedOption = (selectedOption - 1 + 3) % 3;
    
    printToDisplay(modeName[selectedOption], 15, 10);

    if (btnL.isPressed()) {
      
      switch (selectedOption) {
        case 0:
          currState = ADD_ALARM;
          break;
        case 1:
          currState = EDIT_ALARM;
          break;
        case 2:
          break;
        default:
          break;
      }

      selectedOption = 0;
    }

    break;
  }

  case ADD_ALARM: {
    timePaused = false;

    static enum { HOUR, MIN, SEC } setMode = HOUR;
    static unsigned long completedTime = 0;
    static boolean addAlarmCompleted = false;
    static unsigned long alarmSetTime = 0;

    static const unsigned long advancedTime[3] = {3600, 60, 1};

    if (btnM.isPressed())
      alarmSetTime = (alarmSetTime - advancedTime[setMode] + 86400) % 86400;
    if (btnR.isPressed())
      alarmSetTime = (alarmSetTime + advancedTime[setMode]) % 86400;

    if (setMode == HOUR) {
      if (btnL.isPressed())
        setMode = MIN;
    }
    else if (setMode == MIN) {
      if (btnL.isPressed())
        setMode = SEC;
    }
    else if (setMode == SEC) {
      if (btnL.isPressed()) {
        completedTime = millis();
        addAlarmCompleted = true;

        printToDisplay("Alarm Set!", 7, 10);
      }
    }

    if (addAlarmCompleted && millis() - completedTime > 1000) {
      setMode = HOUR;
      completedTime = 0;
      addAlarmCompleted = false;
      alarmSetTime = 0;

      currState = DISPLAY_TIME;
    }

    if (!addAlarmCompleted)
      displayTime(alarmSetTime);

    break;
  }

  case EDIT_ALARM: {
    break;
  }

  default:
    break;
  }
}
