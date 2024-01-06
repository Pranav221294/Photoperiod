#include <Arduino.h>
#include <ezButton.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"
#include <Preferences.h>
#include "keys_condition.h"
#include <U8x8lib.h> 

extern ezButton Prog_Button; // Programming button
extern ezButton Ent_Button; // Enter button
extern ezButton Inc_Button; // Increment button      
extern ezButton Dec_Button; // Decrement button


extern RTC_DS3231 rtc;
extern unsigned long lastBlinkMillis;
//extern const long blinkInterval;
extern bool blinkOn;
extern U8X8_SH1106_128X64_NONAME_HW_I2C disp;
// put function declarations here:
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 100;

SETTING_11 COMP_VARIABLES;

void setup() 
{
  
 initization();


}

void loop()
{
  Prog_Button.loop();
  Ent_Button.loop();
  Inc_Button.loop();
  Dec_Button.loop();

  now = rtc.now();
  unsigned long currentTime = millis();
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastBlinkMillis >= blinkInterval) 
  {
    lastBlinkMillis = currentMillis;
    blinkOn = !blinkOn; // Toggle the blinking state
  }

  if (currentMode == MAIN_SCREEN)
  {
      if (currentTime - lastUpdateTime >= 1000)
      {
       lastUpdateTime = currentTime; 
      // disp.clearDisplay();  
       default_page_oled();
      
      }
  } 
  
  else if (currentMode == PROGRAMMING_MODE) 
  
  {
    

  //   disp.clear();
   
    //setting_page_oled();
     if (currentTime - lastUpdateTime >= updateInterval)
     {
         lastUpdateTime = currentTime;
         
        //oled_set_display();
         CLK_SET_OLED();
         YEAR_SET_OLED();
         STARTCLK_SET_OLED();
         PHOTO_SET_OLED();
     
     }
  }
 

  key_pressing_func();
  Ent_key();
  inc_key();
  dec_key();

}


void initization()
{

  Serial.begin(115200);
  Prog_Button.setDebounceTime(DEBOUNCE_TIME);
  Ent_Button.setDebounceTime(DEBOUNCE_TIME);
  Inc_Button.setDebounceTime(DEBOUNCE_TIME);
  Dec_Button.setDebounceTime(DEBOUNCE_TIME);
 //\\ currentMode = MAIN_SCREEN;
 //Inc_Button.setEnableContinuousPress(1000); // Enable continuous press after 1000 milliseconds
 //Dec_Button.setEnableContinuousPress(1000); // Enable continuous press after 1000 milliseconds

 
   intilisation_RTC_n_oled();
    loadSettings(); 
   COMP_VARIABLES = get_func_stop_hrs_n_stop_min();

  Serial.println("COMP_VARIABLES: " + String(COMP_VARIABLES.SET_STOP_CLK_HRS));
  Serial.println("COMP_VARIABLES: " + String(COMP_VARIABLES.SET_STOP_CLK_MIN));
  //rtc.adjust(DateTime(__DATE__, __TIME__));

  
  disp.clear();
     disp.setCursor(0, 6);
     disp.print(F("01/23/45   23:45")); 
    disp.setCursor(0, 7);
    disp.print(F("Prog Mode")); 
  // disp.setCursor(0, 2);
  // disp.print(F("0123456789012345")); 
  // disp.setCursor(0, 3);
  // disp.print(F("0123456789012345")); 
  // disp.setCursor(0, 4);
  // disp.print(F("0123456789012345")); 
  // disp.setCursor(0, 5);
  // disp.print(F("0123456789012345")); 
  // disp.setCursor(0, 6);
  // disp.print(F("0123456789012345")); 
  // disp.setCursor(0, 7);
  // disp.print(F("0123456789012345")); 
   disp.setFont(u8x8_font_amstrad_cpc_extended_r);
   disp.draw1x2String(0, 0, "01:23:45   90:12");
   disp.draw1x2String(0, 3, "COL=023    143%");
  // disp.draw1x2String(0, , "0123456789012345");
   //disp.draw1x2String(0, 4, "0123456789012345");
   //disp.draw1x2String(0, 6, "0123456789012345");
  // disp.draw1x2String(0, 4, "0123456789012345");
  // disp.draw1x2String(0, 1, "0123456789012345");
// disp.draw1x2String(0, 3, (hourStr + ':' + minuteStr + ':' + secondStr).c_str());

  delay(3000);
  disp.clear();


}
