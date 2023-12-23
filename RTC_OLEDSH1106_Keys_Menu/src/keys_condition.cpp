#include <Arduino.h>
#include <ezButton.h>
#include <SPI.h>
#include <Wire.h>
#include "RTClib.h"
#include "keys_condition.h"
#include <U8x8lib.h> 
#include <Preferences.h>
// ezButton Prog_Button(16); // Programming button
// ezButton Ent_Button(17); // Enter button
// ezButton Inc_Button(18); // Increment button
// ezButton Dec_Button(19); // Decrement button

enum  SET{ 
           CLK_HRS, 
           CLK_MIN,
           CLK_SEC,
           DAY,
           MONTH,
           YEAR,
           START_CLK_HRS,
           START_CLK_MIN,
           START_CLK_SEC,
           PHOTO_CLK_HRS,
           PHOTO_CLK_MIN,
           PHOTO_CLK_SEC,
           STOP_CLK_HRS,
           STOP_CLK_MIN,
           STOP_CLK_SEC,
           DURATION
           };

SET setting = CLK_HRS;
typedef struct
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

          } SETTING;
          
  SETTING SET_VARIABLES;
  SETTING STORE_VARIABLES;
  SETTING LIVE_VARIABLES;

int startTimeHour = 0;
int startTimeMinute = 0;
int startTimeSecond = 0;
int duration = 0;



unsigned long lastButtonPress = 0;
unsigned long buttonPressInterval = 1000; // Adjust this interval as needed (in milliseconds)

typedef struct
   {
     int inc_counter;
     int dec_counter;
     bool inc_f;
     bool dec_f;
   }Counter;
  
typedef struct
   {
     unsigned long pressedTime  = 0;
     unsigned long releasedTime = 0;
     bool isPressing = false;
     bool isLongDetected = false;
     long pressDuration;
   }switch_k;

  Counter Count_up;
  Counter Decrement;
  switch_k Prog_key_1;


U8X8_SH1106_128X64_NONAME_HW_I2C disp(U8X8_PIN_NONE);  // use this line for 1.3" OLED


#define OLED_Address 0x3C
#define SDA_2 33
#define SCL_2 32



const int SHORT_PRESS_TIME = 1000; // 1000 milliseconds
const int LONG_PRESS_TIME = 3000; // 1000 milliseconds

ezButton Prog_Button(4); // Programming button
ezButton Ent_Button(17); // Enter button
ezButton Inc_Button(18); // Increment button
ezButton Dec_Button(19); // Decrement button

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; 

bool enterButtonPressed = false;
unsigned long enterButtonPressTime = 0;
const unsigned long longPressThreshold = 1000; // Set the long-press threshold time (in milliseconds)
bool blinkOn = true;
unsigned long lastBlinkMillis = 0;
const long blinkInterval = 500;
DateTime now;


 //enum MenuMode { MAIN_SCREEN, PROGRAMMING_MODE };
 //MenuMode currentMode = MAIN_SCREEN;
//typedef enum MenuMode { MAIN_SCREEN, PROGRAMMING_MODE };
//MenuMode currentMode = MAIN_SCREEN;

MenuMode currentMode = MAIN_SCREEN;
Preferences preferences;


