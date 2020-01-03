// Network settings
// --
const char* hostname = "esp-clock";
const char* ssid = "ssid";
const char* password = "pass";
// --

// Time settings
const char* ntpServerName = "pool.ntp.org";
const int   TIMEZONE = 3;
const int   daylightOffset_sec = 0;



const int NUM_LEDS = 30; // 4*7*4 +2  Number of LED controlled
const  char* COLOR_ORDER = "BRG";  // Define color order for your strip
const int DATA_PIN = 18;  // Data pin for led comunication
const int DST_PIN = 33;  // Define DST adjust button pin
const int MIN_PIN = 32;  // Define Minutes adjust button pin
const int HUR_PIN = 31;  // Define Hours adjust button pin
const int BRI_PIN = 30;  // Define Light sensor pin
