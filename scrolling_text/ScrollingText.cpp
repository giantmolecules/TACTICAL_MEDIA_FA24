#include "Adafruit_ST77xx.h"
#include "ScrollingText.h"

ScrollingText::ScrollingText(Adafruit_ST7789 &display, int screenWidth, int screenHeight, int fontSize, String text)
  : tft(display), screenWidth(screenWidth), screenHeight(screenHeight), fontSize(fontSize), text(text) {
  textX = screenWidth;
  calculateTextBounds();
}

void ScrollingText::update() {
  tft.setTextColor(ST77XX_BLACK, ST77XX_BLACK);
  //
  tft.print(text);
  // Draw the text at the current position
  tft.setCursor(textX, (screenHeight / 2) - (fontSize * 8 / 2));  // Centered vertically
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.print(text);

  // Update the text's position
  textX--;

  // If the text has completely scrolled off the screen, reset to start from the right again
  if (textX < textMinX) {
    textX = screenWidth;
    tft.fillScreen(ST77XX_BLACK);
  }

  delay(1);  // Adjust delay to control scroll speed
}

void ScrollingText::calculateTextBounds() {
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(fontSize);
  tft.setTextWrap(0);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
 
  textMinX = -w;
}
