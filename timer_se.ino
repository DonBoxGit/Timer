/******************************************************************************
 *                               Timer v.1.1                                  *
 *                     A simple timer for kitchen methers                     *
 *                  Add class TimerClock                                      *
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

#include <Adafruit_SSD1306.h>
#include <GyverEncoder.h> 
#include "timerclock.h"
TimerClock timer; // Создаем объект таймера
/***************************Подключаем SSD1306********************************/
Adafruit_SSD1306 display(128, 32, &Wire,   OLED_RESET);
/***************************Подключаем Энкодер********************************/
Encoder encoder(SLK, DT, SW);

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

/**********************Вектро Прерывания TIMER1*****************************/
//ISR (TIMER1_COMPA_vect) {
//  timerClick = true;
//}

int8_t val = 0;
MenuLevel menuLevel       = MAIN;
//ElementTimer elementTimer = START;
//TimerClock::Element elementTimer;
void setup() {
  Serial.begin(9600);
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
  timer.readRomTime();        // Считываем Время из EEPROM
  draw_led(val, menuLevel);
}

void loop() {
  encoder.tick();
  if (encoder.isRight()) {
    if(menuLevel == MAIN)      draw_led(++val, menuLevel);
    if(menuLevel == EDIT_MENU) draw_led(++val, menuLevel);
    if(menuLevel == EDIT_TIME){
      timer.changeTime(timer.elementTimer, true);
      draw_led(val, menuLevel);
    }
    tone(SPEAKER_PIN, 150, 20);
  }
  if (encoder.isLeft()) {
    if(menuLevel == MAIN)      draw_led(--val, menuLevel);
    if(menuLevel == EDIT_MENU) draw_led(--val, menuLevel);
    if(menuLevel == EDIT_TIME){
      timer.changeTime(timer.elementTimer, false);
      draw_led(val, menuLevel);
    }
    tone(SPEAKER_PIN, 150, 20);
  }

  if (encoder.isClick()) {
    if(menuLevel == MAIN && val == EDIT) {
      menuLevel = EDIT_MENU;
      val = HOURS;
      draw_led(val, menuLevel);
    }else if(menuLevel == EDIT_MENU && val == CLEAR){
      timer.resetTime();
      draw_led(val, menuLevel);
    }else if(menuLevel == EDIT_MENU && val == APPLY){
      menuLevel = MAIN;
      draw_led(val, menuLevel);
    }else if(menuLevel == EDIT_MENU && val == HOURS){
      timer.elementTimer = TimerClock::Element::HOURS;
      menuLevel = EDIT_TIME;
      draw_led(val, menuLevel);
    }else if(menuLevel == EDIT_MENU && val == MINUTES){
      timer.elementTimer = TimerClock::Element::MINUTES;
      menuLevel = EDIT_TIME;
      draw_led(val, menuLevel);
    }else if(menuLevel == EDIT_MENU && val == SECONDS){
      timer.elementTimer = TimerClock::Element::SECONDS;
      menuLevel = EDIT_TIME;
      draw_led(val, menuLevel);
    }else if(menuLevel == EDIT_TIME){
      menuLevel = EDIT_MENU;
      draw_led(val, menuLevel);
    }
    tone(SPEAKER_PIN, 250, 40);
  }

}

void header(void){
  // Header of TimerClock
  //display.drawRoundRect(0, 0, 128, 32, 0, WHITE);
  display.setTextSize(2);
  display.setCursor(16, 3);
  display.setTextColor(WHITE);
  display.print(timer.getTime(timer.HOURS));
  display.print(":");
  display.print(timer.getTime(timer.MINUTES));
  display.print(":");
  display.print(timer.getTime(timer.SECONDS)); 
}

void draw_led(int8_t &param, const MenuLevel &level) {  
  display.clearDisplay();
  // Dynamic footer
  switch(level){
    case MAIN:
      header();
      if(param > EDIT)  param = START;
      if(param < START) param = EDIT;
      display.setTextSize(1);
      display.setCursor(31, 23);
      if(param == START){
        display.fillRoundRect(28, 22, 35, 9, 3, WHITE);
        display.setTextColor(BLACK);
        display.print("START");
      }else{
        display.setTextColor(WHITE);
        display.print("START");
      }
      display.print("  ");

      if(param == EDIT){
        display.fillRoundRect(69, 22, 30, 9, 3, WHITE);
        display.setTextColor(BLACK);
        display.print("EDIT");
      }else{
        display.setTextColor(WHITE);
        display.print("EDIT");
      }
    break;
    
    case EDIT_MENU:
      header();
      if(param > CLEAR) param = HOURS;
      if(param < HOURS) param = CLEAR;
      display.setTextSize(1);
      display.setCursor(25, 23);

      if(param == HOURS)   display.drawRoundRect(14, 0, 27, 20, 3, WHITE);
      if(param == MINUTES) display.drawRoundRect(50, 0, 27, 20, 3, WHITE);
      if(param == SECONDS) display.drawRoundRect(86, 0, 27, 20, 3, WHITE);

      if(param == APPLY){
        display.fillRoundRect(22, 22, 35, 9, 3, WHITE);
        display.setTextColor(BLACK);
        display.print("APPLY");
      }else{
        display.setTextColor(WHITE);
        display.print("APPLY");
      }
      display.print("   ");

      if(param == CLEAR){
        display.fillRoundRect(70, 22, 35, 9, 3, WHITE);
        display.setTextColor(BLACK);
        display.print("CLEAR");
      }else{
        display.setTextColor(WHITE);
        display.print("CLEAR");
      }
    break;

    case EDIT_TIME:
      display.setTextSize(2);
      display.setCursor(16, 3);
      if(param == HOURS){
        display.fillRoundRect(13, 0, 28, 20, 3, WHITE);
        display.setTextColor(BLACK);
      }else{
        display.setTextColor(WHITE);
      }
      display.print(timer.getTime(timer.Element::HOURS));
      display.setTextColor(WHITE);
      display.print(":");

      if(param == MINUTES){
        display.fillRoundRect(49, 0, 28, 20, 3, WHITE);
        display.setTextColor(BLACK);
      }else{
        display.setTextColor(WHITE);
      }
      display.print(timer.getTime(timer.Element::MINUTES));
      display.setTextColor(WHITE);
      display.print(":");
      
      if(param == SECONDS){
        display.fillRoundRect(85, 0, 28, 20, 3, WHITE);
        display.setTextColor(BLACK);
      }else{
        display.setTextColor(WHITE);
      }
      display.print(timer.getTime(timer.Element::SECONDS));
      display.setTextColor(WHITE);
      // Drawing the fake footer for the beautiful vision
      display.setTextSize(1);
      display.setCursor(25, 23);
      display.print("APPLY");
      display.print("   ");
      display.print("CLEAR");  
    break;

    case COUNTDOWN:
       header();
    break;

    case MESSAGE_SCREEN:

    break;

    case ALERT:

    break;
  }
  display.display();
}
