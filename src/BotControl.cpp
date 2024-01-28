#include "BotControl.h"

Bot::Bot(WiFiClientSecure &client)
{
    timeParameters = new TimeParameters();

    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
    bot = new UniversalTelegramBot(BOTtoken, client);
    lastTimeBotRan = millis();

#ifdef USE_FLIPCLOCK
    flipClock = new FlipClock();
#endif

#ifdef USE_SCREEN
    lcdScreen = new LcdScreen();
#endif
    updateTimeParametersFromServer();
    updateOutput();
}

void Bot::updateTimeParametersFromServer()
{
    String currentTime;
    String nightStart;
    String nightEnd;

    if(httpControl.getChronicleParameters(currentTime, nightStart, nightEnd, chronicleName) != true)
        return;
    timeParameters->setNightStart(nightStart);
    timeParameters->setNightEnd(nightEnd);
    timeParameters->setCurrentTime(currentTime);
}

void Bot::updateCurrentTimeOnServer()
{
    String timeString;
    timeParameters->getCurrentTime(timeString);        
    httpControl.updateChronicleCurrentTime(timeString);
}

void Bot::checkNewMessages()
{
    if (millis() > lastTimeBotRan + botRequestDelay)
    {
        int numNewMessages = bot->getUpdates(bot->last_message_received + 1);

        while (numNewMessages)
        {
            handleNewMessages(numNewMessages);
            numNewMessages = bot->getUpdates(bot->last_message_received + 1);
        }
        lastTimeBotRan = millis();
    }
}

void Bot::updateOutput()
{
#ifdef USE_FLIPCLOCK
    flipClock->setPosition(timeParameters->getHours(), timeParameters->getTens(), timeParameters->getOnes());
#endif

#ifdef USE_SCREEN
    String currentTime;
    timeParameters->getCurrentTime(currentTime);
    lcdScreen->updateDisplay(currentTime, chronicleName);    
#endif
    updateCurrentTimeOnServer();
}

int Bot::splitBySpaces(String text, String *stringArray, int stringArraySize)
{
    int i = 0;
    int k = 0;
    int n = 0;
    int cnt = 0;
    char string[MAX_STRING_LENGTH];

    while (cnt < stringArraySize)
    {
        while (text[i] != ' ' && text[i])
        {
            string[k] = text[i];
            i++;
            k++;
        }
        i++;
        string[k] = 0;
        stringArray[n] = string;
        k = 0;
        n++;
        cnt++;
    }

    return n;
}

void Bot::processTimeAndSessionCommand(String text)
{
    String stringArray[3];
    char output[MAX_STRING_LENGTH];

    if(splitBySpaces(text, stringArray, 3) != 3)
        return;

    chronicleName = stringArray[2];
    timeParameters->setCurrentTime(stringArray[1]);

    updateOutput();
}

void Bot::processTimeCommand(String text)
{
    String stringArray[2];
    char output[MAX_STRING_LENGTH];

    if(splitBySpaces(text, stringArray, 2) != 2)
        return;

    timeParameters->setCurrentTime(stringArray[1]);

    updateOutput();
}

void Bot::processAddTimeCommand(String text)
{
    String stringArray[2];
    char output[MAX_STRING_LENGTH];

    if(splitBySpaces(text, stringArray, 2) != 2)
        return;

    timeParameters->addTime(stringArray[1]);

    updateOutput();
}

void Bot::botSetup()
{
    /*bool result = bot->setMyCommands(F("["
                            "{\"command\":\"" RESET_TO_START "\", \"description\":\"Reset to night start\"},"
                            "{\"command\":\"" SET_TIME_AND_SESSION "\", \"description\":\"Set time and session\"},"
                            "{\"command\":\"" SET_TIME "\", \"description\":\"Set time\"},"
                            "{\"command\":\"" ADD_TIME "\", \"description\":\"Add time\"},"
                            "{\"command\":\"" UPDATE_FROM_SERVER "\", \"description\":\"Update from server\"}"
                            "]"
    ));*/
}

void Bot::sendBotControlMessage(String &chat_id)
{
    String message;
    String currentTime;
    timeParameters->getCurrentTime(currentTime);
    message = "Current chronicle: " + chronicleName + ", current time is " + currentTime;

    bot->sendMessage(chat_id, message, "");
    String keyboardJson = F(
        "["
        "[{\"text\":\"Reset to start\", \"callback_data\":\"" RESET_TO_START "\"}, {\"text\":\"Update from server\", \"callback_data\":\"" UPDATE_FROM_SERVER "\"}]," 
        "[{\"text\":\"Add 15 minutes\", \"callback_data\":\"" ADD_TIME " 00:15\"}, {\"text\":\"Add 30 minutes\", \"callback_data\":\"" ADD_TIME " 00:30\"}],"
        "[{\"text\":\"Add 1 hour\", \"callback_data\":\"" ADD_TIME " 01:00\"}]"
        "]");
        //"[[{\"text\":\"Reset to start\", \"callback_data\":\"/" RESET_TO_START "\"}]]");
    //bot->sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
    bot->sendMessageWithInlineKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson);
}

// Handle what happens when you receive new messages
void Bot::handleNewMessages(int numNewMessages)
{
    //Serial.println("handleNewMessages");
    //Serial.println(String(numNewMessages));

    for (int i = 0; i < numNewMessages; i++)
    {
        // Chat id of the requester
        String chat_id = String(bot->messages[i].chat_id);
        if (chat_id != CHAT_ID)
        {
            bot->sendMessage(chat_id, "Unauthorized user", "");
            continue;
        }

        // Print the received message
        String text = bot->messages[i].text;
        // Serial.println(text);

        String from_name = bot->messages[i].from_name;

        if (text == START)
        {
            String welcome = "Welcome, " + from_name + ".\n";
            welcome += "Use the following commands to control your outputs.\n\n";
            welcome += "" SET_TIME_AND_SESSION " <time> <session>\n";
            welcome += "" SET_TIME " <time>\n";
            welcome += "" ADD_TIME " <time>\n";
            welcome += "" RESET_TO_START "\n";
            welcome += "" UPDATE_FROM_SERVER "\n";
            bot->sendMessage(chat_id, welcome, "");
            sendBotControlMessage(chat_id);
        }

        if (text.startsWith(SET_TIME_AND_SESSION))
        {
            processTimeAndSessionCommand(text);
            sendBotControlMessage(chat_id);
        }
        else if (text.startsWith(SET_TIME))
        {
            processTimeCommand(text);
            sendBotControlMessage(chat_id);
        }
        else if (text.startsWith(RESET_TO_START))
        {
            timeParameters->resetToNightStart();
            updateOutput();
            sendBotControlMessage(chat_id);
        }
        else if (text.startsWith(ADD_TIME))
        {
            processAddTimeCommand(text);
            sendBotControlMessage(chat_id);
        }
        else if (text.startsWith(UPDATE_FROM_SERVER))
        {
            updateTimeParametersFromServer();
            updateOutput();
            sendBotControlMessage(chat_id);
        }
    }
}