void default_page_oled()
{
 
DateTime now = rtc.now();

// Update the LIVE_VARIABLES structure with live time values
LIVE_VARIABLES.SET_CLKHRS = now.hour();
LIVE_VARIABLES.SET_CLKMIN = now.minute();
LIVE_VARIABLES.SET_CLKSEC = now.second();
LIVE_VARIABLES.SET_DAY = now.day();
LIVE_VARIABLES.SET_MONTH = now.month();
LIVE_VARIABLES.SET_YEAR = now.year();
LIVE_VARIABLES.SET_START_CLK_HRS = SET_VARIABLES.SET_START_CLK_HRS ;
LIVE_VARIABLES.SET_START_CLK_MIN = SET_VARIABLES.SET_START_CLK_MIN;
LIVE_VARIABLES.SET_START_CLK_SEC = SET_VARIABLES.SET_START_CLK_SEC;
LIVE_VARIABLES.SET_PHOTO_CLK_HRS = SET_VARIABLES.SET_PHOTO_CLK_HRS;
LIVE_VARIABLES.SET_PHOTO_CLK_MIN = SET_VARIABLES.SET_PHOTO_CLK_MIN;
//LIVE_VARIABLES.SET_STOP_CLK_SEC = SET_VARIABLES.SET_STOP_CLK_SEC;





// Format the time with leading zeros
String hourStr = (LIVE_VARIABLES.SET_CLKHRS < 10) ? "0" + String(LIVE_VARIABLES.SET_CLKHRS) : String(LIVE_VARIABLES.SET_CLKHRS);
String minuteStr = (LIVE_VARIABLES.SET_CLKMIN < 10) ? "0" + String(LIVE_VARIABLES.SET_CLKMIN) : String(LIVE_VARIABLES.SET_CLKMIN);
String secondStr = (LIVE_VARIABLES.SET_CLKSEC < 10) ? "0" + String(LIVE_VARIABLES.SET_CLKSEC) : String(LIVE_VARIABLES.SET_CLKSEC);

// Format the date with leading zeros
String dayStr = (LIVE_VARIABLES.SET_DAY < 10) ? "0" + String(LIVE_VARIABLES.SET_DAY) : String(LIVE_VARIABLES.SET_DAY);
String monthStr = (LIVE_VARIABLES.SET_MONTH < 10) ? "0" + String(LIVE_VARIABLES.SET_MONTH) : String(LIVE_VARIABLES.SET_MONTH);
String yearStr = String(LIVE_VARIABLES.SET_YEAR);

if (yearStr.length() > 2) 
{
    yearStr = yearStr.substring(2);
}
 else 
{
    // Handle the case where the string has 2 or fewer characters
    // This depends on your specific requirements. You might want to leave it unchanged or handle it differently.
}

String StarthourStr = (LIVE_VARIABLES.SET_START_CLK_HRS < 10) ? "0" + String(LIVE_VARIABLES.SET_START_CLK_HRS) : String(LIVE_VARIABLES.SET_START_CLK_HRS);
String StartminuteStr = (LIVE_VARIABLES.SET_START_CLK_MIN < 10) ? "0" + String(LIVE_VARIABLES.SET_START_CLK_MIN) : String(LIVE_VARIABLES.SET_START_CLK_MIN);
//String StartsecondStr = (LIVE_VARIABLES.SET_START_CLK_SEC < 10) ? "0" + String(LIVE_VARIABLES.SET_START_CLK_SEC) : String(LIVE_VARIABLES.SET_START_CLK_SEC);

String photohourStr = (LIVE_VARIABLES.SET_PHOTO_CLK_HRS < 10) ? "0" + String(LIVE_VARIABLES.SET_PHOTO_CLK_HRS) : String(LIVE_VARIABLES.SET_PHOTO_CLK_HRS);
String photominuteStr = (LIVE_VARIABLES.SET_PHOTO_CLK_MIN < 10) ? "0" + String(LIVE_VARIABLES.SET_PHOTO_CLK_MIN) : String(LIVE_VARIABLES.SET_PHOTO_CLK_MIN);
//String StopsecondStr = (LIVE_VARIABLES.SET_STOP_CLK_SEC < 10) ? "0" + String(LIVE_VARIABLES.SET_STOP_CLK_SEC) : String(LIVE_VARIABLES.SET_STOP_CLK_SEC);

// Set the font for the display
//disp.setFont(u8x8_font_victoriabold8_r);
// disp.setFont(u8x8_font_amstrad_cpc_extended_r);
String dateString = dayStr + '/' + monthStr + '/' + yearStr +' '+' '+StarthourStr + ':' + StartminuteStr  ;
String timeString = hourStr + ':' + minuteStr + ':' + secondStr;
String StarttimeString = StarthourStr + ':' + StartminuteStr;// + ':' + StartsecondStr;
String phototimeString = photohourStr + ':' + photominuteStr ;//+ ':' + StopsecondStr;
// // Display the formatted date on the first line  
// disp.draw1x2String(0, 0, (dayStr + '-' + monthStr + '-' + yearStr).c_str());
// disp.draw1x2String(0, 3, (hourStr + ':' + minuteStr + ':' + secondStr).c_str());
// disp.setCursor(0, 0);
// disp.print(F(dateString.c_str()));
// disp.setCursor(0, 1);
// disp.print(F(timeString.c_str()));
// disp.setCursor(0, 2);
// disp.print(F(StarttimeString.c_str()));
// disp.setCursor(0, 3);
// disp.print(F(StoptimeString.c_str()));

disp.setFont(u8x8_font_amstrad_cpc_extended_r);
disp.draw1x2String(0, 0, timeString.c_str());

disp.setFont(u8x8_font_amstrad_cpc_extended_r);
disp.draw1x2String(10, 0, phototimeString.c_str());

 disp.setCursor(0, 6);
 disp.print(F(dateString.c_str())); 

disp.setCursor(10, 6);
 disp.print(F(StarttimeString.c_str())); 

//disp.draw1x2String(0, 0, "90:12");
//disp.draw1x2String(0, 3, "COL=023    143%");





}


void key_pressing_func()
{
  if (Prog_Button.isPressed()) 
  {
   Prog_key_1.pressedTime = millis();
    Prog_key_1.isPressing = true;
    Prog_key_1.isLongDetected = false;
  }

  if (Prog_Button.isReleased()) 
  {
    Prog_key_1.isPressing = false;
    Prog_key_1.releasedTime = millis();

    Prog_key_1.pressDuration = Prog_key_1.releasedTime - Prog_key_1.pressedTime;
    
    if ( Prog_key_1.pressDuration< SHORT_PRESS_TIME )
      {
    
      Serial.println("A short press is detected");
      currentMode = MAIN_SCREEN;
      disp.clearDisplay();
      saveSettings() ;
      liveSettings() ;
      }
   }
    
    if (Prog_key_1.isPressing == true && Prog_key_1.isLongDetected == false) 
    {
    long pressDuration = millis() - Prog_key_1.pressedTime;

    if ( pressDuration > LONG_PRESS_TIME ) 
    {
      Serial.println("A long press is detected");
       Prog_key_1.isLongDetected = true;
        currentMode = PROGRAMMING_MODE;
        setting = CLK_HRS;
       // loadSettings() ;
       liveSettings() ;
        disp.clearDisplay();
    }
  }
}


