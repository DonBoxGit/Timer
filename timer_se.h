#pragma once
#include <EEPROM.h>
#include <Arduino.h>

class TimerClock {
  public:
    enum Element {
      HOURS = 0,
      MINUTES,
      SECONDS
    };
    void   readRomTime (void);
    void   writeRomTime(void);
    void   resetTime   (void);
    String getTime     (const TimerClock::Element&);
    void   changeTime  (const TimerClock::Element&, bool);
    void   changeTime  (const TimerClock::Element&, int8_t);
  private:
    uint8_t hours   = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
    void ctrlRange    (const TimerClock::Element&, int8_t*);
};

int8_t val = 0;                   // Переменая Счетчик Энкодера
int8_t menu_level = 0;
int8_t element_menu = 0;
volatile bool timerClick = false; // Переменная отсчета времени в прерывании

enum ElementTimer {
  START,
  EDIT,
  HOURS,
  MINUTES,
  SECONDS,
  APPLY,
  CLEAR,
  STOP
};

enum MenuLevel {
  MAIN,
  EDIT_MENU,
  EDIT_TIME,
  COUNTDOWN,
  MESSAGE_SCREEN,
  ALERT
};
