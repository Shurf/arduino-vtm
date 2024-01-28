#include "Arduino.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"

class HttpControl 
{    
public:

    bool getChronicleParameters(String& currentTime, String& nightStart, String& nightEnd, String& chronicleName);
    bool updateChronicleCurrentTime(String& currentTime);
    
private:
    String getCurrentChronicleUrl = "http://192.168.0.104:8000/vtm_server/chronicles/current";
    String setCurrentChronicleTime = "http://192.168.0.104:8000/vtm_server/chronicles/current/set_time";
};