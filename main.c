#include <WiFi.h>
#include "ThingSpeak.h"
#include "DHT.h"

const char* ssid = "Galaxy A52s 5GD8E1";   // your network SSID (name)
const char* password = "mdop6429";   // your network password

WiFiClient  client;


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define DHTPIN 4
#define DHTTYPE DHT11
#define VT_PIN 35// connect VT
//#define AT_PIN A1 // connect AT

DHT dht(DHTPIN, DHTTYPE);
#define ARDUINO_WORK_VOLTAGE 5

unsigned long myChannelNumber = 3;
const char * myWriteAPIKey = "MR8SISLTZ6FBY828";

unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

LiquidCrystal_I2C lcd(0x27, 20, 4); // LCD2004

void setup()
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  dht.begin();
  lcd.init();  // initialize the lcd
  lcd.backlight();
  lcd.print("IIUC EEE");

  delay(1000);

  lcd.clear();
}

void loop(){
if ((millis() - lastTime) > timerDelay) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

    lcd.setCursor(0, 0);
    lcd.print("IoT Solar Power Monitor");

    int vt_temp = analogRead(VT_PIN);
    //  int at_temp = analogRead(AT_PIN);
  
    float voltage = vt_temp * (ARDUINO_WORK_VOLTAGE / 4095.0) * 5;
    float current = vt_temp * (ARDUINO_WORK_VOLTAGE / 4095.0);
    float power = voltage * current;
    lcd.setCursor(0, 1);
    lcd.print("Voltage= " + (String)voltage + " V  ");
    Serial.println("Voltage= " + (String)voltage + " V ");
    lcd.setCursor(0, 2);
    lcd.print("Current= " + (String)current + " A  ");
    Serial.println("Current= " + (String)current + " A  ");
    lcd.setCursor(0, 3);
    lcd.print("Power= " + (String)power + " Watt  ");
    Serial.println("Power= " + (String)power + " Watt  ");
    delay(500);
    
    delay(1500);
   float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, humidity);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temperature, humidity, false);

  Serial.println(F("Humidity: "));
  Serial.print(humidity);
  Serial.println(F("%  Temperature: "));
  Serial.print(temperature);
//  Serial.print(F("°C "));
//  Serial.print(f);
//  Serial.print(F("°F  Heat index: "));
//  Serial.print(hic);
//  Serial.print(F("°C "));
//  Serial.print(hif);
//  Serial.println(F("°F"));

    ThingSpeak.setField(1, current);
    ThingSpeak.setField(2, voltage);
    ThingSpeak.setField(3, power);
    ThingSpeak.setField(4, humidity);
    ThingSpeak.setField(5, temperature);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
    
}