int isLeapYear(int year) 
{
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) 
    {
        return 1; // Leap year
    } else 
    {
        return 0; // Not a leap year
    }
}

void CLK_SET_OLED()
{
   String tempCLKHRS = formatWithLeadingZero(SET_VARIABLES.SET_CLKHRS);
   String tempCLKMIN = formatWithLeadingZero(SET_VARIABLES.SET_CLKMIN);
   String tempCLKSEC = formatWithLeadingZero(SET_VARIABLES.SET_CLKSEC);
   String displayCLKHRS;
   String displayCLKMIN;
   String displayCLKSEC;

    if (setting == CLK_HRS && blinkOn) 
     {
        displayCLKHRS = "  ";
     } 
    else 
    {
        displayCLKHRS = tempCLKHRS;
    }
    //////////////////////////////
    
     if (setting == CLK_MIN && blinkOn) 
     {
        displayCLKMIN = "  ";
      } 
    else 
    {
        displayCLKMIN = tempCLKMIN;
    }
   ///////////////////////////////////////
   
    if (setting == CLK_SEC && blinkOn) 
    {
        displayCLKSEC = "  ";
    } 
    else 
    {
        displayCLKSEC = tempCLKSEC;
    }
    
    
    String clockSettings = displayCLKHRS + ":" + displayCLKMIN + ":" + displayCLKSEC;
    disp.draw1x2String(0, 0, (clockSettings).c_str());
/////////////////////////////////////



}
void YEAR_SET_OLED()
{
  
  String tempDAY =    formatWithLeadingZero(SET_VARIABLES.SET_DAY);
  String tempMONTH =  formatWithLeadingZero(SET_VARIABLES.SET_MONTH);
  String tempYEAR =   formatWithLeadingZero(SET_VARIABLES.SET_YEAR);
  
   if (tempYEAR.length() > 2) 
{
    tempYEAR = tempYEAR.substring(2);
}
 else 
{
    // Handle the case where the string has 2 or fewer characters
    // This depends on your specific requirements. You might want to leave it unchanged or handle it differently.
}


   String displayDAY;  
   String displayMONTH;
   String displayYEAR;
    // Handle DAY setting
   
    if (setting == DAY && !blinkOn) 
    {
        displayDAY = "  ";
    } 
    else 
    {
        displayDAY = tempDAY;
    }
   

    // Handle MONTH setting
    
    if (setting == MONTH && !blinkOn) 
    {
        displayMONTH = "  ";
    } 
    else 
    {
        displayMONTH = tempMONTH;
    }
   
    // Handle YEAR setting
    
    if (setting == YEAR && !blinkOn) 
    {
        displayYEAR = "    ";
    } 
    else 
    {
        displayYEAR = tempYEAR;
    }

  String dateSettings = displayDAY + '/' + displayMONTH + '/' + displayYEAR;

disp.setFont(u8x8_font_amstrad_cpc_extended_r);
disp.setCursor(0, 6);
disp.print(dateSettings.c_str());

}
void STARTCLK_SET_OLED()
{
    String tempSTART_CLK_HRS=   formatWithLeadingZero(SET_VARIABLES.SET_START_CLK_HRS);
    String tempSTART_CLK_MIN=   formatWithLeadingZero(SET_VARIABLES.SET_START_CLK_MIN);
   // String tempSTART_CLK_SEC=   formatWithLeadingZero(SET_VARIABLES.SET_START_CLK_SEC);

    String displaySTART_CLK_HRS;
    String displaySTART_CLK_MIN;
    
    if (setting == START_CLK_HRS && blinkOn) 
    {
        displaySTART_CLK_HRS = "  ";
    } 
    else 
    {
        displaySTART_CLK_HRS = tempSTART_CLK_HRS;
    }
    
       
    if (setting == START_CLK_MIN && blinkOn) 
    {
        displaySTART_CLK_MIN = "  ";
    } 
    else 
    {
        displaySTART_CLK_MIN = tempSTART_CLK_MIN;
    }
  String startClockSettings = displaySTART_CLK_HRS + ':' + displaySTART_CLK_MIN;
    
   disp.setFont(u8x8_font_amstrad_cpc_extended_r);
   disp.setCursor(10, 6);
   disp.print(startClockSettings.c_str()); 
}
void PHOTO_SET_OLED()
{
   String tempPHOTO_CLK_HRS=   formatWithLeadingZero(SET_VARIABLES.SET_PHOTO_CLK_HRS);
   String tempPHOTO_CLK_MIN =   formatWithLeadingZero(SET_VARIABLES.SET_PHOTO_CLK_MIN);
   //String tempPHOTO_CLK_SEC=   formatWithLeadingZero(SET_VARIABLES.SET_STOP_CLK_SEC);
  String displayPHOTO_CLK_HRS;
  String displayPHOTO_CLK_MIN;
    
    if (setting == PHOTO_CLK_HRS && blinkOn) 
    {
        displayPHOTO_CLK_HRS = "  ";
    } 
    else 
    {
        displayPHOTO_CLK_HRS = tempPHOTO_CLK_HRS;
    }
    

    

    // Handle START_CLK_MIN setting
    
    if (setting == PHOTO_CLK_MIN && blinkOn) 
    {
        displayPHOTO_CLK_MIN = "  ";
    } 
    else 
    {
        displayPHOTO_CLK_MIN = tempPHOTO_CLK_MIN;
    }
    
  String photoSettings = displayPHOTO_CLK_HRS + ":" + displayPHOTO_CLK_MIN;
  disp.draw1x2String(10, 0, (photoSettings).c_str());
}
void oled_set_display()
{
//      disp.setCursor(0, 0);

//     // Create temporary strings to hold the values
//     String tempCLKHRS = formatWithLeadingZero(SET_VARIABLES.SET_CLKHRS);
//     String tempCLKMIN = formatWithLeadingZero(SET_VARIABLES.SET_CLKMIN);
//     String tempCLKSEC = formatWithLeadingZero(SET_VARIABLES.SET_CLKSEC);
//     String tempDAY =    formatWithLeadingZero(SET_VARIABLES.SET_DAY);
//     String tempMONTH =  formatWithLeadingZero(SET_VARIABLES.SET_MONTH);
//     String tempYEAR =   formatWithLeadingZero(SET_VARIABLES.SET_YEAR);
//    // String tempYEAR =   formatWithLeadingZero(SET_VARIABLES.SET_YEAR);
//     String tempSTART_CLK_HRS=   formatWithLeadingZero(SET_VARIABLES.SET_START_CLK_HRS);
//     String tempSTART_CLK_MIN=   formatWithLeadingZero(SET_VARIABLES.SET_START_CLK_MIN);
//     String tempSTART_CLK_SEC=   formatWithLeadingZero(SET_VARIABLES.SET_START_CLK_SEC);
//     String tempSTOP_CLK_HRS=   formatWithLeadingZero(SET_VARIABLES.SET_STOP_CLK_HRS);
//     String tempSTOP_CLK_MIN=   formatWithLeadingZero(SET_VARIABLES.SET_STOP_CLK_MIN);
//     String tempSTOP_CLK_SEC=   formatWithLeadingZero(SET_VARIABLES.SET_STOP_CLK_SEC);
//     //String tempDURATION = formatWithLeadingZero(SET_VARIABLES.SET_DURATION);
    
//      String displayCLKHRS;
//     if (setting == CLK_HRS && blinkOn) 
//      {
//         displayCLKHRS = "  ";
//      } 
//     else 
//     {
//         displayCLKHRS = tempCLKHRS;
//     }
//      disp.print(displayCLKHRS);
//      /////////////////////////////////////////////
//      disp.print(":");

//      String displayCLKMIN;
//      if (setting == CLK_MIN && blinkOn) 
//      {
//         displayCLKMIN = "  ";
//       } 
//     else 
//     {
//         displayCLKMIN = tempCLKMIN;
//     }
//      disp.print(displayCLKMIN);
// /////////////////////////////////////////////////////
//   disp.print(":");
//       String displayCLKSEC;
//     if (setting == CLK_SEC && blinkOn) 
//     {
//         displayCLKSEC = "  ";
//     } 
//     else 
//     {
//         displayCLKSEC = tempCLKSEC;
//     }
//     disp.print(displayCLKSEC);
// /////////////////////////////////////////////////////////////////
//     disp.setCursor(0, 10);

//     // Handle DAY setting
//     String displayDAY;
//     if (setting == DAY && !blinkOn) 
//     {
//         displayDAY = "  ";
//     } 
//     else 
//     {
//         displayDAY = tempDAY;
//     }
//     disp.print(displayDAY);

//     disp.print("-");

//     // Handle MONTH setting
//     String displayMONTH;
//     if (setting == MONTH && !blinkOn) 
//     {
//         displayMONTH = "  ";
//     } 
//     else 
//     {
//         displayMONTH = tempMONTH;
//     }
//     disp.print(displayMONTH);

//     disp.print("-");

//     // Handle YEAR setting
//     String displayYEAR;
//     if (setting == YEAR && !blinkOn) 
//     {
//         displayYEAR = "    ";
//     } 
//     else 
//     {
//         displayYEAR = tempYEAR;
//     }
//     disp.print(displayYEAR);
// ////////////////////////////////////////////////
//     disp.setCursor(0, 20);

//     // Handle START_CLK_HRS setting
//     String displaySTART_CLK_HRS;
//     if (setting == START_CLK_HRS && blinkOn) 
//     {
//         displaySTART_CLK_HRS = "  ";
//     } 
//     else 
//     {
//         displaySTART_CLK_HRS = tempSTART_CLK_HRS;
//     }
//     disp.print(displaySTART_CLK_HRS);

//     disp.print(":");

//     // Handle START_CLK_MIN setting
//     String displaySTART_CLK_MIN;
//     if (setting == START_CLK_MIN && blinkOn) 
//     {
//         displaySTART_CLK_MIN = "  ";
//     } 
//     else 
//     {
//         displaySTART_CLK_MIN = tempSTART_CLK_MIN;
//     }
//     disp.print(displaySTART_CLK_MIN);

//     disp.print(":");

//     // Handle START_CLK_SEC setting
//     String displaySTART_CLK_SEC;
//     if (setting == START_CLK_SEC && blinkOn) 
//     {
//         displaySTART_CLK_SEC = "  ";
//     } 
//     else 
//     {
//         displaySTART_CLK_SEC = tempSTART_CLK_SEC;
//     }
//     disp.print(displaySTART_CLK_SEC);
// //////////////////////////////////////////////////////////////////////////////////
//     disp.setCursor(0, 30);
//     String displaySTOP_CLK_HRS;
//     if (setting == STOP_CLK_HRS && blinkOn) 
//     {
//         displaySTOP_CLK_HRS = "  ";
//     } 
//     else 
//     {
//         displaySTOP_CLK_HRS = tempSTOP_CLK_HRS;
//     }
//     disp.print(displaySTOP_CLK_HRS);

//     disp.print(":");

//     // Handle START_CLK_MIN setting
//     String displaySTOP_CLK_MIN;
//     if (setting == STOP_CLK_MIN && blinkOn) 
//     {
//         displaySTOP_CLK_MIN = "  ";
//     } 
//     else 
//     {
//         displaySTOP_CLK_MIN = tempSTOP_CLK_MIN;
//     }
//     disp.print(displaySTOP_CLK_MIN);

//     disp.print(":");

//     // Handle START_CLK_SEC setting
//     String displaySTOP_CLK_SEC;
//     if (setting == STOP_CLK_SEC && blinkOn) 
//     {
//         displaySTOP_CLK_SEC = "  ";
//     } 
//     else 
//     {
//         displaySTOP_CLK_SEC = tempSTOP_CLK_SEC;
//     }
//     disp.print(displaySTOP_CLK_SEC);

//     // Handle DURATION setting
//     // String displayDURATION;
//     // if (setting == DURATION && !blinkOn) 
//     // {
//     //     displayDURATION = "  ";
//     // } 
//     // else 
//     // {
//     //     displayDURATION = tempDURATION;
//     // }
//     // disp.print(displayDURATION);

    
    
  }

  String formatWithLeadingZero(int value)
  {
    // Format the value with a leading zero if it's a single digit
    if (value < 10) 
    {
        return "0" + String(value);
    } 
    else 
    {
        return String(value);
    }
}
       

