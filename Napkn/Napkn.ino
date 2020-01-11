
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
const String recentUserRFID_path = "Napkn/" + DEVICE_ID + "/RecentUserID";
const String timeRemaining_path = "Napkn/" + DEVICE_ID + "/TimeRemaining";
/*----------*/

/*---RFID---*/
// RFID Reader Libraries
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 25
#define RST_PIN 26
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
/*----------*/

/*---Napkn Locals---*/
unsigned long checkInTime = 0;
unsigned long secondsElapsed = 0;
unsigned long minutesElapsed = 0;

String deviceStatus = "Open";
String recentUserRFID = "";
String recentUserName = "";
unsigned int consecutiveCheckIns = 0;
unsigned int reserveTimeLength = 0;
/*------------------*/

// Convert RFID into String
String processRFID() {
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    return content.substring(1);
}

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
        String scannedID = processRFID();
        Serial.print("Read RFID: ");
        Serial.println(scannedID);

        // Generate user information database paths using Scanned ID
        String scannedID_name_path = "Users/" + scannedID + "/Name";
        //String scannedID_UID_path = "Users/" + scannedID + "/UID";
        String scannedID_tables_path = "Users/" + scannedID + "/Tables";

        if (deviceStatus == "Occupied") {
            if(scannedID == recentUserRFID) {       // User is checking out
                Serial.print(recentUserName);
                Serial.println(" has been checked out of this table");
                deviceStatus = "Open";
                Firebase.setString(deviceStatus_path, "Open");
                int scannedID_tables = Firebase.getInt(scannedID_tables_path);
                Firebase.setInt(scannedID_tables_path, scannedID_tables-1);
                // Play checkout music
                Serial.println("Delaying 1.5s...");
                delay(1500);
                Serial.println("Delay complete.");
            }
            else {                                // User is attempting to check out a table that is already reserved.
                String recentUserRFID_name_path = "Users/" + recentUserRFID + "/Name";
                String recentUserRFID_name = Firebase.getString(recentUserRFID_name_path);
                Serial.print("Sorry! This table is currently being reserved by ");
                Serial.println(recentUserRFID_name);
                // Play angry music
                Serial.println("Delaying 1.5s...");
                delay(1500);
                Serial.println("Delay complete.");
            }
        } else if (deviceStatus == "Open") {
            int maxTables;
            int maxConsecutive;
            int scannedID_tables;
            if(scannedID == recentUserRFID && ((maxConsecutive = Firebase.getInt("Values/MaxConsecutive")) <= consecutiveCheckIns)) {  // User tried to check out table multiple times in a row
                Serial.print("Sorry! You've checked out this table too many times! Max is: ");
                Serial.print(maxConsecutive);
                Serial.println(" times.");
                // Play angry music
                Serial.println("Delaying 1.5s...");
                delay(1500);
                Serial.println("Delay complete.");
            } 
            else if((maxTables = Firebase.getInt("Values/MaxTables")) <= (scannedID_tables = Firebase.getInt(scannedID_tables_path))) {
                Serial.print("Sorry! You've checked out too many tables at once! Max is: ");
                Serial.print(maxTables);
                Serial.println(" tables.");
                // Play angry music
                Serial.println("Delaying 1.5s...");
                delay(1500);
                Serial.println("Delay complete.");
            } else {
                if(scannedID == recentUserRFID)
                    consecutiveCheckIns++;
                else {
                    consecutiveCheckIns = 1;
                    recentUserRFID = scannedID;
                    Firebase.setString(recentUserRFID_path, scannedID);
                    recentUserName = Firebase.getString(scannedID_name_path);
                }
                deviceStatus = "Occupied";
                checkInTime = millis();
                secondsElapsed = 0;
                minutesElapsed = 0;
                reserveTimeLength = Firebase.getInt("Values/CheckoutTimeLength");
                Firebase.setString(deviceStatus_path, "Occupied");
                Firebase.setInt(scannedID_tables_path, scannedID_tables+1);
                Serial.print(recentUserName);
                Serial.println(" just checked in this table!");
                // Play happy music
                Serial.println("Delaying 1.5s...");
                delay(1500);
                Serial.println("Delay complete.");
            }  
        }
    }
  



}
