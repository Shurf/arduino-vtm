#include <Arduino.h>

#define MINUTES_IN_HOUR 60
#define HOURS_IN_DAY 24


class TimeParameters
{

public:

    TimeParameters();

    TimeParameters(String& currentTime, String& nightStart, String& nightEnd);

    void setCurrentTime(String& currentTime);

    void setNightStart(String& nightStart);

    void setNightEnd(String& nightEnd);

    void addTime(int hours, int minutes);

    void addTime(String& time);

    void resetToNightStart();

    void getCurrentTime(String& string);

    int getHours();

    int getTens();

    int getOnes();

private:
    int currentHours;
    int currentMinutes;

    int nightStartHours;
    int nightStartMinutes;

    int nightEndHours;
    int nightEndMinutes;

    void parseTimeString(String &string, int *hours, int *minutes);
};