void intilisation_RTC_n_oled()
{
	Wire1.begin(SDA_2, SCL_2);  // Use Wire1 for RTC communication

  if (!rtc.begin(&Wire1)) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) 
  {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  disp.begin();
  disp.setI2CAddress(OLED_Address << 1);  // I2C address multiplied by 2, the lowest bit must be zero
  disp.setFont(u8x8_font_chroma48medium8_r);

  Serial.println(F("RTC and OLED Test"));
	
	
	
}

void loadSettings() 
{
  preferences.begin("settings", true);

//   SET_VARIABLES.SET_CLKHRS = now.hour();
//   SET_VARIABLES.SET_CLKMIN = now.minute();
//   SET_VARIABLES.SET_CLKSEC = now.second();
//   SET_VARIABLES.SET_DAY =    now.day();
//   SET_VARIABLES.SET_MONTH =  now.month();
//   SET_VARIABLES.SET_YEAR =   now.year();

  SET_VARIABLES.SET_CLKHRS = preferences.getInt("SET_CLKHRS", 0);
  SET_VARIABLES.SET_CLKMIN = preferences.getInt("SET_CLKMIN", 0);
  SET_VARIABLES.SET_CLKSEC = preferences.getInt("SET_CLKSEC", 0);
  SET_VARIABLES.SET_DAY = preferences.getInt("SET_DAY", 0); // Replace with your default day value
  SET_VARIABLES.SET_MONTH = preferences.getInt("SET_MONTH", 0); // Replace with your default month value
  SET_VARIABLES.SET_YEAR = preferences.getInt("SET_YEAR", 0); // Replace with your default year value
  SET_VARIABLES.SET_START_CLK_HRS = preferences.getInt("START_HRS", 0);
  SET_VARIABLES.SET_START_CLK_MIN = preferences.getInt("START_MIN", 0);
  //SET_VARIABLES.SET_START_CLK_SEC = preferences.getInt("START_SEC", 0);
  SET_VARIABLES.SET_PHOTO_CLK_HRS = preferences.getInt("PHOTO_HRS", 0); // Corrected key
  SET_VARIABLES.SET_PHOTO_CLK_MIN = preferences.getInt("PHOTO_MIN", 0); // Corrected key
  //SET_VARIABLES.SET_STOP_CLK_SEC = preferences.getInt("STOP_SEC", 0); // Corrected key

  //SET_VARIABLES.SET_DURATION = preferences.getInt("SET_DURATION", 0);


  preferences.end();

//   DateTime rtcTime(SET_VARIABLES.SET_YEAR, SET_VARIABLES.SET_MONTH, SET_VARIABLES.SET_DAY,
//                    SET_VARIABLES.SET_CLKHRS, SET_VARIABLES.SET_CLKMIN, SET_VARIABLES.SET_CLKSEC);
//   rtc.adjust(rtcTime);
// 
}


