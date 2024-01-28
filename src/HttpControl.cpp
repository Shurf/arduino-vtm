#include "HttpControl.h"

bool HttpControl::getChronicleParameters(String& currentTime, String& nightStart, String& nightEnd, String& chronicleName)
{
    HTTPClient http;
    String serverResponse;
    JsonDocument doc;
    http.begin(getCurrentChronicleUrl);

    if(http.GET() != 200)
    {
        Serial.println("HTTP Error");
        return false;
    }

    serverResponse = http.getString();
    deserializeJson(doc, serverResponse);

    currentTime = ((const char*)doc["current_time"]);
    nightStart = ((const char*)doc["night_start"]);
    nightEnd = ((const char*)doc["night_end"]);
    chronicleName = ((const char*)doc["chronicle_name"]);

    return true;
}

bool HttpControl::updateChronicleCurrentTime(String& currentTime)
{
    JsonDocument doc;
    String requestBody;
    HTTPClient http;
    
    doc["current_time"] = currentTime;

    serializeJson(doc, requestBody);

    http.begin(setCurrentChronicleTime);
    http.addHeader("Content-Type", "application/json");
    http.POST(requestBody);

    return true;
}