#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <SoftwareSerial.h>

// Pin Definitions
#define DHTPIN 6          
#define POT_PIN A2        // Potentiometer for soil moisture
#define RELAY_PIN 13      

// DHT22 sensor
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// LCD 2004 (I2C) - Main Display
LiquidCrystal_I2C lcd2004(0x27, 20, 4);  

// LCD 1602 (Parallel) - Pump Status Display
LiquidCrystal lcd1602(7, 8, 9, 10, 11, 12); 

// SoftwareSerial for communication between Arduino and ESP32
SoftwareSerial esp32Serial(10, 11); // RX, TX pins

void setup() {
  // Initialize the LCD screens
  lcd2004.begin(20, 4);  
  lcd1602.begin(16, 2);  

  lcd2004.setCursor(0, 0);
  lcd2004.print("Monitoring System");

  lcd1602.setCursor(0, 0);
  lcd1602.print("Pump Status:");

  // Initialize DHT sensor
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Initially turn off the pump

  // Initialize communication with ESP32
  esp32Serial.begin(115200); // Set baud rate for ESP32
}

void loop() {
  // Read temperature and humidity from DHT22
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read soil moisture from potentiometer (soil sensor)
  int soilMoisture = analogRead(POT_PIN);
  soilMoisture = map(soilMoisture, 0, 1023, 0, 100);  // Convert to percentage

  // Display soil moisture and temperature on LCD2004
  lcd2004.clear();
  lcd2004.setCursor(0, 0);
  lcd2004.print("Soil Moisture: ");
  lcd2004.print(soilMoisture);
  lcd2004.print("%");

  lcd2004.setCursor(0, 1);
  lcd2004.print("Temp: ");
  lcd2004.print(temperature);
  lcd2004.print("C");

  // Display pump status on LCD1602 based on conditions
  if (soilMoisture < 40 || temperature > 38.0) {  // If soil moisture is low and temperature is high
    lcd1602.setCursor(0, 1);
    lcd1602.print("Pump On!        ");
    digitalWrite(RELAY_PIN, HIGH);  // Turn on the pump
  } else {
    lcd1602.setCursor(0, 1);
    lcd1602.print("Pump Off!       ");
    digitalWrite(RELAY_PIN, LOW);   // Turn off the pump
  }

  // Prepare data to send to ESP32
  String data = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + ",\"soilMoisture\":" + String(soilMoisture) + "}";
  
  // Send data to ESP32 via Serial
  esp32Serial.println(data);

  // Wait for the ESP32 to handle the request and Wi-Fi communication
  delay(5000);  // Wait for 5 seconds before next reading
}
