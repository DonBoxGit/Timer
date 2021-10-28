#include "timer_se.h"

/*************************Подключаем SSD1306********************************/
Adafruit_SSD1306 display(128, 32, &Wire,   OLED_RESET);
/*************************Подключаем Энкодер********************************/
Encoder encoder(SLK, DT, SW);
/**********************Вектро Прерывания TIMER1*****************************/
ISR (TIMER1_COMPA_vect) {
  timerClick = true;
}

void countdown_timer() {
  if (hours_t == 0 && min_t == 0 && sec_t < MIN_TIME_START) { //Если меньше MIN_TIME_START секунд, отсчет не начинаем
    menu_level = ALERT;
    draw_led(val, menu_level);
    delay(200);
    menu_level = MAIN;
    draw_led(val, menu_level);
    return;
  }
  // Вкл. Вектор Прерывания
  cli();
  TCNT1 = 0x00;            // Сбрасываем Счетный Регистр TCNT1
  TIMSK1 |= (1 << OCIE1A); // Включаем Прерывания
  sei();
  ++sec_t;                 // Увеличиваем на 1 секунду т.к. timerClick = true
  while (true) {
    encoder.tick();
    if (encoder.isClick() && menu_level == COUNTDOWN && val == STOP) // Нажали STOP
    {
      tone(SPEAKER_PIN, 250, 40);
      menu_level = MAIN;
      val = START;
      draw_led(val, menu_level);
      break;
    }
    if (hours_t == 0 && min_t == 0 && sec_t == 0) { // Если Закончилось Время: Выход из Цикла
      tone(SPEAKER_PIN, TONE, 1000);
      menu_level = MESSAGE_SCREEN;
      draw_led(val, menu_level);
      digitalWrite(OUT_PIN, HIGH);
      delay(1000);
      menu_level = MAIN;
      val = START;
      draw_led(val, menu_level);
      digitalWrite(OUT_PIN, LOW);
      break;
    }
    // Таймер Обратного Отсчета
    if (timerClick) {
      timerClick = false;
      if (sec_t != 0) {
      --sec_t;
                } else {
                        if (min_t != 0) {
                        --min_t;
                                        } else {
                                                if (hours_t != 0) {
                                                --hours_t;
                                               }
                                          min_t = 59;
                                        }
                        sec_t = 59;
                      }
    if (hours_t == 0 && min_t == 0 && sec_t <= REMAINING_TIME_SIGNAL) tone(SPEAKER_PIN, TONE, 100);
                  }
    draw_led(val, menu_level);  // Выводим таймер на дисплей
 }
  /********************End Of CounterTimer********************/
  TIMSK1 &= ~(1 << OCIE1A); // Сброс Бита OCIE1A (INTERUPT OFF)
}


void writeTime(){
  EEPROM.update(0, hours_t);
  EEPROM.update(1, min_t);
  EEPROM.update(2, sec_t);
}

void readTime(){
  hours_t = EEPROM.read(0);
  min_t   = EEPROM.read(1);
  sec_t   = EEPROM.read(2);
}

void reset() {
  hours_t = 0;
  min_t   = 0;
  sec_t   = 0;
}

void edit_time(int8_t &elem, bool dir) {
  if      (elem == HOURS   &&  dir)  (hours_t > 22)   ? hours_t = 0  : ++hours_t;
  else if (elem == HOURS   && !dir)  (hours_t < 1)    ? hours_t = 23 : --hours_t;
  if      (elem == MINUTES &&  dir)  (min_t   > 58)   ? min_t = 0    : ++min_t;
  else if (elem == MINUTES && !dir)  (min_t   < 1)    ? min_t = 59   : --min_t;
  if      (elem == SECONDS &&  dir)  (sec_t   > 58)   ? sec_t = 0    : ++sec_t;
  else if (elem == SECONDS && !dir)  (sec_t   < 1)    ? sec_t = 59   : --sec_t;
  draw_led(elem, menu_level);
}

