//----------------------------------------------------------------//
//
// SAIC Tactical Media Fall 2024
// Brett Ian Balogh
// https://github.com/giantmolecules/TACTICAL_MEDIA_FA24
//
// tft_template.ino
//
// This is a template for our code that readies the TFT for use.
//
//----------------------------------------------------------------//

// Include libraries for TFT
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Create a TFT object
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

int colors[8] = {0xe81f, 0x681f, 0xf814, 0xfed0, 0xfd34, 0xff8d, 0x6d5f, 0x6e3f};
int mono[2] = {ST77XX_WHITE, ST77XX_BLACK};
int height = 135;
int width = 240;
int maxRadius = 30;
int minRadius = 5;

//----{SETUP}-----------------------------------------------------//

void setup() {

  // Start Serial COM
  Serial.begin(115200);
  //while(!Serial);
  
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
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  Serial.println(F("TFT Initialized"));

}

//----{LOOP}------------------------------------------------------//

void loop() {

  int choice = random(2);
  Serial.print(choice);
  int numLines = random(100);
  int x = random(width);
  int y = random(height);
  int length = random(1, width);
  int spacing = random(50);
  int c = random(8);
  int t = random(10, 100);
  int bw = random(4);

  if (choice >= 1) {
    for(int i = 0; i <= numLines; i++){
      tft.drawFastHLine(x, y+(spacing*i), length, mono[bw]); 
    }
  }

  if (choice == 0) {
    for(int i = 0; i <= numLines; i++){
      tft.drawFastVLine(x+(spacing*i), y, length, mono[bw]); 
    }
  }
  
  delay(t);
}

//----{END}------------------------------------------------------//
