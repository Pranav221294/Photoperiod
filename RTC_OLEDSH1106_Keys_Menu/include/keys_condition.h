#include <Arduino.h>
// #include <ezButton.h>
// #include <SPI.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
 #include "RTClib.h"


extern DateTime now;
//enum MenuMode { MAIN_SCREEN, PROGRAMMING_MODE };

enum MenuMode { MAIN_SCREEN, PROGRAMMING_MODE };
extern MenuMode currentMode ;

//extern MenuMode currentMode;
extern const long blinkInterval;

#define DEBOUNCE_TIME 50 // Debounce time in milliseconds

void setting_page_oled();
void default_page_oled();
void key_pressing_func();
void initization();
int isLeapYear(int year) ;
String formatWithLeadingZero(int value);
void oled_set_display();
void intilisation_RTC_n_oled();
void loadSettings() ;
void saveSettings() ;
void liveSettings() ;