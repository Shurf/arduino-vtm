#include <Arduino.h>


#include "SPI.h"
#include "Adafruit_ILI9341.h"
#include "ankh.h"

#define LCD_cs 33
#define LCD_dc 26
#define LCD_rst 27

class LcdScreen
{
public:
    LcdScreen();

    void updateDisplay(String &time, String &chronicleName);
private:
    Adafruit_ILI9341 *tft;
};