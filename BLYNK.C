#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>


#define BLYNK_TEMPLATE_ID "TMPL3GxGMCtRv"
#define BLYNK_TEMPLATE_NAME "ff robot"

// Blynk Auth Token
char auth[] = "2jjoTgr7nIsi8CFG-LXDA6MxhCBaB130";  // Replace with your Blynk auth token

// WiFi credentials
char ssid[] = "ESP_83DD7E";
char pass[] = "Shubham@123";

// Pin Definitions
#define FLAME_SENSOR_1 D1
#define FLAME_SENSOR_2 D2
#define FLAME_SENSOR_3 D3
#define DHTPIN D4
#define SMOKE_SENSOR A0
#define DHTTYPE DHT11  // or DHT22 depending on your sensor

DHT dht(DHTPIN, DHTTYPE);

// Variables for sensor thresholds
int flameThreshold = 500;  // Adjust as per your sensor's sensitivity
int smokeThreshold = 300;  // Example threshold for smoke detection

void setup()
{
  Serial.begin(9600);
  
  // Blynk setup
  Blynk.begin(auth, ssid, pass);
  
  // DHT setup
  dht.begin();
  
  // Flame sensor pins
  pinMode(FLAME_SENSOR_1, INPUT);
  pinMode(FLAME_SENSOR_2, INPUT);
  pinMode(FLAME_SENSOR_3, INPUT);
}

void loop()
{
  Blynk.run();
  
  // Read flame sensors
  int flame1 = digitalRead(FLAME_SENSOR_1);
  int flame2 = digitalRead(FLAME_SENSOR_2);
  int flame3 = digitalRead(FLAME_SENSOR_3);
  
  // Read temperature and humidity
  float temp = dht.readTemperature();
  float smoke = analogRead(SMOKE_SENSOR);
  
  // Blynk virtual pins for temperature and smoke
  Blynk.virtualWrite(V1, temp);  // Send temperature to Blynk
  Blynk.virtualWrite(V2, smoke); // Send smoke level to Blynk
  
  // Flame detection logic
  if (flame1 == LOW || flame2 == LOW || flame3 == LOW) {
    // Fire detected
    Blynk.logEvent("Fire detected! Take action immediately!");
    Serial.println("Fire detected!");
  }

  // Check smoke threshold
  if (smoke > smokeThreshold) {
    Blynk.logEvent("High smoke level detected!");
    Serial.println("Smoke detected!");
  }
  
  // Print values to serial for debugging
  Serial.print("Flame 1: "); Serial.print(flame1);
  Serial.print(" Flame 2: "); Serial.print(flame2);
  Serial.print(" Flame 3: "); Serial.println(flame3);
  Serial.print("Temperature: "); Serial.println(temp);
  Serial.print("Smoke level: "); Serial.println(smoke);
  
  delay(2000);  // 2 second delay between sensor readings
}
