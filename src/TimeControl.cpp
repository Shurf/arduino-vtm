#include "TimeControl.h"

TimeParameters::TimeParameters()
{
    currentHours = 20;
    currentMinutes = 0;
    nightStartHours = 20;
    nightStartMinutes = 0;
    nightEndHours = 8;
    nightEndMinutes = 0;
}

TimeParameters::TimeParameters(String& currentTime, String& nightStart, String& nightEnd)
{
    setCurrentTime(currentTime);
    setNightStart(nightStart);
    setNightEnd(nightEnd);
}

void TimeParameters::setCurrentTime(String& currentTime)
{
    parseTimeString(currentTime, &currentHours, &currentMinutes);
}

void TimeParameters::setNightStart(String& nightStart)
{
    parseTimeString(nightStart, &nightStartHours, &nightStartMinutes);
}

void TimeParameters::setNightEnd(String& nightEnd)
{
    parseTimeString(nightEnd, &nightEndHours, &nightEndMinutes);
}

void TimeParameters::addTime(int hours, int minutes)
{
    currentMinutes += minutes;
    if (currentMinutes >= MINUTES_IN_HOUR)
    {
        currentMinutes -= MINUTES_IN_HOUR;
        currentHours++;
    }

    currentHours += hours;
    if (currentHours >= HOURS_IN_DAY)
        currentHours -= HOURS_IN_DAY;
}

void TimeParameters::addTime(String& time)
{
    int hours;
    int minutes;
    parseTimeString(time, &hours, &minutes);
    addTime(hours, minutes);
}

void TimeParameters::resetToNightStart()
{
    currentHours = nightStartHours;
    currentMinutes = nightStartMinutes;
}

void TimeParameters::getCurrentTime(String& string)
{
    char tmp_str[20];
    sprintf(tmp_str, "%02d:%02d", currentHours, currentMinutes);
    string = tmp_str;
}

int TimeParameters::getHours() 
{
    return currentHours;
}

int TimeParameters::getTens() 
{
    return (currentMinutes - currentMinutes % 10)/10;
}

int TimeParameters::getOnes() 
{
    return currentMinutes % 10;
}

void TimeParameters::parseTimeString(String &string, int *hours, int *minutes)
{
    sscanf(string.c_str(), "%d:%d", hours, minutes);
}