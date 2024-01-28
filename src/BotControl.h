#include <Arduino.h>
#include <UniversalTelegramBot.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include "HttpControl.h"
#include "TimeControl.h"

#include "Config.h"
#include "secrets.h"

#ifdef USE_FLIPCLOCK
#include "FlipClock.h"
#endif

#ifdef USE_SCREEN
#include "LcdScreen.h"
#endif





#define MAX_STRING_LENGTH 50

class Bot
{

#define RESET_TO_START "/reset_to_start"
#define ADD_15_MIN "/add_15_min"
#define ADD_30_MIN "/add_30_min"
#define ADD_1_HR "/add_1_hr"
#define SET_TIME_AND_SESSION "/set_time_and_session"
#define SET_TIME "/set_time"
#define ADD_TIME "/add_time"
#define START "/start"
#define UPDATE_FROM_SERVER "/update_from_server"

public:
    Bot(WiFiClientSecure &client);

    void updateTimeParametersFromServer();

    void updateCurrentTimeOnServer();

    void checkNewMessages();

private:

    void updateOutput();

    int splitBySpaces(String text, String *stringArray, int stringArraySize);

    void processTimeAndSessionCommand(String text);

    void processTimeCommand(String text);

    void processAddTimeCommand(String text);

    void botSetup();

    void sendBotControlMessage(String &chat_id);

    void handleNewMessages(int numNewMessages);

    UniversalTelegramBot *bot;
#ifdef USE_FLIPCLOCK
    FlipClock *flipClock;
#endif
#ifdef USE_SCREEN
    LcdScreen * lcdScreen;
#endif
    TimeParameters* timeParameters;
    HttpControl httpControl;
    String chronicleName = "[TBD]";
    // Checks for new messages every 1 second.
    int botRequestDelay = 1000;
    unsigned long lastTimeBotRan;
};