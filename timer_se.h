#ifndef timer_se_h
#define timer_se_h

#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_SSD1306.h>
#include <GyverEncoder.h>

#define SLK                   3            // Пин SLK(S2) Энкодера
#define DT                    2            // Пин DT(S1)  Энкодера
#define SW                    4            // Пин Кнопки Энкодера
#define OLED_RESET            4            // Пин Сброса Дисплея (Если Нет: -1)
#define DISPLAY_I2C_ADDR      0x3C         // I2C Адрес SSD1306 Экрана
#define SPEAKER_PIN           12           // Пин Пьезоизлучателя
#define OUT_PIN               10           // Пин Реле
#define MESSAGE       "CHICKEN IS COOKED!" // Сообщение Таймера
#define TONE                  1000         // Тон Спикера
#define REMAINING_TIME_SIGNAL 3            // Cигнализация Остатка Времени(3 сек.)
#define MIN_TIME_START        5            // Минимальное Время для Старта Отсчета Времени

uint8_t hours_t = 0;
uint8_t min_t   = 0;
uint8_t sec_t   = 0;
int8_t val = 0;                      // Переменая Счетчик
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
void reset(void);
void countdown_timer(void);
void readTime(void);
void writeTime(void);

#endif
