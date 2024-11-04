//----------------------------------------------------------------//
//
// SAIC Tactical Media Fall 2024
// Brett Ian Balogh
// https://github.com/giantmolecules/TACTICAL_MEDIA_FA24
//
// ESP32_CaptivePortal.ino
//
// This is a modified CaptivePortal example from Arduino
//
//----------------------------------------------------------------//

#include <WiFi.h>
#include <DNSServer.h>

// Include libraries for TFT
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Create a TFT object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

const byte DNS_PORT = 53;
IPAddress apIP(8,8,4,4); // The default android DNS
DNSServer dnsServer;
WiFiServer server(80);

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
  "<h1>Hello World!</h1><p>I am Brett and this is a word. </p></body></html>";

void setup() { 
    // Start Serial COM
  Serial.begin(115200);
  
  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  // Turn off word wrapping
  tft.setTextWrap(false);

  // default text size
  tft.setTextSize(4);
  
  // set text foreground and background colors
  tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Brett's AP");
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  server.begin();

  tft.setCursor(0,50);
  tft.print("ESP32-DNSServer");
}

void loop() {
  dnsServer.processNextRequest();
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(responseHTML);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}
