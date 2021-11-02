/******************************************************************************
 *                              Timer v.1.1                                   *
 *                                                                            *
 *                          by Roman Yakubovskiy                              *
 ******************************************************************************/
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

#include <EEPROM.h>
#include <Adafruit_SSD1306.h>
#include <GyverEncoder.h>
#include "timer_se.h" 
/*************************Подключаем SSD1306********************************/
Adafruit_SSD1306 display(128, 32, &Wire,   OLED_RESET);
/*************************Подключаем Энкодер********************************/
Encoder encoder(SLK, DT, SW);

void setup() {
  //Serial.begin(9600);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(OUT_PIN, OUTPUT);
  encoder.setType(TYPE2);         // Тип Энкодера
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDR)) {
    //Serial.println(F("SSD1306 allocation failed."));
    for (;;);
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  /***********************Инициализация timer1*************************/
  cli();                      // Отключить Глобольное Прерывание
  TCCR1A = 0x00;              // Устанавка Регистра TCCR1A в 0
  TCCR1B = 0x00;              // Установка Регистра TCCR1B в 0
  OCR1A = 15624;              // Установка Регистра Совпадения
  TCCR1B |= (1 << WGM12);     // Включить CTC - режим(по совпадению)
  TCCR1B |= (1 << CS10);      // Установка Битов CS10 и CS 12 на
  TCCR1B |= (1 << CS12);      // Коэффициент Деления 1024
  // TIMSK1 |= (1 << OCIE1A); // Разрешаем Прерывания по Совпадению
  sei();                      // Включить Глобальное Прерывание
  delay(1000);
  readTime();                 // Считываем Время из EEPROM
  draw_led(val, menu_level);  // Выводим на SSD13006
}

void loop() {
  encoder.tick();
  if (encoder.isRight()) {
    if (menu_level == MAIN) draw_led(++val, menu_level);
    if (menu_level == EDIT_MENU) draw_led(++val, menu_level);
    if (menu_level == EDIT_TIME) edit_time(element_menu, true);
    tone(SPEAKER_PIN, 150, 20);
  }
  if (encoder.isLeft()) {
    if (menu_level == MAIN) draw_led(--val, menu_level);
    if (menu_level == EDIT_MENU) draw_led(--val, menu_level);
    if (menu_level == EDIT_TIME) edit_time(element_menu, false);
    tone(SPEAKER_PIN, 150, 20);
  }

  if (encoder.isClick()) {
    tone(SPEAKER_PIN, 250, 40);
    if (menu_level == MAIN && val == START) {
      menu_level = COUNTDOWN;
      val = STOP;
      countdown_timer();
    } else if (menu_level == MAIN && val == EDIT) {
      menu_level = EDIT_MENU;
      val = HOURS;
      draw_led(val, menu_level);
    } else if (menu_level == EDIT_MENU && val == APPLY) {
      menu_level = MAIN;
      draw_led(val, menu_level);
      writeTime();
    } else if (menu_level == EDIT_MENU && val == CLEAR) {
      resetTime();
      draw_led(val, menu_level);
    } else if (menu_level == EDIT_MENU) {
      menu_level = EDIT_TIME;
      draw_led(val, menu_level);
    } else if (menu_level = EDIT_TIME) {
      menu_level = EDIT_MENU;
      draw_led(val, menu_level);
    }
  }
}
