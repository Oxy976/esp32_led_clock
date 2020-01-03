#include "settings.h"

//#include <Wire.h>
#include "esp_system.h"
#include "FastLED.h"
#include "time.h"
#include <WiFi.h>


CRGB leds[NUM_LEDS]; //Задаём значение светодиодов
byte digits[12][7] = {
  {0, 1, 1, 1, 1, 1, 1}, // Digit 0
  {0, 1, 0, 0, 0, 0, 1}, // Digit 1
  {1, 1, 1, 0, 1, 1, 0 }, // Digit 2
  {1, 1, 1, 0, 0, 1, 1 }, // Digit 3
  {1, 1, 0, 1, 0, 0, 1 }, // Digit 4
  {1, 0, 1, 1, 0, 1, 1 }, // Digit 5
  {1, 0, 1, 1, 1, 1, 1 }, // Digit 6
  {0, 1, 1, 0, 0, 0, 1 }, // Digit 7
  {1, 1, 1, 1, 1, 1, 1 }, // Digit 8
  {1, 1, 1, 1, 0, 1, 1 }, // Digit 9 | 2D Array for numbers on 7 segment
  {1, 1, 1, 1, 0, 0, 0}, // Digit *0
  {0, 0, 1, 1, 1, 1, 0}
};  // Digit C


bool Dot = true;  //Dot state
bool DST = false; //DST state
bool TempShow = false;
int last_digit = 0;

// int ledColor = 0x0000FF; // Color used (in hex)
long ledColor = CRGB::DarkOrchid; // Color used (in hex)
//long ledColor = CRGB::MediumVioletRed;
//Random colors i picked up
long ColorTable[16] = {
  CRGB::Amethyst,
  CRGB::Aqua,
  CRGB::Blue,
  CRGB::Chartreuse,
  CRGB::DarkGreen,
  CRGB::DarkMagenta,
  CRGB::DarkOrange,
  CRGB::DeepPink,
  CRGB::Fuchsia,
  CRGB::Gold,
  CRGB::GreenYellow,
  CRGB::LightCoral,
  CRGB::Tomato,
  CRGB::Salmon,
  CRGB::Red,
  CRGB::Orchid
};


struct tm timeinfo;

// connect to NTP server and write date-time to esp32
void GetNTPTime() {
  //=====
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //init and get the time
  long gmtOffset_sec = 0;
  gmtOffset_sec = TIMEZONE * 3600;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServerName);
  //   printLocalTime();

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  //=====
}


void setup() {
  Serial.begin(115200);
  //  Wire.begin();
  Serial.println("init board");
  //  LEDS.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS); // Set LED strip type
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  LEDS.setBrightness(10); // Set initial brightness
  //  pinMode(DST_PIN, INPUT_PULLUP); // Define DST adjust button pin
  //  pinMode(MIN_PIN, INPUT_PULLUP); // Define Minutes adjust button pin
  //  pinMode(HUR_PIN, INPUT_PULLUP); // Define Hours adjust button pin
  TempShow = false;

  GetNTPTime();
}

// get time from esp32
// if hours will be a single digit then time will be displayed 155 instead of 0155
int GetTime() {
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (8);
  }
  //  Serial.println(&timeinfo, "%m %d %Y / %H:%M:%S");
  int hour = timeinfo.tm_hour;
  int min = timeinfo.tm_min;
  int sec = timeinfo.tm_sec;
  //  Serial.println("Time - " + String(hour) + ":" + String(min) + ":" + String(sec));

  if (sec % 2 == 0) {
    Dot = false;
  }
  else {
    Dot = true;
  };
  return (hour * 100 + min);
};



// Check Light sensor and set brightness accordingly
/*
  void BrightnessCheck(){
  const byte sensorPin = BRI_PIN; // light sensor pin
  const byte brightnessLow = 20; // Low brightness value
  const byte brightnessHigh = 70; // High brightness value
  int sensorValue = analogRead(sensorPin); // Read sensor
  Serial.print("Sensor is: ");Serial.println(sensorValue);
  sensorValue = map(sensorValue, 0, 255, 10, 100);
  LEDS.setBrightness(sensorValue);
  // if (sensorValue <= 10) {LEDS.setBrightness(brightnessHigh);}
  // else {LEDS.setBrightness(brightnessLow);}
  };
*/

