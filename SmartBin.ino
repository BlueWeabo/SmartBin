#include <WiFi.h>
#include <FirebaseESP32.h>

const char* ssid = "smartbin";
const char* password = "smartbin";

#define FIREBASE_HOST "https://smartbin-d94f7-default-rtdb.europe-west1.firebasedatabase.app/"
#define FIREBASE_AUTH "96f39500cb59f6f63ca1d780e9a35b10ba1c2633"

#define DATABASE_PATH "/smartbin/percentage"

const int trigPin = 5;
const int echoPin = 18;

const int trashCanHeight = 30;


FirebaseData firebaseData;

void setup() {
  Serial.begin(9600);
  
  
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

  float percentage = 0.0;
  if (distance <= trashCanHeight) {
    percentage = ((float)(trashCanHeight - distance) / trashCanHeight) * 100;
    Serial.print("Trash can is ");
    Serial.print(percentage);
    Serial.println("% full");

    Firebase.setFloat(firebaseData,DATABASE_PATH ,percentage);
    if (firebaseData.dataType() == "float") {
      Serial.print("Data saved to Firebase: ");
      Serial.println(firebaseData.floatData());
    } else {
      Serial.println("Error saving data to Firebase");
    }
  } 

  delay(1000);
}