/// @brief 
void saveSettings() 
{
  preferences.begin("settings", false);

  preferences.putInt("SET_CLKHRS", SET_VARIABLES.SET_CLKHRS);
  preferences.putInt("SET_CLKMIN", SET_VARIABLES.SET_CLKMIN);
  preferences.putInt("SET_CLKSEC", SET_VARIABLES.SET_CLKSEC);
  preferences.putInt("SET_DAY", SET_VARIABLES.SET_DAY);
  preferences.putInt("SET_MONTH", SET_VARIABLES.SET_MONTH);
  preferences.putInt("SET_YEAR", SET_VARIABLES.SET_YEAR);
  preferences.putInt("START_HRS", SET_VARIABLES.SET_START_CLK_HRS);
  preferences.putInt("START_MIN", SET_VARIABLES.SET_START_CLK_MIN);
  //preferences.putInt("START_SEC", SET_VARIABLES.SET_START_CLK_SEC);
  preferences.putInt("PHOTO_HRS", SET_VARIABLES.SET_PHOTO_CLK_HRS);
  preferences.putInt("PHOTO_MIN", SET_VARIABLES.SET_PHOTO_CLK_MIN);
  //preferences.putInt("STOP_SEC", SET_VARIABLES.SET_STOP_CLK_SEC);
 // preferences.putInt("SET_DURATION", SET_VARIABLES.SET_DURATION);

  preferences.end();

  DateTime rtcTime(SET_VARIABLES.SET_YEAR, SET_VARIABLES.SET_MONTH, SET_VARIABLES.SET_DAY,
                   SET_VARIABLES.SET_CLKHRS, SET_VARIABLES.SET_CLKMIN, SET_VARIABLES.SET_CLKSEC);
  rtc.adjust(rtcTime);
}

