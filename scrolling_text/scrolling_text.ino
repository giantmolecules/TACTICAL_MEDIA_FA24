//----------------------------------------------------------------//
//
// SAIC Tactical Media Fall 2024
// Brett Ian Balogh
// https://github.com/giantmolecules/TACTICAL_MEDIA_FA24
//
// scrolling_text.ino
//
// A scrolling text display ala Jenny Holzer
//
//----------------------------------------------------------------//

// Include libraries for TFT
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "ScrollingText.h"

// Set the display dimensions
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 135
#define TEXT_SIZE 8

// Create a TFT object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Create a ScrollingText object
ScrollingText scrollText(tft, SCREEN_WIDTH, SCREEN_HEIGHT,TEXT_SIZE, "Abuse Of Power Comes As No Surprise");



//----{SETUP}-----------------------------------------------------//

void setup() {

  // Start Serial COM for debugging
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
  tft.setTextWrap(0);

  // default text size
  tft.setTextSize(TEXT_SIZE);

  // set text foreground and background colors
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  Serial.println(F("TFT Initialized"));

}

//----{LOOP}------------------------------------------------------//

void loop() {
   // Update the scrolling text
  scrollText.update();
}

//----{END}------------------------------------------------------//
