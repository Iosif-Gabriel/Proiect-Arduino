#include <SoftwareSerial.h>
#include "DHT.h"
#include <Wire.h>

#define DHTPIN 12   
#include <Servo.h> 
Servo myservo;     

#define DHTTYPE DHT22  // DHT 22  (AM2302)

int sensorMoistSoil = A0;
int sensorValue = 0;
int percentValue = 0;
const int ldrPin = A1;
const int relay = 3;

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

char inchar;  // Will hold the incoming character from the GSM shield

int bluetoothTx = 7;  // bluetooth tx to 7 pin
int bluetoothRx = 8;  // bluetooth rx to 8 pin

SoftwareSerial blue(bluetoothTx, bluetoothRx);  

int powerb = 6;

String airSensor;
String soilSensor;
String lightSensor;
String humidity = " Humidity: %";
String temperature = "   Temperature";
String sign = " *C";
String pump;

#define trigPin 13
#define echoPin 11


void setup() {
  pinMode(relay, OUTPUT);
  Serial.begin(9600);
  blue.begin(9600);  // original 19200
  pinMode(powerb, OUTPUT);
  digitalWrite(powerb, HIGH);
  dht.begin();
  myservo.attach(9);  

  pinMode(trigPin, OUTPUT);

  Wire.begin();

  pinMode(echoPin, INPUT);
  pinMode(ldrPin, INPUT);
}


void loop() {

  if (blue.available() == 0);
  digitalWrite(relay, HIGH);

  if (blue.available() > 0) {
    inchar = blue.read();
    Serial.println(inchar);
    delay(20);
    if (inchar == 'x') {
      delay(10);

      Serial.println(inchar);

      // Wait a few seconds between measurements.
      delay(2000);

      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      int h = dht.readHumidity();
      // Read temperature as Celsius
      int t = dht.readTemperature();
      // Read temperature as Fahrenheit
      int f = dht.readTemperature(true);

      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }

      sensorValue = analogRead(sensorMoistSoil);
      Serial.print("\n\nAnalogValue:");
      Serial.print(sensorValue);

      percentValue = map(sensorValue, 1023, 200, 0, 100);

      Serial.print("\nPercentValue: ");
      Serial.print(percentValue);
      Serial.print("%");

      Serial.print("Soil Moisture");
      Serial.print("Percent: ");
      Serial.print(percentValue);
      Serial.print("% \n\n\n");
      delay(5000);

      sensorValue = analogRead(sensorMoistSoil);
      Serial.print(sensorValue);

      // Compute heat index
      // Must send in temp in Fahrenheit!
      int hi = dht.computeHeatIndex(f, h);

      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");


      int ldrStatus = analogRead(ldrPin);
      Serial.print(ldrStatus);
      delay(1000);

      airSensor = airSensor + "AIR HUMIDITY: ";
      airSensor.concat(h);
      airSensor = airSensor + "% AIR TEMPERATURE: ";
      airSensor.concat(t);
      airSensor = airSensor + "*C";
      blue.print(airSensor);
      Serial.println(airSensor);
      delay(2000);
      airSensor = " ";

      soilSensor = soilSensor + "    SOIL HUMIDITY: ";
      soilSensor.concat(percentValue) + "%";
      blue.print(soilSensor + "%");
      Serial.println(soilSensor + "%");
      delay(2000);
      soilSensor = " ";

      if (ldrStatus > 400) {
        lightSensor = lightSensor + "  LUMINOSITY: ";
        lightSensor.concat(ldrStatus);
        lightSensor = lightSensor + " (Good light)\n";
        blue.print(lightSensor);
        Serial.println(lightSensor);
        delay(2000);
        lightSensor = " ";
      } else if (ldrStatus > 150 && ldrStatus < 500) {
        lightSensor = lightSensor + "  LUMINOSITY: ";
        lightSensor.concat(ldrStatus);
        lightSensor = lightSensor + " (Medium light)\n";
        blue.print(lightSensor);
        Serial.println(lightSensor);
        delay(2000);
        lightSensor = " ";
      } else {
        lightSensor = lightSensor + "  LUMINOSITY: ";
        lightSensor.concat(ldrStatus);
        lightSensor = lightSensor + " (Bad light)\n";
        blue.print(lightSensor);
        Serial.println(lightSensor);
        delay(2000);
        lightSensor = " ";
      }
    }else if(percentValue>= 60 && inchar == 's'){
      pump="Pump off \n";
      blue.print(pump);
      Serial.println(pump);
      digitalWrite(relay, HIGH);
      delay(6000);
    } else if (inchar == 's') {
      pump="Pump on \n";
      blue.print(pump);
      Serial.println(pump);
      digitalWrite(relay, LOW);
      delay(6000);
    }
  } else if (blue.available() > 0) {
    int servopos = blue.read();  // save the received number to servopos
    Serial.println(servopos);    // serial print servopos current number received from bluetooth
    myservo.write(servopos);     // roate the servo the angle received from the android app
  }
}