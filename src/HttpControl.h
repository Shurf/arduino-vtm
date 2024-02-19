#include "Arduino.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"

class HttpControl 
{    
public:

    bool getChronicleParameters(String& currentTime, String& nightStart, String& nightEnd, String& chronicleName);
    bool updateChronicleCurrentTime(String& currentTime);
    
private:
    String getCurrentChronicleUrl = "http://vtm.haven/vtm_server/chronicles/current";
    String setCurrentChronicleTime = "http://vtm.haven/vtm_server/chronicles/current/set_time";
};