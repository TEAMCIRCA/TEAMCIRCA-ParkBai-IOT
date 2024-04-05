#include <ESP8266WiFi.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <RFID.h>
#include "FirebaseESP8266.h" // Install Firebase ESP8266 library

#define FIREBASE_HOST "https://parkbai-c8f04-default-rtdb.firebaseio.com"  //Without http:// or https:// schemes
#define FIREBASE_AUTH "HpVsDx4CNYQzEs8t5uq92UIRZlgxHMbut27OJStj"
// https://parkbai-c8f04-default-rtdb.firebaseio.com/     <== CIRCA FIREBASE HOST
// HpVsDx4CNYQzEs8t5uq92UIRZlgxHMbut27OJStj               <== CIRCA FIREBASE SECRET KEY

RFID rfid(D4, D3); //D4:pin of tag reader SDA. D3:pin of tag reader RST

unsigned char str[MAX_LEN]; //MAX_LEN is 16: size of the array
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const char ssid[] = "rocotoyz";
const char pass[] = "rocotoyz_13";

//ssid --> rocs13_2.4G 
//password --> RngDQ5Dk   <== wifi credentials

//ssid --> rocotoyz 
//password --> rocotoyz_13 <== mobile data credentials

//Firebase objects
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData; //Define FirebaseESP8266 data object

void connect() {

  Serial.print("CONNECTING TO WIFI PLEASE WAIT.");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  
  Serial.println("\nYOUR RFID SCANNER CONNECTED SUCCESSFULLY");
  Serial.print("ESP Board MAC Address:");
  Serial.println(WiFi.macAddress());
  Serial.print("IP Address: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

void setup() {

  Serial.begin(9600);
  WiFi.begin(ssid, pass);

  lcd.init();  // initialize the lcd
  lcd.clear();
  lcd.backlight();

  SPI.begin();
  rfid.init();

  connect();
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);
  // Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

// push rfidnumber to the database 
void pushUser(String rfidnumber)  //Function to check if an identified tag is registered to allow access
{

  lcd.setCursor(3,0);
  lcd.print("RFID CARD");
  
  lcd.setCursor(3,1);
  lcd.print("REGISTERED!");

  delay(1000);
  lcd.clear();

  Serial.println("RFID CARD ALREADY REGISTERED: " + rfidnumber);
  
  String rfidvalue = "not_owned";

  Firebase.setString(firebaseData, "/ADMIN/RFID_CARDS/" + rfidnumber, rfidvalue);
}

//reads new rfid card and generate random rfidnumber
void loop() {
  if (rfid.findCard(PICC_REQIDL, str) == MI_OK)  //Wait for a tag to be placed near the reader
  {
    Serial.println("RFID CARD DETECTED!");
    String rfidcard = "";             //Temporary variable to store the read RFID number
    if (rfid.anticoll(str) == MI_OK)  //Anti-collision detection, read tag serial number
    {
      Serial.print("The RFID card number is : ");
      for (int i = 0; i < 4; i++)  //Record and display the tag serial number
      {
        rfidcard = rfidcard + (0x0F & (str[i] >> 4));
        rfidcard = rfidcard + (0x0F & str[i]);
      }
      Serial.println(rfidcard);
      
      //function with rfidcard parameter to pass the data to pushUser method
      pushUser(rfidcard);  //Check if the identified tag is an allowed to open tag
    }
    rfid.selectTag(str);  //Lock card to prevent a redundant read, removing the line will make the sketch read cards continually
  }
  rfid.halt();

  lcd.setCursor(1,0);   
  lcd.print("TAP NEW CARD");
  
  lcd.setCursor(2,1);   
  lcd.print("TO REGISTER");
  delay(300);
  lcd.clear();
}
