/*
 * Air quality Sensor based on 
 * WEMOS D1 MINI
 * MQ135 for air particle
 * BME280 for air baro
 * 
 * Lib contribution:
 *  Georg Krocker https://github.com/GeorgK/MQ135
 *  Adafruit
 * 
 * Developed Nicolas GRANIER - Jan 2017
*/

//Lib 
#include "MQ135.h"
#include <ESP8266WiFi.h> // Lib Wifi
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//BME280 pinout
#define BME_SCK 0
#define BME_MISO 5
#define BME_MOSI 2
#define BME_CS 4
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

//MQ7 (CO sensor for next update)



//MQ135 Pinout
const int mq135Pin = 0;    //  MQ135 PIN alloc (A0)
MQ135 gasSensor = MQ135(mq135Pin);  // MQ135 PIN Init

// Wifi init
const char* ssid = "xxxxxx"; //  Wifi SSID
const char* password = "xxxxxx"; // Wifi pwd

//Jeedom init
String APIKEYJEEDOM = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // API Key  Jeedom
const char* host = "192.168.xxx.xxx"; // Local IP
const int httpPort = 80;

// Url for parameter sending
String url ="/core/api/jeeApi.php?apikey=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx&type=virtual&id=";// API Key Jeedom
 
//Jeedom parameter number (virtual) to be upadted with your parameter number
String NumeroVirtuelCo2 = "xxxx"; 
String NumeroVirtuelTemp = "xxxx"; 
String NumeroVirtuelPress = "xxxx"; 
String NumeroVirtuelHum = "xxxx"; 
 
void setup()
{
    Serial.begin(9600);   // Init Wemos serial port at 9600 bps 
    float rzero = gasSensor.getRZero();
    Serial.print("R0: ");
    Serial.println(rzero);  // R0 to calibrate MQ135 in file mq135.h,  ligne 27  after 48h warmup

// Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  //Init BME280
  Serial.println(F("BME280 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
    
}
 
void loop()
{
    //Atmo sensor
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature()-2); // -2 for offset
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 98.1F);
    Serial.println(" hPa");

     Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");


    //Gaz Sensor
    float ppm = gasSensor.getPPM();
    Serial.print("A0: ");
    Serial.print(analogRead(mq135Pin));
    float rzero = gasSensor.getRZero();
    Serial.print(" /R0: ");
    Serial.print(rzero);  // For sensor qualibration Xcheck    
    Serial.print(" /ppm CO2: ");
    Serial.println(ppm);




    //Send to Jeedom
      WiFiClient client;

      String urlCo2 = url;
      urlCo2 += NumeroVirtuelCo2;
      urlCo2 += "&value=";
      urlCo2 += ppm;
      
      String urlTemp = url;
      urlTemp += NumeroVirtuelTemp;
      urlTemp += "&value=";
      urlTemp += bme.readTemperature()-2;
      
      String urlPress = url;
      urlPress += NumeroVirtuelPress;
      urlPress += "&value=";
      urlPress += bme.readPressure()/98.1F;
      
      String urlHum = url;
      urlHum += NumeroVirtuelHum;
      urlHum += "&value=";
      urlHum += bme.readHumidity()+2;

      if(client.connect(host, httpPort)){
      client.print(String("GET ") + urlCo2 + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
       
      }
      delay(2000);    //  2s delay for not overload jeedom 
         if(client.connect(host, httpPort)){
      client.print(String("GET ") + urlTemp + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
                   //Serial.println(urlTemp);
       
      }
      delay(2000);    
         if(client.connect(host, httpPort)){
      client.print(String("GET ") + urlPress + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
                   //Serial.println(urlPress);
       
      }
      delay(2000);    
         if(client.connect(host, httpPort)){
      client.print(String("GET ") + urlHum + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n");
       
      }

      

      client.flush();
    client.stop();
    

        //refresh delay
    delay(30000);    // Data updated every 30s

    
}
