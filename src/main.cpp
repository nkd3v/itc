#include "Arduino.h"
#include <EEPROM.h>
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

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

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
int eeAddress = 0;

struct Clock {
  unsigned long time;
  unsigned long alarmTime[5];
  int nMaxAlarm = 5;
  byte nAlarmSet;
} clock;

void updateTime() {
  if (!timePaused)
    clock.time++;

  if (clock.time % 60 == 0)
    EEPROM.put(eeAddress, clock);
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

const unsigned long alarmDuration = 10;
boolean alarmActive = false;

void playSound(unsigned long startTime) {
  if ((millis() - startTime) % 400 > 200) {
    tone(speakerPin, 300);
  } else {
    noTone(speakerPin);
  }
}

void checkAlarm() {
  for (int i = 0; i < clock.nAlarmSet; i++) {
    static unsigned long alarmStartTime;
    if (clock.time == clock.alarmTime[i]) {
      alarmStartTime = millis();
      alarmActive = true;
    }
    if (alarmActive && clock.time - clock.alarmTime[i] < alarmDuration) {
      playSound(alarmStartTime);
    } else {
      noTone(speakerPin);
      alarmActive = false;
    }
  }
}

void setup() {
  
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
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

  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setRotation(2);
}

void loop() {

  static enum STATE {
    DISPLAY_TIME, SET_TIME, ALARM_MENU,
    MENU, ADD_ALARM, REMOVE_ALARM
  } currState;

  float xAccel = (analogRead(xPin) - 507) / 112.;
  // float yAccel = (analogRead(yPin) - 514) / 111.;
  // float zAccel = (analogRead(zPin) - 503) / 100.;

  if (xAccel > 0.3)
    display.setRotation(0);
  else if (xAccel < -0.3)
    display.setRotation(2);

  switch (currState) {

    case DISPLAY_TIME: {
      timePaused = false;

      displayTime(clock.time);
      checkAlarm();

      if (alarmActive) {
        if (btnL.isPressed() || btnM.isPressed() || btnR.isPressed()) {
          alarmActive = false;
        }
      }

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
        clock.time = (clock.time - advancedTime[setMode] + 86400) % 86400;
      if (btnR.isPressed())
        clock.time = (clock.time + advancedTime[setMode]) % 86400;

      if (btnL.isPressed()) {
        if (setMode == HOUR)
          setMode = MIN;
        else if (setMode == MIN)
          setMode = SEC;
        else if (setMode == SEC) {
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
        displayTime(clock.time);

      break;
    }

    case ALARM_MENU: {
      timePaused = false;

      int nOptions = clock.nAlarmSet > 0 ? 2 : 1;
      static int selectedOption = 0;
      const char* modeName[] = { "Add", "Remove" };

      if (btnR.isPressed()) 
        selectedOption = (selectedOption + 1) % nOptions;
      
      printToDisplay(modeName[selectedOption], 15, 10);

      if (btnM.isPressed()) {
        currState = MENU;
        selectedOption = 0;
      }

      if (btnL.isPressed()) {
        if (selectedOption == 0) currState = ADD_ALARM;
        if (selectedOption == 1) currState = REMOVE_ALARM;

        selectedOption = 0;
      }

      break;
    }

    case ADD_ALARM: {
      timePaused = false;

      if (clock.nAlarmSet == clock.nMaxAlarm) {
        printToDisplay("Full", 15, 10);
        currState = DISPLAY_TIME;
      }

      static enum { HOUR, MIN, SEC } setMode = HOUR;
      static unsigned long completedTime = 0;
      static boolean setAlarmCompleted = false;
      static unsigned long alarmSetTime = 0;

      static const unsigned long advancedTime[3] = {3600, 60, 1};

      if (btnM.isPressed())
        alarmSetTime = (alarmSetTime - advancedTime[setMode] + 86400) % 86400;
      if (btnR.isPressed())
        alarmSetTime = (alarmSetTime + advancedTime[setMode]) % 86400;

      if (btnL.isPressed()) {
        if (setMode == HOUR)
          setMode = MIN;
        else if (setMode == MIN)
          setMode = SEC;
        else if (setMode == SEC) {
          completedTime = millis();
          setAlarmCompleted = true;

          clock.alarmTime[clock.nAlarmSet] = alarmSetTime;
          clock.nAlarmSet++;

          printToDisplay("Alarm Set!", 7, 10);
        }
      }

      if (setAlarmCompleted && millis() - completedTime > 1000) {
        setMode = HOUR;
        completedTime = 0;
        setAlarmCompleted = false;
        alarmSetTime = 0;

        currState = DISPLAY_TIME;
      }

      if (!setAlarmCompleted)
        displayTime(alarmSetTime);

      break;
    }
    
    case REMOVE_ALARM: {
      timePaused = false;

      static unsigned long removeCompletedTime;
      static boolean removeCompleted = false;

      static int selectedOption = 0;
      if (btnR.isPressed())
        selectedOption = (selectedOption + 1) % clock.nAlarmSet;

      if (btnM.isPressed()) {
        currState = ALARM_MENU;
        selectedOption = 0;
      }

      if (btnL.isPressed()) {
        for (int i = selectedOption; i < clock.nAlarmSet - 1; i++) {
          clock.alarmTime[i] = clock.alarmTime[i + 1];
        }
        clock.nAlarmSet--;
        EEPROM.put(eeAddress, clock);

        printToDisplay("Removed!", 10, 15);
        removeCompletedTime = millis();
        removeCompleted = true;
      }

      if (!removeCompleted)
        printToDisplay(String(selectedOption + 1).c_str(), 10, 15);

      if (removeCompleted && millis() - removeCompletedTime > 1000) {
        selectedOption = 0;
        removeCompleted = false;

        currState = DISPLAY_TIME;
      }

      break;
    }

    default:
      break;
  }
}
