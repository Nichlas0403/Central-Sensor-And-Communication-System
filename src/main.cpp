#include "SensorService.h"
#include "HttpClientService.h"
#include "EEPROM.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <SPI.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
// #include <ESP8266WiFi.h>
// #include <ESP8266mDNS.h>
// #include <ESP8266WebServer.h>

//Core values
// #define _analogInputGPIO A0
// #define _photoResistorGPIO D5
// #define _humidityAndTemperatureGPIO D6

//Services
WebServer _server(80);
HttpClientService _client;


//Certificate expiration
const int SmsCaExpirationYear = 2031;
const int SmsCaExpirationMonth = 10; //one month before expiration
const int weatherCaExpirationYear = 2038;
const int weatherCaExpirationMonth = 12; //one month before expiration

bool smsCaExpirationNotified = false;
bool weatherCaExpirationNotified = false;

//Certificate checks
unsigned long timeBetweenCertificateChecks = 86400000; //24 hours
unsigned long lastCertificateCheck;

void connectToWiFi();

void setup() {
    Serial.begin(9600);
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    connectToWiFi();
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1); //disable brownout detector
}

void loop() 
{
  _server.handleClient();
  delay(1);

  unsigned long currentTime = millis();

  if(currentTime < lastCertificateCheck) //currentTime has exceeded unsigned long max value
    ESP.restart();

  if(currentTime - lastCertificateCheck > timeBetweenCertificateChecks)
  {

    lastCertificateCheck = currentTime;

    String currentDate = _client.GetCurrentDateTime();
    int currentMonth = currentDate.substring(3,5).toInt();
    int currentYear = currentDate.substring(6,10).toInt();

    //sms CA Check
    if(currentYear == SmsCaExpirationYear)
      if(currentMonth == SmsCaExpirationMonth)
        if(!smsCaExpirationNotified)
        {
          _client.SendSMS("SMS CA is 1 month from expiring");
          smsCaExpirationNotified = true;
        }

    //sms CA Check
    if(currentYear == weatherCaExpirationYear)
      if(currentMonth == weatherCaExpirationMonth) 
        if(!weatherCaExpirationNotified)
        {
          _client.SendSMS("Weather CA is 1 month from expiring");
          weatherCaExpirationNotified = true;
        }

  }

}








// ------------------------------------- SERVER ------------------------------------------


// ENDPOINTS
void sendSMS()
{
  String arg = "message";

  if(!_server.hasArg(arg))
  {
    _server.send(400, "text/json", "Missing argument: " + arg);
    return;
  }

  _client.SendSMS(_server.arg(arg));

  _server.send(200);

}

void healthCheck()
{
  _server.send(200);
  
}

// void getPhotoresistorReading()
// {
//   _server.send(200, "text/json", String(_sensorService.GetPhotoresistorReading()));
  
// }

void getCurrentDateTime()
{
  String dateTime = _client.GetCurrentDateTime();
  _server.send(200, "text/json", dateTime);
}

void getWeather()
{

  String weatherData = _client.GetCurrentWeather();

  _server.send(200, "text/json", weatherData);

}

void WiFiChecks() //Checks if certain wifi SSID's are within range
{
    int networks = WiFi.scanNetworks();

    bool wifiWithinRange = false;

    for(int i = 0; i < networks; i++)
    {
      String currentSSID = WiFi.SSID(i);

      if(currentSSID == phone1 || currentSSID == phone2)
      {
        Serial.println(currentSSID);
        wifiWithinRange = true;
        break;
      }
    }

    _server.send(200, "text/json", String(wifiWithinRange));
}

// // Core server functionality
void restServerRouting() 
{
  //HTTP services
  
  _server.on(F("/send-SMS"), HTTP_POST, sendSMS);
  _server.on(F("/health-check"), HTTP_GET, healthCheck);
  _server.on(F("/current-datetime"), HTTP_GET, getCurrentDateTime);
  _server.on(F("/weather"), HTTP_GET, getWeather);
  _server.on(F("/wifi-check"), HTTP_GET, WiFiChecks);

  //Sensor services
  // _server.on(F("/photoresistor-value"), HTTP_GET, getPhotoresistorReading);
}

void handleNotFound() 
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += _server.uri();
  message += "_server: ";
  message += (_server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += _server.args();
  message += "\n";

  for (uint8_t i = 0; i < _server.args(); i++) 
  {
    message += " " + _server.argName(i) + ": " + _server.arg(i) + "\n";
  }

  _server.send(404, "text/plain", message);
}

void connectToWiFi()
{
  // Connect to Wi-Fi network with SSID and password
  WiFi.begin(_wifiName, _wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  restServerRouting();
  // Set not found response
  _server.onNotFound(handleNotFound);

  _server.begin();
  Serial.println("Server started!");
}



