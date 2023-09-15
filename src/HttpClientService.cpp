#include "HttpClientService.h"
#include "Arduino.h"

HttpClientService::HttpClientService() : _urlEncoderDecoder(), _client(), _wifiClient(), _wifiClientSecure()
{


}

void HttpClientService::SendSMS(String message)
{
  int fingerprintLength = _SMSServiceFingerPrint.length() + 1; 
  char fingerPrintArray[fingerprintLength];
  _SMSServiceFingerPrint.toCharArray(fingerPrintArray, fingerprintLength);

  _wifiClientSecure.setFingerprint(fingerPrintArray);
  if(_wifiClientSecure.connect(_SMSServiceHost, _SMSServiceHttpsPort))
  {
    Serial.println("Sending text");

    _client.begin(_wifiClientSecure, "https://" + _SMSServiceHost + "/2010-04-01/Accounts/" + _SMSServiceAccountIdSMS + "/Messages.json");
    _client.addHeader("Content-Type", "application/x-www-form-urlencoded");
    _client.addHeader("Authorization", _SMSServiceToken);
    String data = "To=" + _urlEncoderDecoder.urlencode(_SMSServiceToSMS) + "&From=" + _urlEncoderDecoder.urlencode(_SMSServiceFromSMS) + "&Body=" + _urlEncoderDecoder.urlencode(message);
    _client.POST(data);
    _client.end();
  }
  else
  {
    Serial.println("Could not connect to " + _SMSServiceHost);
  }
        
}


String HttpClientService::GetCurrentWeather()
{
    String url = "https://" + _weatherHost + "/data/2.5/weather?lat=" + _latitude + "&lon=" + _longitude + "&appid=" + _weatherApiKey;

    int fingerprintLength = _weatherFingerPrint.length() + 1; 
    char fingerPrintArray[fingerprintLength];
    _weatherFingerPrint.toCharArray(fingerPrintArray, fingerprintLength);

    _wifiClientSecure.setFingerprint(fingerPrintArray);
    if(_wifiClientSecure.connect(_weatherHost, _weatherHttpsPort))
    {
      Serial.println("Retrieving weather data");
    }
    else
    {
      Serial.println("Could not connect to " + _weatherHost);
    }

    _client.begin(_wifiClientSecure, url);

    int httpResponseCode = _client.GET();

    if (httpResponseCode > 0) 
    {
      String payload = _client.getString();
      return payload;
    }
    else 
    {
      return String(httpResponseCode);
    }
}


String HttpClientService::GetCurrentDateTime()
{
    Serial.println("Retrieving current datetime");

    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, "pool.ntp.org");
    timeClient.update();
    unsigned long currentEpoch = timeClient.getEpochTime();

    char currentDateTime[32];
    sprintf(currentDateTime, "%02d-%02d-%02dTM%02d-%02d-%02d", day(currentEpoch), month(currentEpoch), year(currentEpoch), (hour(currentEpoch) + 1), minute(currentEpoch), second(currentEpoch));
    return currentDateTime;
}