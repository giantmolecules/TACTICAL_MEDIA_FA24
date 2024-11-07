/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

AsyncWebServer server(80);

// Create a TFT object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -21600;
const int daylightOffset_sec = 3600;

String weekDays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

int alarmDay = 4;
int alarmHour = 13;
int alarmMinute = 31;

int currentDay;
int currentHour;
int currentMinute;

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "SAIC-Guest";
const char* password = "wifi@saic";

const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";
const char* PARAM_INPUT_3 = "input3";

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Set Alarm</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    Day of the Week (0-6) <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Hour: <input type="text" name="input2">
    <input type="submit" value="Submit">
  </form><br>
  <form action="/get">
    Minute: <input type="text" name="input3">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {

  // Start Serial COM
  Serial.begin(115200);

  // Setup pins
  pinMode(13, OUTPUT);

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

  // Turn off word wrapping
  //tft.setTextWrap(false);

  // default text size
  tft.setTextSize(2);

  // set text foreground and background colors
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  Serial.println(F("TFT Initialized"));

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest* request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      alarmDay = inputMessage.toInt();
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2)) {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
      alarmHour = inputMessage.toInt();
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3)) {
      inputMessage = request->getParam(PARAM_INPUT_3)->value();
      inputParam = PARAM_INPUT_3;
      alarmMinute = inputMessage.toInt();
    } else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" + inputParam + ") with value: " + inputMessage + "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  delay(1000);
  printLocalTime();

  Serial.print("Day: ");
  Serial.println(currentDay);

  Serial.print("Hour: ");
  Serial.println(currentHour);

  Serial.print("Minute: ");
  Serial.println(currentMinute);

  if (currentHour >= alarmHour && currentMinute >= alarmMinute) {
    Serial.println("ALARM!");
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  //currentDay = (&timeinfo, "%A");
  //currentMinute = int((&timeinfo, "%M"));
  //currentHour = int((&timeinfo, "%I"));

  currentDay = timeinfo.tm_wday;
  currentMinute = int(timeinfo.tm_min);
  currentHour = int(timeinfo.tm_hour);

  Serial.println(&timeinfo, "%A, %B %d %Y %I:%M:%S");
  //tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.println(&timeinfo, "%A");
  tft.setCursor(0, 25);
  tft.println(&timeinfo, "%B %d");
  tft.setCursor(0, 50);
  tft.println(&timeinfo, "%Y");
  tft.setCursor(0, 75);
  tft.println(&timeinfo, "%I:%M:%S");
  tft.setCursor(0, 100);
  tft.print("Alarm: ");
  tft.print(weekDays[alarmDay]);
  tft.print(" ");
  tft.print(alarmHour);
  tft.print(" ");
  tft.print(alarmMinute);
}
