#include "LcdScreen.h"

LcdScreen::LcdScreen()
{
    tft = new Adafruit_ILI9341(LCD_cs, LCD_dc, LCD_rst);
    tft->begin();
    tft->setRotation(3);
}

void LcdScreen::updateDisplay(String &time, String &chronicleName)
{
    
    tft->fillScreen(ILI9341_BLACK);

    tft->setTextColor(ILI9341_WHITE);

    tft->drawRGBBitmap(0, 0, (uint16_t *)ankh_resize, ANKH_WIDTH, ANKH_HEIGHT);

    tft->setCursor(160, 20);
    tft->setTextSize(5);

    tft->println(time);

    tft->setCursor(160, 80);
    tft->setTextSize(2);
    tft->println(chronicleName);
}