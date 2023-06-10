#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"



//D5 is a GPIO port in Arduino MKR1010 and corresponds to Pin 14
//https://docs.arduino.cc/static/2a06d42b63ff944a11b29a73368e4aec/ABX00023-datasheet.pdf
//You can see a tutorial: https://learn.adafruit.com/dht/overview

#define DHTPIN 5
#define DHTTYPE DHT22
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;      // the Wifi radio's status

DHT dht(DHTPIN, DHTTYPE);

//BoardId
const char deviceId[] = "53306a39-490a-41ed-80a9-22024aa26a79";

//D0 is a GPIO port in Arduino MKR1010 and corresponds to Pin 9
const int ledPin = 0;
int ledState = LOW;

//Set timer for LED and readings tasks
unsigned long previousMillis = 0;
unsigned long previousLedMillis = 0;

//Interval for reading Temperature and Humidity but also ON/OFF led
long sensorReadInterval = 3600000;
long ledInterval = 1000;

void printData() {
  Serial.println("Board Information:");
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  IPAddress mask = WiFi.subnetMask();
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.print("IP Mask: ");
  Serial.println(mask);
  Serial.print("IP Gateway: ");
  Serial.println(gateway);
  Serial.println("********************************");
  Serial.println("Network Information:");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
}

void getSensorData(){

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if(isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read sensor"));
      return;
    }
    float hic = dht.computeHeatIndex(t, h, false);
    unsigned long currentTime = WiFi.getTime();
    //Print info (Humidity, Temperature and Heat Index in Serial port)
    //Example: Humidity: 84.70%  Temperature: 28.60°C   Heat index: 34.58°C
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temp: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(F("  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(F("  Timestamp: "));
    Serial.print(currentTime);
    Serial.println(F(" sec"));
    Serial.print(F("  DeviceId: "));
    Serial.println(deviceId);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  Serial.println("----------------------------------------");
  printData();
  Serial.println("----------------------------------------");

  // Initialize DHT22 Sensor
  dht.begin();
  getSensorData();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  unsigned long currentLedMillis = millis();

  //validate if interval time has elapsed to take another sensor reading
  if(currentMillis - previousMillis >= sensorReadInterval) {
    previousMillis = currentMillis;
    getSensorData();
  }
  
  //validate if interval LED time has elapsed
  if(currentLedMillis - previousLedMillis >= ledInterval) {
    previousLedMillis = currentLedMillis;
    
    if(ledState == LOW) {
      ledState = HIGH;
    }
    else {
      ledState = LOW;
    }
    //Turn led On/off
    digitalWrite(ledPin, ledState);
  }
}