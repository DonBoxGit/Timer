#pragma once
#include <EEPROM.h>
#include <Arduino.h>

class TimerClock {
  public:
    enum Element {
      HOURS = 2,
      MINUTES,
      SECONDS
    }      elementTimer;
    void   readRomTime   (void);
    void   writeRomTime  (void);
    void   resetTime     (void);
 uint8_t   getTime       (const TimerClock::Element&);
  String   getTimeString (const TimerClock::Element&);
    void   changeTime    (const TimerClock::Element&, const int8_t&);
    void   changeTime    (const TimerClock::Element&, bool);
    
  private:
 uint8_t   hours   = 0;
 uint8_t   minutes = 0;
 uint8_t   seconds = 0;
 
    void   ctrlRange     (const TimerClock::Element&, int8_t*);
};
