#include "DHT.h"
#include <Adafruit_Sensor.h>

//D5 is a GPIO port in Arduino MKR1010 and corresponds to Pin 14
//https://docs.arduino.cc/static/2a06d42b63ff944a11b29a73368e4aec/ABX00023-datasheet.pdf
//You can see a tutorial: https://learn.adafruit.com/dht/overview

#define DHTPIN 5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//D0 is a GPIO port in Arduino MKR1010 and corresponds to Pin 9
const int ledPin = 0;
int ledState = LOW;

//Set timer for LED and readings tasks
unsigned long previousMillis = 0;
unsigned long previousLedMillis = 0;

//Interval for reading Temperature and Humidity but also ON/OFF led
long sensorReadInterval = 30000;
long ledInterval = 900;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
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

    //Print info (Humidity, Temperature and Heat Index in Serial port)
    //Example: Humidity: 84.70%  Temperature: 28.60°C   Heat index: 34.58°C
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(F("  Heat index: "));
    Serial.print(hic);
    Serial.println(F("°C "));
  
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