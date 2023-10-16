/**
 * Libraries needed:
 * Wifi 1.2.7
 * EspSoftwareSerial 8.1.0
 * Firebase ESP32 Client 4.3.14
 * TinyGPSPlus 1.0.3
 */

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

const char* ssid = "smartbin";
const char* password = "smartbin";
#define RXD2 16
#define TXD2 17
#define FIREBASE_HOST "https://smartbin-d94f7-default-rtdb.europe-west1.firebasedatabase.app/"
#define FIREBASE_AUTH "96f39500cb59f6f63ca1d780e9a35b10ba1c2633"

#define DATABASE_PATH "/smartbin/Bin1/percentage"

const int trigPin = 5;
const int echoPin = 18;

const float trashCanHeight = 30;

TinyGPSPlus gps;
FirebaseData firebaseData;
EspSoftwareSerial::UART ss;
void setup() {
  Serial.begin(9600);
  ss.begin(9600, SWSERIAL_8N1, RXD2, TXD2, false);
  if (!ss) {
    Serial.println("Serial initialisation failed");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.println("Firebase initialized");


  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration * 0.034) / 2; 

  float percentage = 0.0f;
  if (distance <= trashCanHeight) {
    percentage = float(((trashCanHeight - distance) / trashCanHeight) * 100);
    Serial.print("Trash can is ");
    Serial.print(percentage);
    Serial.println("% full");

    Firebase.setFloat(firebaseData,DATABASE_PATH ,percentage);
    Serial.print("Data saved to Firebase: ");
    Serial.println(firebaseData.floatData());
  }

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }
  Serial.println("");

  delay(1000);
}

void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid()){
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }
}
