#ifndef timer_se_h
#define timer_se_h

#include <Arduino.h>

class TimerClock {
  public:
    enum Element {
      HOURS,
      MINUTES,
      SECONDS
    };
    void readTime (void);
    void writeTime(void);
    void resetTime(void);
    String getTime(TimerClock::Element&);
    void changeTime(TimerClock::Element&, bool);
  private:
    uint8_t hours   = 0;
    uint8_t minutes = 0;
    uint8_t seconds = 0;
};

int8_t val = 0;                   // Переменая Счетчик Энкодера
int8_t menu_level = 0;
int8_t element_menu;
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

String str_time(uint8_t);
void draw_led(int8_t&, int8_t&);
void edit_time(int8_t&, bool);
void countdown_timer(void);

#endif