void draw_led(int8_t &num, int8_t &level) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(19, 3);
  display.setTextColor(WHITE);
  display.print(str_time(hours_t));
  display.print(":");
  display.print(str_time(min_t));
  display.print(":");
  display.print(str_time(sec_t));

  switch (level) {
    case MAIN:
      if (num > EDIT)  val = START;
      if (num < START) val = EDIT;
      display.setTextSize(1);
      display.setCursor(34, 23);
      if (num == START) {
        display.fillRoundRect(31, 22, 35, 9, 3, WHITE);
        display.setTextColor(BLACK);
        display.print("START");
      } else {
        display.setTextColor(WHITE);
        display.print("START");
      }
      display.print("  ");

      if (num == EDIT) {
        display.fillRoundRect(72, 22, 30, 9, 3, WHITE);
        display.setTextColor(BLACK);
        display.print("EDIT");
      } else {
        display.setTextColor(WHITE);
        display.print("EDIT");
      }
      break;

    case EDIT_MENU:
      if (num > CLEAR)   num = HOURS;
      if (num < HOURS)   num = CLEAR;
      display.setTextSize(1);
      display.setCursor(27, 23);

      if (num == HOURS) {
        display.drawRoundRect(16, 0, 28, 20, 3, WHITE);
      }

      if (num == MINUTES) {
        display.drawRoundRect(52, 0, 28, 20, 3, WHITE);
      }

      if (num == SECONDS) {
        display.drawRoundRect(88, 0, 28, 20, 3, WHITE);
      }

      if (num == APPLY) {
        display.fillRoundRect(24, 22, 35, 9, 3, WHITE);
        display.setTextColor(BLACK);
        display.print("APPLY");
      } else {
        display.setTextColor(WHITE);
        display.print("APPLY");
      }
      display.print("   ");

      if (num == CLEAR) {
        display.fillRoundRect(72, 22, 35, 9, 3, WHITE);
        display.setTextColor(BLACK);
        display.print("CLEAR");
      } else {
        display.setTextColor(WHITE);
        display.print("CLEAR");
      }
      break;

    case EDIT_TIME:
      display.setTextSize(2);
      display.setCursor(19, 3);
      if (num == HOURS) {
        display.fillRoundRect(16, 0, 28, 20, 3, WHITE);
        display.setTextColor(BLACK);
        element_menu = HOURS;
      } else {
        display.setTextColor(WHITE);
      }
      display.print(str_time(hours_t));
      display.setTextColor(WHITE);
      display.print(":");

      if (num == MINUTES) {
        display.fillRoundRect(52, 0, 28, 20, 3, WHITE);
        display.setTextColor(BLACK);
        element_menu = MINUTES;
      } else {
        display.setTextColor(WHITE);
      }
      display.print(str_time(min_t));
      display.setTextColor(WHITE);
      display.print(":");

      if (num == SECONDS) {
        display.fillRoundRect(88, 0, 28, 20, 3, WHITE);
        display.setTextColor(BLACK);
        element_menu = SECONDS;
      } else {
        display.setTextColor(WHITE);
      }
      display.print(str_time(sec_t));
      display.setTextColor(WHITE);

      display.setTextSize(1);
      display.setCursor(27, 23);
      display.print("APPLY");
      display.print("   ");
      display.print("CLEAR");
      break;

    case COUNTDOWN:
      display.setCursor(54, 23);
      display.setTextSize(1);
      display.fillRoundRect(51, 22, 29, 9, 3, WHITE);
      display.setTextColor(BLACK);
      display.print("STOP");
      break;

    case MESSAGE_SCREEN:
      display.clearDisplay();
      display.setCursor(15, 15);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.print(F(MESSAGE));
      break;

    case ALERT:
      display.setTextSize(1);
      display.setCursor(34, 23);
      display.setTextColor(WHITE);
      display.print("START");
      display.drawRoundRect(31, 21, 35, 11, 3, WHITE);
      display.print("  ");
      display.print("EDIT");
      break;
  }
  display.display();
}

String str_time(uint8_t t) {
  String str = "0";
  if (t < 10) str += t;
  else str = t;
  return str;
}
