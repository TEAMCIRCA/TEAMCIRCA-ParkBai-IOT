#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include <NewPing.h>

#define FIREBASE_HOST "https://parkbai-c8f04-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "HpVsDx4CNYQzEs8t5uq92UIRZlgxHMbut27OJStj"

// https://parkbai-c8f04-default-rtdb.firebaseio.com/     <== CIRCA FIREBASE HOST
// HpVsDx4CNYQzEs8t5uq92UIRZlgxHMbut27OJStj               <== CIRCA FIREBASE SECRET KEY

// https://parking-management-syste-c8da7-default-rtdb.asia-southeast1.firebasedatabase.app/     <== ROCS FIREBASE HOST
// IQMoHdAj7VgPZKNyBdHNpuN6fu72J5RZuwiadYOP                                                      <== ROCS FIREBASE SECRET KEY

// Replace with your network credentials
const char* ssid = "rocotoyz";
const char* password = "rocotoyz_13";

//ssid --> rocs13_2.4G
//password --> RngDQ5Dk   <== wifi credentials
//ssid --> rocotoyz
//password --> rocotoyz_13 <== mobile data credentials

#define TRIGGER_PIN D5
#define ECHO_PIN D6
#define MAX_DISTANCE 400.0
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);  //Max distance 400.0 cm
//const int ledPin = D7;

FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;
FirebaseJson json;

String ownerUID = "bFnYTNgnXMYLrwwjqDoxzreu7Rn2";

void setup() {
  Serial.begin(9600);
  //pinMode(ledPin, OUTPUT);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  Serial.println("Successfully connected.");
  Serial.print("ESP Board MAC Address:");
  Serial.println(WiFi.macAddress());
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);
  //Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  delay(100);
   double distance_cm = sonar.ping_cm();
   double distance_feet = distance_cm / 30.48;  // CM to Feet // distance_feet is directly calculated from distance_cm by dividing it by 30.48 (since 1 foot = 30.48 cm).
   double detectOccupied1 = 0.01;
   double detectOccupied2 = 3;
   double detectImproperPark1 = 4;
   double detectImproperPark2 = 13;
   double detecVacant = 0.00; 

  Serial.print("Distance: ");
  Serial.print(distance_feet);
  Serial.println(" feet");

  json.set("parking_number", 3);
  json.set("parking_section", "A");

  if (distance_feet >= detectOccupied1 && distance_feet <= detectOccupied2) {
    Serial.println("OCCUPIED");
    json.set("parking_space", "OCCUPIED");
    Firebase.updateNode(firebaseData, "PARK_OWNER/" + ownerUID + "/PARKING_AREA/A03", json);
  } else if (distance_feet >= detectImproperPark1 && distance_feet <= detectImproperPark2) {
    Serial.println("IMPROPER PARK");
    json.set("parking_space", "IMPROPER PARK");
    Firebase.updateNode(firebaseData, "PARK_OWNER/" + ownerUID + "/PARKING_AREA/A03", json);
  } else if (distance_feet <= detecVacant) {
    Serial.println("VACANT");
    json.set("parking_space", "VACANT");
    Firebase.updateNode(firebaseData, "PARK_OWNER/" + ownerUID + "/PARKING_AREA/A03", json);
  }
}


