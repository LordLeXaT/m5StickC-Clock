#include <M5StickC.h>
#include <WiFi.h>
#include "time.h"

//WiFi credentials
char* ssid       = "xxxxxx";
char* password   = "xxxxxx"; 

// 
char* ntpServer =  "de.pool.ntp.org";

// define what timezone you are in
int timeZone = 7200;

// delay workarround
int tcount = 0;

// LCD Status
bool LCD = true;

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;


void buttons_code() {
  // Button A control the LCD (ON/OFF)
  if (M5.BtnA.wasPressed()) {
    if (LCD) {
      M5.Lcd.writecommand(ST7735_DISPOFF);
      M5.Axp.ScreenBreath(0);
      LCD = !LCD;
    } else {
      M5.Lcd.writecommand(ST7735_DISPON);
      M5.Axp.ScreenBreath(255);
      LCD = !LCD;
    }
  }
  // Button B doing a time resync if pressed for 2 sec
  if (M5.BtnB.pressedFor(2000)) {
    timeSync();
  }
}

// Printing time to LCD
void doTime() {
  if (timeToDo(1000)) {
    M5.Lcd.setTextSize(2);
    M5.Rtc.GetTime(&RTC_TimeStruct);
    M5.Rtc.GetData(&RTC_DateStruct);
    M5.Lcd.setCursor(27, 15);
    M5.Lcd.printf("%02d:%02d:%02d\n", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
    M5.Lcd.setCursor(27, 50);
    M5.Lcd.setTextSize(1);
    M5.Lcd.printf("Date: %04d-%02d-%02d\n", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
  }
}

//delays stopping usualy everything using this workarround
bool timeToDo(int tbase) {
  tcount++;
  if (tcount == tbase) {
    tcount = 0;
    return true;    
  } else {
    return false;
  }  
}

// Syncing time from NTP Server
void timeSync() {
    M5.Lcd.setTextSize(1);
    Serial.println("Syncing Time");
    Serial.printf("Connecting to %s ", ssid);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 15);
    M5.Lcd.println("connecting WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" CONNECTED");
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 15);
    M5.Lcd.println("Connected");
    // Set ntp time to local
    configTime(timeZone, 0, ntpServer);

    // Get local time
    struct tm timeInfo;
    if (getLocalTime(&timeInfo)) {
      // Set RTC time
      RTC_TimeTypeDef TimeStruct;
      TimeStruct.Hours   = timeInfo.tm_hour;
      TimeStruct.Minutes = timeInfo.tm_min;
      TimeStruct.Seconds = timeInfo.tm_sec;
      M5.Rtc.SetTime(&TimeStruct);

      RTC_DateTypeDef DateStruct;
      DateStruct.WeekDay = timeInfo.tm_wday;
      DateStruct.Month = timeInfo.tm_mon + 1;
      DateStruct.Date = timeInfo.tm_mday;
      DateStruct.Year = timeInfo.tm_year + 1900;
      M5.Rtc.SetData(&DateStruct);
      Serial.println("Time now matching NTP");
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(20, 15);
      M5.Lcd.println("S Y N C");
      delay(500);
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(40, 0, 2);
      M5.Lcd.println("Simple Clock");
    }
}

void setup() {
  M5.begin();

  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);

  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE,BLACK);
  M5.Lcd.setCursor(40, 0, 2);
  M5.Lcd.println("Simple Clock");
  //timeSync(); //uncomment if you want to have a timesync everytime you turn device on (if no WIFI is avail mostly bad)
}

void loop() {
  M5.update();
  buttons_code();
  doTime();
}
