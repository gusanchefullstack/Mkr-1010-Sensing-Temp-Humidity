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
const char deviceId[] = "ArduinoMkrwifi1010";

//D0 is a GPIO port in Arduino MKR1010 and corresponds to Pin 9
const int ledPin = 0;
int ledState = LOW;

//Set timer for LED and readings tasks
unsigned long previousMillis = 0;
unsigned long previousLedMillis = 0;

//Interval for reading Temperature and Humidity but also ON/OFF led
long sensorReadInterval = 60000;
long ledInterval = 1000;

void printData() {
  Serial.println("Board Information:");
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println();
  Serial.println("Network Information:");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
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
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  unsigned long currentLedMillis = millis();

  //validate if interval time has elapsed to take another sensor reading
  if(currentMillis - previousMillis >= sensorReadInterval) {
    previousMillis = currentMillis;
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
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(F("  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(F("  Current time: "));
    Serial.print(currentTime);
    Serial.println(F(" sec"));
    Serial.print(F("  DeviceId: "));
    Serial.println(deviceId);
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