// Convert time to array needet for display
void TimeToArray() {
  int Now = GetTime();  // Get time

  int cursor = 30; // last led number

  // Serial.print("Time is: ");Serial.println(Now);
  if (DST) {  // if DST is true then add one hour
    Now += 100;
    // Serial.print("DST is ON, time set to : ");Serial.println(Now);
  };
  if (Dot) {
    leds[14] = ledColor;
    leds[15] = ledColor;
  }
  else  {
    leds[14] = 0x000000;
    leds[15] = 0x000000;
  };

  for (int i = 1; i <= 4; i++) {
    int digit = Now % 10; // get last digit in time
    if (i == 1) {
      //  Serial.print("Digit 4 is : ");Serial.print(digit);Serial.print(" ");

      cursor = 23;

      for (int k = 0; k <= 6; k++) {
        // Serial.print(digits[digit][k]);
        if (digits[digit][k] == 1) {
          leds[cursor] = ledColor;
        }
        else if (digits[digit][k] == 0) {
          leds[cursor] = 0x000000;
        };
        cursor ++;
      };
      // Serial.println();

      if (digit != last_digit)
      {
        cylon();
        ledColor =  ColorTable[random(16)];
      }
      last_digit = digit;

    }
    else if (i == 2) {
      // Serial.print("Digit 3 is : ");Serial.print(digit);Serial.print(" ");

      cursor = 16;
      for (int k = 0; k <= 6; k++) {
        // Serial.print(digits[digit][k]);
        if (digits[digit][k] == 1) {
          leds[cursor] = ledColor;
        }
        else if (digits[digit][k] == 0) {
          leds[cursor] = 0x000000;
        };
        cursor ++;
      };
      // Serial.println();
    }
    else if (i == 3) {
      // Serial.print("Digit 2 is : ");Serial.print(digit);Serial.print(" ");
      cursor = 7;
      for (int k = 0; k <= 6; k++) {
        // Serial.print(digits[digit][k]);
        if (digits[digit][k] == 1) {
          leds[cursor] = ledColor;
        }
        else if (digits[digit][k] == 0) {
          leds[cursor] = 0x000000;
        };
        cursor ++;
      };
      // Serial.println();
    }
    else if (i == 4) {
      // Serial.print("Digit 1 is : ");Serial.print(digit);Serial.print(" ");
      cursor = 0;
      for (int k = 0; k <= 6; k++) {
        // Serial.print(digits[digit][k]);
        if (digits[digit][k] == 1) {
          leds[cursor] = ledColor;
        }
        else if (digits[digit][k] == 0) {
          leds[cursor] = 0x000000;
        };
        cursor ++;
      };
      // Serial.println();
    }
    Now /= 10;
  };
};
/*
  // Convert temp to array needet for display
  void TempToArray(){
  // tmElements_t tm;
  //  RTC.(tm);

  if (tm.Second != 27) {
    TempShow = false;
    return;
  }
  TempShow = true;
  // int t = RTC.temperature();
  int celsius = (t / 4.0) * 100;

  Serial.print("Temp is: ");Serial.println(celsius);

  int cursor = 30; // last led number

    leds[57]=0x000000;		// #?
    leds[56]=0x000000;		// #?

  for(int i=1;i<=4;i++){
    int digit = celsius % 10; // get last digit in time
    if (i==1){
      Serial.print("Digit 4 is : ");Serial.print(digit);Serial.print(" ");

      cursor = 86;

      for(int k=0; k<=27;k++){
        Serial.print(digits[11][k]);
        if (digits[11][k]== 1){leds[cursor]=ledColor;}
         else if (digits[11][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      Serial.println();
    }
    else if (i==2){
      Serial.print("Digit 3 is : ");Serial.print(digit);Serial.print(" ");

      cursor =58;

      for(int k=0; k<=27;k++){
        Serial.print(digits[10][k]);
        if (digits[10][k]== 1){leds[cursor]=ledColor;}
         else if (digits[10][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      Serial.println();
      }
    else if (i==3){
      Serial.print("Digit 2 is : ");Serial.print(digit);Serial.print(" ");
      cursor =28;
      for(int k=0; k<=27;k++){
        Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      Serial.println();
      }
    else if (i==4){
      Serial.print("Digit 1 is : ");Serial.print(digit);Serial.print(" ");
      cursor =0;
      for(int k=0; k<=27;k++){
        Serial.print(digits[digit][k]);
        if (digits[digit][k]== 1){leds[cursor]=ledColor;}
         else if (digits[digit][k]==0){leds[cursor]=0x000000;};
         cursor ++;
        };
      Serial.println();
      }
    celsius /= 10;
  };
  };
*/

/*
  void DSTcheck() {
  int buttonDST = digitalRead(2);
  // Serial.print("DST is: ");Serial.println(DST);
  if (buttonDST == LOW) {
    if (DST) {
      DST = false;
      // Serial.print("Switching DST to: ");Serial.println(DST);
    }
    else if (!DST) {
      DST = true;
      // Serial.print("Switching DST to: ");Serial.println(DST);
    };
    delay(500);
  };
  }
*/

/*
  void TimeAdjust(){
  int buttonH = digitalRead(HUR_PIN);
  int buttonM = digitalRead(MIN_PIN);
  if (buttonH == LOW || buttonM == LOW){
    delay(500);
    tmElements_t Now;
    RTC.(Now);
    int hour=Now.Hour;
    int minutes=Now.Minute;
    int second =Now.Second;
      if (buttonH == LOW){
        if (Now.Hour== 23){Now.Hour=0;}
          else {Now.Hour += 1;};
        }else {
          if (Now.Minute== 59){Now.Minute=0;}
          else {Now.Minute += 1;};
          };
    RTC.write(Now);
    }
  }
*/

/* coool effect function*/
void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);
  }
}


void cylon () {
  static uint8_t hue = 0;
  Serial.print("x");
  // First slide the led in one direction
  for (int i = 0; i < NUM_LEDS; i++) {
    // Set the i'th led to red
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
  Serial.print("x");

  // Now go in the other direction.
  for (int i = (NUM_LEDS) - 1; i >= 0; i--) {
    // Set the i'th led to red
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
}

void loop()  // Main loop
{
  // BrightnessCheck(); // Check brightness
  //  DSTcheck(); // Check DST
  //  TimeAdjust(); // Check to se if time is geting modified
  TimeToArray(); // Get leds array with required configuration
  //  TempToArray();
  FastLED.show(); // Display leds array
  //  if (TempShow == true) delay (8000);

}
