
/*---WiFi---*/
// WiFi Libraries
#include <WiFi.h>                                                   // esp32 library
#include <IOXhop_FirebaseESP32.h>                                   // firebase library
#define FIREBASE_HOST "napkn-test-d7a85.firebaseio.com"             // the project name address from firebase id
#define FIREBASE_AUTH "o1DICNIM0C1pJboj0sFu0VH5mMZ4U9zQZwq8M0jq"    // the secret key generated from firebase
#define WIFI_SSID "AndroidAP"                                       // input your home or public wifi name
#define WIFI_PASSWORD "wxai8878"                                    // password of wifi ssid

// Unique Device ID
const String DEVICE_ID = "Napkn1";

// Database Paths
const String deviceStatus_path = "Napkn/" + DEVICE_ID + "/DeviceStatus";
const String recentUserID_path = "Napkn/" + DEVICE_ID + "/RecentUserID";
const String timeLimit_path = "Napkn/" + DEVICE_ID + "/TimeRemaining";
/*----------*/

/*---RFID---*/
// RFID Reader Libraries
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 25
#define RST_PIN 26
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
/*----------*/

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                      //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                                      //print local IP address
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                                       // connect to firebase

  // Initialize RFID Reader
  SPI.begin();      // Initiate  SPI bus
  Serial.println("Initiating MFRC522");
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}

void loop() {



    // Look for new cards
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) 
    {
        String deviceStatus = Firebase.getString(deviceStatus_path); 
//        String recentUserID = Firebase.getString(recentUserID_path);
//        String timeLimit = Firebase.getString(timeLimit_path);
        Serial.print("Device Status: ");
        Serial.println(deviceStatus);
    
        //Show UID on serial monitor
        Serial.print("UID tag :");
        String content= "";
        byte letter;
        for (byte i = 0; i < mfrc522.uid.size; i++) 
        {
           Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
           Serial.print(mfrc522.uid.uidByte[i], HEX);
           content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
           content.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        Serial.println();
        Serial.print("Message : ");
        content.toUpperCase();
        String scannedID = content.substring(1);

//        // Reap user information from database
//        String scannedID_name_path = "Users/" + scannedID + "/Name";
//        String scannedID_UID_path = "Users/" + scannedID + "/UID";
//        String scannedID_tables_path = "Users/" + scannedID + "/Tables";
//
//        String scannedID_name = Firebase.getString(scannedID_name_path);
//        String scannedID_UID = Firebase.getString(scannedID_UID_path);
//        int scannedID_tables = Firebase.getInt(scannedID_tables_path);
//
//        Serial.print("Scanned ID Name: ");
//        Serial.println(scannedID_name);
//        Serial.print("Scanned ID UID: ");
//        Serial.println(scannedID_UID);
//        Serial.print("Scanned ID Tables: ");
//        Serial.println(scannedID_tables);
//        
//        if (deviceStatus == "Occupied") {
//            if(scannedID == recentUserID) {
//                Serial.print(scannedID_name);
//                Serial.println(" has been checked out of this table");
//                Firebase.setString(deviceStatus_path, "Open");
//                Firebase.setInt(scannedID_tables_path, scannedID_tables-1);
//            }
//            else {
//                String recentUserID_name_path = "Users/" + recentUserID + "/Name";
//                String recentUserID_name = Firebase.getString(recentUserID_name_path);
//                Serial.print("Sorry! This table is currently being reserved by ");
//                Serial.println(recentUserID_name);
//            }
//        } else {
//            // if user has consecutive checkins 
//                // deny
//            // else
//                // if user has too many tables
//                    // deny
//                // else
//                    // checkin
//            Firebase.setString(deviceStatus_path, "Occupied");
//            Firebase.setString(recentUserID_path, scannedID);
//            Firebase.setInt(scannedID_tables_path, scannedID_tables+1);
//            Serial.print(scannedID_name);
//            Serial.println(" has just checked out this table!");
//        }
      
    }
  



}
