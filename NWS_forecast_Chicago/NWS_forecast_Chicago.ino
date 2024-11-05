//----------------------------------------------------------------//
//
// SAIC Biological Communications Spring 2024
// Brett Ian Balogh
//
// NWS_forecast_pittsburgh_v1.ino
//
// This sketch gets the current chance of precipitation in Pittsburgh
//
// How it works:
//
// Once an hour, check the NWS site for the current precipitation probability.
// if the chance exceeds precipThreshold, state variable raining is true. If the
// probability goes below precipThreshold, raining is false. If raining is true,
// enter a state controlled by rain that rains for a random interval between rainDurationMin
// rainDurationMax. When rain stops, wait for a time controlled by rainPause that is
// between rainPauseMin and rainPauseMax. Continue raining and pausing until
// probability is below precipThreshold.
//
//----------------------------------------------------------------//

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Arduino_JSON.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Create a TFT object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Not sure if WiFiClientSecure checks the validity date of the certificate.
// Setting clock just to be sure...
String setClock() {
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
  return asctime(&timeinfo);
}

// Variables

WiFiMulti WiFiMulti;
JSONVar myObject;
String forecast = "";
int timeNow = 0;
int timeThen = 0;
int errorCounter = 0;
boolean firstTime = 1;
boolean raining = 0;
boolean rain = 0;
int pastRainTime = 0;
int pastPrintTime = 0;
int printInterval = 5000;

/************************************************/

// Interval to check forecast in milliseconds. 1s = 1000ms

int interval = 3600000;  //1 hour

/************************************************/


void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);

  /************************************************/

  // Change for your wifi credentials

  WiFiMulti.addAP("SAIC-Guest", "wifi@saic");
  

  /************************************************/


  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  setClock();

  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240);  // Init ST7789 240x135
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  // default text size
  tft.setTextSize(12);

  // set text foreground and background colors
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  Serial.println(F("TFT Initialized"));
}

void loop() {
  // Print to TFT
  if (millis() - pastPrintTime > printInterval) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.println(setClock());
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
    tft.print(forecast);
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    pastPrintTime = millis();
  }

  timeNow = millis();
  if (timeNow - timeThen > interval || firstTime) {
    WiFiClientSecure *client = new WiFiClientSecure;

    if (client) {
      //client -> setCACert(rootCACertificate);
      client->setInsecure();
      {
        // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
        HTTPClient https;

        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, "https://api.weather.gov/gridpoints/LOT/76,73/forecast")) {  // Chicago
          Serial.print("[HTTPS] GET...\n");
          // start connection and send HTTP header
          int httpCode = https.GET();

          // httpCode will be negative on error
          if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
              String jsonString = https.getString();
              myObject = JSON.parse(jsonString);
                Serial.print("Output: ");
                Serial.println(myObject["properties"]["periods"][0]["shortForecast"]);
                forecast = JSON.stringify(myObject["properties"]["periods"][0]["shortForecast"]);
              }
            } else {
              Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
              errorCounter++;

              // if we get too many connection errors, restart the ESP

              if (errorCounter >= 3) {
                ESP.restart();
              }
            }

            https.end();
          } else {
            Serial.printf("[HTTPS] Unable to connect\n");
          }

          // End extra scoping block
        }

        delete client;
      }
      else {
        Serial.println("Unable to create client");
      }

      Serial.println();
      Serial.println("Waiting 1h before the next round...");
      //delay(90000);
      if (firstTime) {
        firstTime = 0;
      }
      timeThen = millis();
    }
  }