void liveSettings() 
{
  preferences.begin("settings", true);

  SET_VARIABLES.SET_CLKHRS = now.hour();
  SET_VARIABLES.SET_CLKMIN = now.minute();
  SET_VARIABLES.SET_CLKSEC = now.second();
  SET_VARIABLES.SET_DAY =    now.day();
  SET_VARIABLES.SET_MONTH =  now.month();
  SET_VARIABLES.SET_YEAR =   now.year();
  SET_VARIABLES.SET_START_CLK_HRS = preferences.getInt("START_HRS", 0);
  SET_VARIABLES.SET_START_CLK_MIN = preferences.getInt("START_MIN", 0);
  //SET_VARIABLES.SET_START_CLK_SEC = preferences.getInt("START_SEC", 0);
  SET_VARIABLES.SET_PHOTO_CLK_HRS = preferences.getInt("PHOTO_HRS", 0); // Corrected key
  SET_VARIABLES.SET_PHOTO_CLK_MIN = preferences.getInt("PHOTO_MIN", 0); // Corrected key
  //SET_VARIABLES.SET_STOP_CLK_SEC = preferences.getInt("STOP_SEC", 0); // Corrected key

  //SET_VARIABLES.SET_DURATION = preferences.getInt("SET_DURATION", 0);
  


   
  preferences.end();

  Serial.println(SET_VARIABLES.SET_START_CLK_HRS);
  Serial.println(SET_VARIABLES.SET_START_CLK_MIN);
  Serial.println(SET_VARIABLES.SET_START_CLK_SEC);

LIVE_VARIABLES.SET_CLKHRS = SET_VARIABLES.SET_CLKHRS;
LIVE_VARIABLES.SET_CLKMIN = SET_VARIABLES.SET_CLKMIN;
LIVE_VARIABLES.SET_CLKSEC = SET_VARIABLES.SET_CLKSEC ;
LIVE_VARIABLES.SET_DAY = SET_VARIABLES.SET_DAY;
LIVE_VARIABLES.SET_MONTH = SET_VARIABLES.SET_MONTH;
LIVE_VARIABLES.SET_YEAR = SET_VARIABLES.SET_YEAR;
LIVE_VARIABLES.SET_START_CLK_HRS = SET_VARIABLES.SET_START_CLK_HRS ;
LIVE_VARIABLES.SET_START_CLK_MIN = SET_VARIABLES.SET_START_CLK_MIN;
//LIVE_VARIABLES.SET_START_CLK_SEC = SET_VARIABLES.SET_START_CLK_SEC;
LIVE_VARIABLES.SET_PHOTO_CLK_HRS = SET_VARIABLES.SET_STOP_CLK_HRS;
LIVE_VARIABLES.SET_PHOTO_CLK_MIN = SET_VARIABLES.SET_STOP_CLK_MIN;
//LIVE_VARIABLES.SET_STOP_CLK_SEC = SET_VARIABLES.SET_STOP_CLK_SEC;


}



