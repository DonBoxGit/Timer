/******************************************************************************
                                Timer v.1.1
 *                                                                            *
                            by Roman Yakubovskiy
 ******************************************************************************/
#include "timer_se.h"
TimerClock timer;
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

  timer.readRomTime();     // Считываем Время из EEPROM
  draw();
}

void loop() {
  encoder.tick();
  if (encoder.isRight()) {
    tone(SPEAKER_PIN, 150, 20);
    timer.changeTime(TimerClock::Element::HOURS, true);
    draw();
  }
  if (encoder.isLeft()) {
    tone(SPEAKER_PIN, 150, 20);
    timer.changeTime(TimerClock::Element::HOURS, false);
    draw();
  }

  if (encoder.isClick()) {
    tone(SPEAKER_PIN, 250, 40);
  }

}

void draw() {
  // Header of LED Timer
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(19, 3);
  display.setTextColor(WHITE);
  display.print(timer.getTime(TimerClock::Element::HOURS));
  display.print(":");
  display.print(timer.getTime(TimerClock::Element::MINUTES));
  display.print(":");
  display.print(timer.getTime(TimerClock::Element::SECONDS));
  display.display();
}
