#include "timerclock.h"

uint8_t TimerClock::getTime(const TimerClock::Element &elem){
  if(elem == HOURS)   return this -> hours;
  if(elem == MINUTES) return this -> minutes;
  if(elem == SECONDS) return this -> seconds;
}

String TimerClock::getTimeString(const TimerClock::Element &elem){
  String str = "0";
    if(elem == HOURS)   (hours   < 10) ? str += hours   : str = hours;
    if(elem == MINUTES) (minutes < 10) ? str += minutes : str = minutes;
    if(elem == SECONDS) (seconds < 10) ? str += seconds : str = seconds;
    return str; 
}

void TimerClock::changeTime(const TimerClock::Element &elem, bool dir) {
  if(elem == HOURS){
    (dir) ? ++hours : --hours;
    ctrlRange(elem, &hours);
  }
  if(elem == MINUTES){
    (dir) ? ++minutes : --minutes;
    ctrlRange(elem, &minutes);
  }
  if(elem == SECONDS){
    (dir) ? ++seconds : --seconds;
    ctrlRange(elem, &seconds);
  }
}

void TimerClock::changeTime(const TimerClock::Element &elem, int8_t num){
  ctrlRange(elem, &num);
  if(elem == HOURS)   this -> hours   = num;
  if(elem == MINUTES) this -> minutes = num;
  if(elem == SECONDS) this -> seconds = num;
}

void TimerClock::ctrlRange(const TimerClock::Element &elem, int8_t *param){
  if(elem == MINUTES || elem == SECONDS){
    if(*param > 59) *param = 0;
    if(*param < 0 ) *param = 59;
  }else{
    if(*param > 23) *param = 0;
    if(*param < 0 ) *param = 23;
  }
}

void TimerClock::readRomTime(void){
  this -> hours   = EEPROM.read(0);
  this -> minutes = EEPROM.read(1);
  this -> seconds = EEPROM.read(2);
}

void TimerClock::writeRomTime(void){
  EEPROM.update(0, this -> hours);
  EEPROM.update(1, this -> minutes);
  EEPROM.update(2, this -> seconds);
}

void TimerClock::resetTime(void) {
  this -> hours   = 0;
  this -> minutes = 0;
  this -> seconds = 0;
}
