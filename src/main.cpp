// Adafruit IO Publish Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/
#include <TimeLib.h>
#include <ArduinoOTA.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TM1637Display.h>
 
TM1637Display display(SEG_7_CLK, SEG_7_DIO); //set up the 4-Digit Display.

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(TEMP_SENSE_IO);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress; 
float tempC = 0.0;
const uint8_t SEG_DEGREE_SIGN[] = {
  SEG_A | SEG_F | SEG_B | SEG_G     // o
  };

uint8_t segto=0xff;


unsigned int lastMonitorTime = 0;
unsigned int lastReportTime = 0;
unsigned int firstReportTime = 0;

// set up the 'time/seconds' topic
AdafruitIO_Time *seconds = io.time(AIO_TIME_SECONDS);
time_t secTime = 0;

// set up the 'time/milliseconds' topic
//AdafruitIO_Time *msecs = io.time(AIO_TIME_MILLIS);

// set up the 'time/ISO-8601' topic
// AdafruitIO_Time *iso = io.time(AIO_TIME_ISO);
// char *isoTime;

// this int will hold the current count for our sketch
// int count = 0;

// set up the 'counter' feed
// AdafruitIO_Feed *counter = io.feed("counter");
AdafruitIO_Feed *rssi = io.feed("rssi_temp");
AdafruitIO_Feed *temperature = io.feed("tempC");


// message handler for the seconds feed
void handleSecs(char *data, uint16_t len) {
  // Serial.print("Seconds Feed: ");
  // Serial.println(data);
  secTime = atoi (data);
}

// message handler for the milliseconds feed
// void handleMillis(char *data, uint16_t len) {
//   Serial.print("Millis Feed: ");
//   Serial.println(data);
// }

// message handler for the ISO-8601 feed
// void handleISO(char *data, uint16_t len) {
//   Serial.print("ISO Feed: ");
//   Serial.println(data);
//   isoTime = data;
// }

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void displayWithFade(float tempRead){
  int k;
  for(k = 7; k > 0; k--) {
    display.setBrightness(k);
    display.setSegments(SEG_DEGREE_SIGN,1,3);
    display.showNumberDecEx(tempRead*10,segto,false,3,0);
    delay(BRIGHTNESS_DELAY);
  }
  display.setBrightness(0,false);
  display.setSegments(SEG_DEGREE_SIGN,1,3);
  display.showNumberDecEx(tempRead*10,segto,false,3,0);
  delay(2*BRIGHTNESS_DELAY);
  for(k = 0; k < 7; k++) {
    display.setBrightness(k);
    display.setSegments(SEG_DEGREE_SIGN,1,3);
    display.showNumberDecEx(tempRead*10,segto,false,3,0);
    delay(BRIGHTNESS_DELAY);
  }
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

// time sync function
time_t timeSync()
{
  if (secTime == 0) {
    return 0;
  }
  return (secTime + TZ_HOUR_SHIFT * 3600);
}

void setup() {

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  // Start up the temperature sensor library
  sensors.begin();

  display.setBrightness(7); //set the diplay to maximum brightness
  display.showNumberDecEx(0,segto,true); // init display with all 0

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // attach message handler for the seconds feed
  seconds->onMessage(handleSecs);

  // attach a message handler for the msecs feed
  //msecs->onMessage(handleMillis);

  // attach a message handler for the ISO feed
  // iso->onMessage(handleISO);

  // attach message handler for the button feed
  // reportButton->onMessage(setButton);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  // Because Adafruit IO doesn't support the MQTT retain flag, we can use the
  // get() function to ask IO to resend the last value for this feed to just
  // this MQTT client after the io client is connected.
  // reportButton->get();

  setSyncProvider(timeSync);
  setSyncInterval(60); // sync interval in seconds, consider increasing
  
  //// Over The Air section ////
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(THING_NAME);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

}

void loop() {
  unsigned int currTime = millis();

  ArduinoOTA.handle();

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // temperature sensor
  sensors.requestTemperatures(); // Send the command to get temperatures
  tempC = sensors.getTempCByIndex(0);
  Serial.print("Temperature in C is: ");
  Serial.println(tempC);
  
  // set the 7 segment display to current temp
  displayWithFade(tempC); // Display the temperature with a fade effect;

  // don't do anything before we get a time read and set the clock
  if (timeStatus() == timeNotSet) {
    if (secTime > 0) {
      setTime(timeSync());
      Serial.print("Time set, time is now <- ");
      digitalClockDisplay();
    }
    else {
      return;
    }
  }
  
  if (currTime - lastMonitorTime >= (MONITOR_SECS * 1000)) {
    temperature->save(tempC);

    // save the wifi signal strength (RSSI) to the 'rssi' feed on Adafruit IO
    Serial.print("sending rssi value -> ");
    rssi->save(WiFi.RSSI());
    
    Serial.print("Time is: ");
    digitalClockDisplay();

    lastMonitorTime = currTime;
  }
}
