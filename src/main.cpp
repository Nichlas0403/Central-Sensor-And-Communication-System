#include "SensorService.h"
#include "HttpClientService.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

//Core values
#define _analogInputGPIO A0
#define _photoResistorGPIO D5
#define _humidityAndTemperatureGPIO D6





//Services
ESP8266WebServer _server(80);
HttpClientService _client;
SensorService _sensorService(_analogInputGPIO, _photoResistorGPIO);

//Core server functionality
void restServerRouting();
void connectToWiFi();

//Endpoints
void getPhotoresistorReading();

void setup() 
{
  Serial.begin(9600);
  pinMode(_analogInputGPIO, INPUT);
  pinMode(_photoResistorGPIO, OUTPUT);
  connectToWiFi();
}

void loop() 
{
  _server.handleClient();
  delay(1);
}








// ------------------------------------- SERVER ------------------------------------------


//ENDPOINTS
void sendSMS()
{
  String arg = "message";

  if(!_server.hasArg(arg))
  {
    _server.send(400, "text/json", "Missing argument: " + arg);
    return;
  }

  _client.SendSMS(arg);

  _server.send(200);

}

void healthCheck()
{
  _server.send(200);
  
}

void getPhotoresistorReading()
{
  _server.send(200, "text/json", String(_sensorService.GetPhotoresistorReading()));
  
}

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

// Core server functionality
void restServerRouting() 
{
  //HTTP services
  _server.on(F("/send-SMS"), HTTP_POST, sendSMS);
  _server.on(F("/health-check"), HTTP_GET, healthCheck);
  _server.on(F("/current-datetime"), HTTP_GET, getCurrentDateTime);
  _server.on(F("/weather"), HTTP_GET, getWeather);

  //Sensor services
  _server.on(F("/photoresistor-value"), HTTP_GET, getPhotoresistorReading);
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
  WiFi.mode(WIFI_STA);

  if (WiFi.SSID() != _wifiName) 
  {
    Serial.println("Creating new connection to wifi");
    WiFi.begin(_wifiName, _wifiPassword);
    WiFi.persistent(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }
  else
  {
    Serial.println("Using existing wifi settings...");
  }

 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(_wifiName);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();
  // Set not found response
  _server.onNotFound(handleNotFound);
  // Start server
  _server.begin();

  Serial.println("HTTP server started");
}



