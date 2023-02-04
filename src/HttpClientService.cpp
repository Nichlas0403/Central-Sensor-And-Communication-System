#include "HttpClientService.h"
#include "Arduino.h"

HttpClientService::HttpClientService() : _urlEncoderDecoder(), _client(), _wifiClient(), _wifiClientSecure()
{

}

void HttpClientService::SendSMS(String message)
{
  Serial.println("");
  Serial.println("-------------- SendSMS ------------- ");

  int fingerprintLength = _SMSServiceFingerPrint.length() + 1; 
  char fingerPrintArray[fingerprintLength];
  _SMSServiceFingerPrint.toCharArray(fingerPrintArray, fingerprintLength);


  Serial.println("Setting up client...");
  _wifiClientSecure.setFingerprint(fingerPrintArray);
  if(_wifiClientSecure.connect(_SMSServiceHost, _SMSServiceHttpsPort))
  {
    Serial.println("Connected to " + _SMSServiceHost);
  }
  else
  {
    Serial.println("Could not connect to " + _SMSServiceHost);
  }
        
  _client.begin(_wifiClientSecure, "https://" + _SMSServiceHost + "/2010-04-01/Accounts/" + _SMSServiceAccountIdSMS + "/Messages.json");
  _client.addHeader("Content-Type", "application/x-www-form-urlencoded");
  _client.addHeader("Authorization", _SMSServiceToken);
  String data = "To=" + _urlEncoderDecoder.urlencode(_SMSServiceToSMS) + "&From=" + _urlEncoderDecoder.urlencode(_SMSServiceFromSMS) + "&Body=" + _urlEncoderDecoder.urlencode(message);

  _client.POST(data);
  _client.end();
  Serial.println("Connection to " + _SMSServiceHost + " has ended.");
}

String HttpClientService::GetCurrentDateTime()
{
    Serial.println("");
    Serial.println("-------------- GetCurrentDateTime ------------- ");

    _client.begin(_wifiClient, "http://" + _dateTimeApiHost + "/v2.1/get-time-zone?key=" + _dateTimeApiKey + "&format=json&by=position&lat=" + _latitude + "&lng=" + _longitude);

    int httpResponseCode = _client.GET();

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = _client.getString();
        Serial.println(payload);
        return payload;
      }
      else 
      {
        Serial.print("Error code: ");
        Serial.println(_client.errorToString(httpResponseCode));
        Serial.println(httpResponseCode);
        return "";
      }

      //https://timezonedb.com/
}

String HttpClientService::GetCurrentWeather()
{
    Serial.println("");
    Serial.println("-------------- GetCurrentWeather ------------- ");

    String url = "https://" + _weatherHost + "/data/2.5/weather?lat=" + _latitude + "&lon=" + _longitude + "&appid=" + _weatherApiKey;

    int fingerprintLength = _weatherFingerPrint.length() + 1; 
    char fingerPrintArray[fingerprintLength];
    _weatherFingerPrint.toCharArray(fingerPrintArray, fingerprintLength);

    _wifiClientSecure.setFingerprint(fingerPrintArray);
    if(_wifiClientSecure.connect(_weatherHost, _weatherHttpsPort))
    {
      Serial.println("Connected to " + _weatherHost);
    }
    else
    {
      Serial.println("Could not connect to " + _weatherHost);
    }

    _client.begin(_wifiClientSecure, url);

    int httpResponseCode = _client.GET();

    if (httpResponseCode > 0) 
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = _client.getString();
      Serial.println(payload);
      return payload;
    }
    else 
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      return "";
    }
}

