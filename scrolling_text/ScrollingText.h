#ifndef SCROLLINGTEXT_H
#define SCROLLINGTEXT_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

class ScrollingText {
  public:
    ScrollingText(Adafruit_ST7789 &display, int screenWidth, int screenHeight, int fontSize, String text);

    void update();

  private:
    Adafruit_ST7789 &tft;
    int screenWidth;
    int screenHeight;
    String text;
    int textX;
    int textMinX;
    int fontSize;

    void calculateTextBounds();
};

#endif // SCROLLINGTEXT_H
