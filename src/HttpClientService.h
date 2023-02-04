#ifndef HttpClientService_h
#define HttpClientService_h
#include "Arduino.h"
#include "UrlEncoderDecoder.h"
#include <ESP8266mDNS.h>
#include <ESP8266HttpClient.h>

class HttpClientService
{
    private:
        String _latitude;
        String _longitude;

        String _SMSServiceFingerPrint;
        String _SMSServiceToSMS;
        String _SMSServiceFromSMS;
        String _SMSServiceAccountIdSMS;
        String _SMSServiceToken;
        String _SMSServiceHost;
        int _SMSServiceHttpsPort;

        String _weatherApiKey;
        String _weatherFingerPrint;
        int _weatherHttpsPort;
        String _weatherHost;

        String _dateTimeApiKey;
        String _dateTimeApiHost;

        UrlEncoderDecoderService _urlEncoderDecoder;
        HTTPClient _client;
        WiFiClient _wifiClient;
        WiFiClientSecure _wifiClientSecure;

    public:
        HttpClientService();
        void SendSMS(String message);
        String GetCurrentDateTime(); 
        String GetCurrentWeather();
};

#endif