void calculateDuration(int start_hrs, int start_min, int start_sec, int stop_hrs, int stop_min, int stop_sec) 
{
    // Calculate the duration in seconds
    int start_seconds = start_hrs * 3600 + start_min * 60 + start_sec;
    int stop_seconds = stop_hrs * 3600 + stop_min * 60 + stop_sec;
    int duration_seconds = stop_seconds - start_seconds;

    // Calculate hours, minutes, and seconds from the duration
    int duration_hrs = duration_seconds / 3600;
    int duration_min = (duration_seconds % 3600) / 60;
    int duration_sec = duration_seconds % 60;

    // Output the result
    
}

void Ent_key()
{
  if (Ent_Button.isPressed() && currentMode == PROGRAMMING_MODE) 
 {
    switch (setting) 
    {
        case CLK_HRS:
            // Handle programming for CLK_HRS
            setting = CLK_MIN;
            break;
        case CLK_MIN:
            // Handle programming for CLK_MIN
            setting = CLK_SEC;
            break;
        case CLK_SEC:
            // Handle programming for CLK_SEC
            setting = DAY;
            break;
        case DAY:
            // Handle programming for DAY
            setting = MONTH;
            break;
        case MONTH:
            // Handle programming for MONTH
            setting = YEAR;
            break;
        case YEAR:
            // Handle programming for YEAR
            setting = START_CLK_HRS;
            break;
        case START_CLK_HRS:
            // Handle programming for START_CLK_HRS
            setting = START_CLK_MIN;
            break;
        case START_CLK_MIN:
            // Handle programming for START_CLK_MIN
            setting = PHOTO_CLK_HRS;
            break;
        case PHOTO_CLK_HRS:
            // Handle programming for START_CLK_SEC
            setting = PHOTO_CLK_MIN;
            break;
        case PHOTO_CLK_MIN:
            // Handle programming for START_CLK_HRS
            setting = CLK_HRS;
            break;
        // case STOP_CLK_MIN:
        //     // Handle programming for START_CLK_MIN
        //     setting = STOP_CLK_SEC;
        //     break;
        // case STOP_CLK_SEC:
        //     // Handle programming for START_CLK_SEC
        //     setting = CLK_HRS;

            //break;
        }
 }
 
}



