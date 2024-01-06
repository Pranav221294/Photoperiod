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

//SETTING_11 SET_VARIABLES;
#define DEBOUNCE_TIME 50 // Debounce time in milliseconds

typedef struct _SETTING_11
          {
            int SET_CLKHRS = 0;
            int SET_CLKMIN= 0;
            int SET_CLKSEC= 0;
            int SET_DAY= 0;
            int SET_MONTH= 0;
            int SET_YEAR= 0;
            int SET_START_CLK_HRS = 0;
            int SET_START_CLK_MIN = 0;
            int SET_START_CLK_SEC = 0; 
            int SET_PHOTO_CLK_HRS = 0;
            int SET_PHOTO_CLK_MIN = 0;
            int SET_PHOTO_CLK_SEC = 0; 
            int SET_DURATION = 0;
            int SET_STOP_CLK_HRS = 0;
            int SET_STOP_CLK_MIN = 0;
            int SET_STOP_CLK_SEC = 0; 
           // int SET_DURATION = 0;

          } SETTING_11;
          
//   SETTING_11 SET_VARIABLES;
//   SETTING_11 STORE_VARIABLES;
//   SETTING_11 LIVE_VARIABLES;
//   SETTING_11 COMP_VARIABLES;




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
void Ent_key();
void inc_key();
void dec_key();
void CLK_SET_OLED();
void YEAR_SET_OLED();
void STARTCLK_SET_OLED();
void PHOTO_SET_OLED();
void calculateStopTime(int start_hours, int start_minutes, int photo_hours, int photo_minutes, int *stop_hours, int *stop_minutes);
SETTING_11 get_func_stop_hrs_n_stop_min();