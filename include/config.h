
#include "secrets.h"

// uncomment the following line if you are using airlift
// #define USE_AIRLIFT

// uncomment the following line if you are using winc1500
// #define USE_WINC1500

// comment out the following lines if you are using fona or ethernet
#include "AdafruitIO_WiFi.h"

#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE) ||         \
    defined(ADAFRUIT_PYPORTAL)
// Configure the pins used for the ESP32 connection
#if !defined(SPIWIFI_SS) // if the wifi definition isnt in the board variant
// Don't change the names of these #define's! they match the variant ones
#define SPIWIFI SPI
#define SPIWIFI_SS 10 // Chip select pin
#define NINA_ACK 9    // a.k.a BUSY or READY pin
#define NINA_RESETN 6 // Reset pin
#define NINA_GPIO0 -1 // Not connected
#endif
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS, SPIWIFI_SS,
                   NINA_ACK, NINA_RESETN, NINA_GPIO0, &SPIWIFI);
#else
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
#endif
/******************************* FONA **************************************/

// the AdafruitIO_FONA client will work with the following boards:
//   - Feather 32u4 FONA -> https://www.adafruit.com/product/3027

// uncomment the following two lines for 32u4 FONA,
// and comment out the AdafruitIO_WiFi client in the WIFI section
// #include "AdafruitIO_FONA.h"
// AdafruitIO_FONA io(IO_USERNAME, IO_KEY);

/**************************** ETHERNET ************************************/

// the AdafruitIO_Ethernet client will work with the following boards:
//   - Ethernet FeatherWing -> https://www.adafruit.com/products/3201

// uncomment the following two lines for ethernet,
// and comment out the AdafruitIO_WiFi client in the WIFI section
// #include "AdafruitIO_Ethernet.h"
// AdafruitIO_Ethernet io(IO_USERNAME, IO_KEY);

// device related configurations
#define THING_NAME      "ada_temp"
#define TEMP_SENSE_IO   4  // Temperature sensor data IO
#define SEG_7_CLK       26 // Set the CLK pin IO to the 7 segment display
#define SEG_7_DIO       27 // Set the DIO pin IO to the 7 segment display

// Time related configurations
#define BRIGHTNESS_DELAY   100
#define TZ_HOUR_SHIFT   3  // number of hours to shift due to Time Zone
#define RESET_HOUR      16 // hour of day (0-23) to reset the button presses count
#define OFF_HOUR        23 // hour of day (0-23) to reset the button presses count
#define DEBOUNCE_SECS   0.5 // minimum number of seconds between presses
#define MONITOR_SECS    30 // interval between monitoring messages, watch out for Adafruit IO rate limit 30 topics per minute