void inc_key()
{
  if (Inc_Button.isPressed() && currentMode == PROGRAMMING_MODE) 
 {
    int daysInMonth[] = {31, 28 + isLeapYear(SET_VARIABLES.SET_YEAR), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; 
     
    switch (setting)
     {
        case CLK_HRS:
            SET_VARIABLES.SET_CLKHRS = (SET_VARIABLES.SET_CLKHRS + 1) % 24 ;
            break;
        case CLK_MIN:
              SET_VARIABLES.SET_CLKMIN = (SET_VARIABLES.SET_CLKMIN + 1) % 60 ;
            break;
        case CLK_SEC:
             SET_VARIABLES.SET_CLKSEC = (SET_VARIABLES.SET_CLKSEC + 1) % 60 ;
            break;
        case DAY:
             
            if (SET_VARIABLES.SET_MONTH == 0 || SET_VARIABLES.SET_DAY == 0) 
            {
        // Handle the case where either the month or day is zero (unset)
        // You can choose to set both to non-zero default values, or handle it in a way that makes sense for your application.
        SET_VARIABLES.SET_MONTH = 1; // Set to January or another default month
        SET_VARIABLES.SET_DAY = 1;   // Set to the first day
    } else if (SET_VARIABLES.SET_DAY >= 1 && SET_VARIABLES.SET_DAY < daysInMonth[SET_VARIABLES.SET_MONTH - 1]) {
        // The day is within the valid range, perform the increment
        SET_VARIABLES.SET_DAY++;
    } else {
        // Handle the case when the day exceeds the maximum days for the month
        // You can choose to keep it at the last day of the month or reset it to the first day
        SET_VARIABLES.SET_DAY = 1; // Reset to the first day
    }
    break;
        case MONTH:
            if (SET_VARIABLES.SET_MONTH == 0) {
        // Handle the case where the month is zero (unset)
        // You can choose to set it to a default month, like January (1)
        SET_VARIABLES.SET_MONTH = 1;
    } else if (SET_VARIABLES.SET_MONTH >= 1 && SET_VARIABLES.SET_MONTH < 12) {
        // The month is within the valid range, perform the increment
        SET_VARIABLES.SET_MONTH++;
    } else {
        // Handle the case when the month exceeds the maximum month (12)
        // You can choose to keep it at December or reset it to January
        SET_VARIABLES.SET_MONTH = 1; // Reset to January
    }
            break;
        case YEAR:
             SET_VARIABLES.SET_YEAR++;
            break;
        case START_CLK_HRS:
             SET_VARIABLES.SET_START_CLK_HRS = (SET_VARIABLES.SET_START_CLK_HRS + 1) % 24 ;
            break;
        case START_CLK_MIN:
             SET_VARIABLES.SET_START_CLK_MIN = (SET_VARIABLES.SET_START_CLK_MIN + 1) % 60 ;
            break;
        case START_CLK_SEC:
             SET_VARIABLES.SET_START_CLK_SEC = (SET_VARIABLES.SET_START_CLK_SEC + 1) % 60 ;
            break;
        case STOP_CLK_HRS:
             SET_VARIABLES.SET_STOP_CLK_HRS = (SET_VARIABLES.SET_STOP_CLK_HRS + 1) % 24 ;
            break;
        case STOP_CLK_MIN:
             SET_VARIABLES.SET_STOP_CLK_MIN = (SET_VARIABLES.SET_STOP_CLK_MIN + 1) % 60 ;
            break;
        case STOP_CLK_SEC:
             SET_VARIABLES.SET_STOP_CLK_SEC = (SET_VARIABLES.SET_STOP_CLK_SEC + 1) % 60 ;
            break;
        case PHOTO_CLK_HRS:
             SET_VARIABLES.SET_PHOTO_CLK_HRS = (SET_VARIABLES.SET_PHOTO_CLK_HRS + 1) % 24 ;
            break;
        case PHOTO_CLK_MIN:
             SET_VARIABLES.SET_PHOTO_CLK_MIN = (SET_VARIABLES.SET_PHOTO_CLK_MIN + 1) % 60 ;
            break;
        
        //   case DURATION:
        //        SET_VARIABLES.SET_DURATION = (SET_VARIABLES.SET_DURATION + 1) % 60; 
        //     break;   
    }
}



}



void dec_key()
{
    if (Dec_Button.isPressed() && currentMode == PROGRAMMING_MODE) 
  {
    int daysInMonth[] = {31, 28 + isLeapYear(SET_VARIABLES.SET_YEAR), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    switch (setting) 
    {
        case CLK_HRS:
            SET_VARIABLES.SET_CLKHRS = (SET_VARIABLES.SET_CLKHRS - 1 + 24) % 24;
            break;
        case CLK_MIN:
            SET_VARIABLES.SET_CLKMIN = (SET_VARIABLES.SET_CLKMIN - 1 + 60) % 60;
            break;
        case CLK_SEC:
            SET_VARIABLES.SET_CLKSEC = (SET_VARIABLES.SET_CLKSEC - 1 + 60) % 60;
            break;
        case DAY:
           // SET_VARIABLES.SET_DAY = (SET_VARIABLES.SET_DAY - 1 + daysInMonth[SET_VARIABLES.SET_MONTH - 1]) % daysInMonth[SET_VARIABLES.SET_MONTH - 1] + 1;
            if (SET_VARIABLES.SET_DAY > 1) 
            {
    // The day is greater than 1, decrement it
                    SET_VARIABLES.SET_DAY--;
            } 
       else 
          {
    // The day is 1, set it to the last day of the previous month
            if (SET_VARIABLES.SET_MONTH > 1)
          {
              SET_VARIABLES.SET_MONTH--; // Decrement the month
              SET_VARIABLES.SET_DAY = daysInMonth[SET_VARIABLES.SET_MONTH - 1]; // Set to the last day of the previous month
         } 
       else 
          {
        // The month is already January, set the day to 1 (first day of January)
              SET_VARIABLES.SET_DAY = 1;
            }
        }

            break;
        case MONTH:
            SET_VARIABLES.SET_MONTH = (SET_VARIABLES.SET_MONTH - 1 + 12) % 12 + 1;
            break;
        case YEAR:
            SET_VARIABLES.SET_YEAR--;
            break;
        case START_CLK_HRS:
            SET_VARIABLES.SET_START_CLK_HRS = (SET_VARIABLES.SET_START_CLK_HRS - 1 + 24) % 24;
            break;
        case START_CLK_MIN:
            SET_VARIABLES.SET_START_CLK_MIN = (SET_VARIABLES.SET_START_CLK_MIN - 1 + 60) % 60;
            break;
        case START_CLK_SEC:
            SET_VARIABLES.SET_START_CLK_SEC = (SET_VARIABLES.SET_START_CLK_SEC - 1 + 60) % 60;
            break;
        case STOP_CLK_HRS:
            SET_VARIABLES.SET_STOP_CLK_HRS = (SET_VARIABLES.SET_STOP_CLK_HRS - 1 + 24) % 24; 
            break;
        case STOP_CLK_MIN:
            SET_VARIABLES.SET_STOP_CLK_MIN = (SET_VARIABLES.SET_STOP_CLK_MIN - 1 + 60) % 60;
            break;
        case STOP_CLK_SEC:
            SET_VARIABLES.SET_STOP_CLK_SEC = (SET_VARIABLES.SET_STOP_CLK_SEC - 1 + 60) % 60;
            break;  
        case PHOTO_CLK_HRS:
             SET_VARIABLES.SET_PHOTO_CLK_HRS = (SET_VARIABLES.SET_STOP_CLK_HRS - 1 + 24) % 24;;
            break;
        case PHOTO_CLK_MIN:
             SET_VARIABLES.SET_PHOTO_CLK_MIN = (SET_VARIABLES.SET_STOP_CLK_MIN - 1 + 60) % 60;
            break;
      
        // case DURATION:
        //     SET_VARIABLES.SET_DURATION = (SET_VARIABLES.SET_DURATION - 1 + 60) % 60;
        //     break;
    }
}




}