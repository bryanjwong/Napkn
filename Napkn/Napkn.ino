
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

#define IS_BRUINCARD(uid_size) (uid_size == 7)

#define SS_PIN 25
#define RST_PIN 26
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
/*----------*/

/*---Napkn Globals---*/
#define GREEN_LED 13 
#define RED_LED 12   

unsigned long checkInTime = 0;
unsigned long secondsElapsed = 0;
unsigned long minutesElapsed = 0;

String deviceStatus = "";
String recentUserRFID = "";
String recentUserName = "";
unsigned int consecutiveCheckIns = 0;
unsigned int reserveTimeLength = 0;
/*-------------------*/

/*---Helper Functions---*/
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

// Generate count down time in HH:MM format
String countdownTime() {
    int minutesLeft = reserveTimeLength - minutesElapsed - 1;
    int secondsLeft = 60 - (secondsElapsed % 60);
    if(secondsLeft == 60) {
      minutesLeft++;
      secondsLeft = 0;
    }
    return String(minutesLeft) + ':' + String(secondsLeft);
}
/*----------------------*/

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
  
    // Sync Local Variables with Database
    deviceStatus = Firebase.getString(deviceStatus_path);
    recentUserRFID = Firebase.getString(recentUserRFID_path);
    recentUserName = Firebase.getString("Users/" + recentUserRFID + "/Name");
    reserveTimeLength = Firebase.getInt(timeRemaining_path);
  
    // Initialize Display Components
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    if(deviceStatus == "Open") {
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
    } else {
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
    }
  
    checkInTime = millis();
    secondsElapsed = 0;
    minutesElapsed = 0;
}

void loop() {
  
  if(deviceStatus == "Occupied") {
      int currentTime = millis();
      int elapsedTime =  currentTime - checkInTime;
      if(elapsedTime >= reserveTimeLength * 60000) {
        // checkout from table


        deviceStatus = "Open";
        digitalWrite(RED_LED, LOW);       // Turn GREEN LED on
        digitalWrite(GREEN_LED, HIGH);
        Firebase.setString(deviceStatus_path, "Open");
        String recentUserRFID_tables_path = "Users/" + recentUserRFID + "/Tables";
        Firebase.setInt(recentUserRFID_tables_path, Firebase.getInt(recentUserRFID_tables_path)-1);
        Serial.print(recentUserName);
        Serial.println(" has been checked out of this table");
        // Play checkout music
        Serial.println("Delaying 1.5s...");
        delay(1500);
        Serial.println("Delay complete.");
      } 
      else if (elapsedTime - 1000 * secondsElapsed >= 1000) {
        secondsElapsed += (elapsedTime - 1000 * secondsElapsed)/1000; 
        Serial.println(countdownTime());
        // update display
        if(secondsElapsed - 60 * minutesElapsed >= 60) {
          minutesElapsed++;
          Firebase.setInt(timeRemaining_path, reserveTimeLength - minutesElapsed);
        }
      }
    }
    
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) 
  {
      if (!IS_BRUINCARD(mfrc522.uid.size)) {      // Check for invalid cards
          Serial.println("Please enter a valid BruinCard.");
          digitalWrite(RED_LED, HIGH);
          digitalWrite(GREEN_LED, LOW);
          
          // Play card deny music
          Serial.println("Delaying 1.5s...");
          delay(1500);
          Serial.println("Delay complete.");
          if(deviceStatus == "Open") {
            digitalWrite(RED_LED, LOW);
            digitalWrite(GREEN_LED, HIGH);
          }
          return;
      }
      
      String scannedID = processRFID();
      Serial.print("Read RFID: ");
      Serial.println(scannedID);

      // Generate user information database paths using Scanned ID
      String scannedID_name_path = "Users/" + scannedID + "/Name";
      //String scannedID_UID_path = "Users/" + scannedID + "/UID";
      String scannedID_tables_path = "Users/" + scannedID + "/Tables";

      if (deviceStatus == "Occupied") {
          if(scannedID == recentUserRFID) {       // User is checking out
              digitalWrite(RED_LED, LOW);         // Turn GREEN LED on
              digitalWrite(GREEN_LED, HIGH);
              // Play checkout music
              Serial.println("Delaying 1.5s...");
              delay(1500);
              Serial.println("Delay complete.");
              
              Serial.print(recentUserName);
              Serial.println(" has been checked out of this table");
              deviceStatus = "Open";
              Firebase.setString(deviceStatus_path, "Open");
              int scannedID_tables = Firebase.getInt(scannedID_tables_path);
              Firebase.setInt(scannedID_tables_path, scannedID_tables-1);  
          }
          else {                                // User is attempting to check out a table that is already reserved.
              Serial.print("Sorry! This table is currently being reserved by ");
              Serial.println(recentUserName);
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
              digitalWrite(RED_LED, HIGH);        // Turn RED LED on
              digitalWrite(GREEN_LED, LOW);
              // Play angry music
              Serial.println("Delaying 1.5s...");
              delay(1500);
              Serial.println("Delay complete.");
              digitalWrite(RED_LED, LOW);
              digitalWrite(GREEN_LED, HIGH);
          } 
          else if((maxTables = Firebase.getInt("Values/MaxTables")) <= (scannedID_tables = Firebase.getInt(scannedID_tables_path))) {
              Serial.print("Sorry! You've checked out too many tables at once! Max is: ");
              Serial.print(maxTables);
              Serial.println(" tables.");
              digitalWrite(RED_LED, HIGH);          // Turn RED LED On
              digitalWrite(GREEN_LED, LOW);
              // Play angry music
              Serial.println("Delaying 1.5s...");
              delay(1500);
              Serial.println("Delay complete.");
              digitalWrite(RED_LED, LOW);
              digitalWrite(GREEN_LED, HIGH);
          } else {
              digitalWrite(RED_LED, HIGH);      // Turn RED On
              digitalWrite(GREEN_LED, LOW);
              // Play happy music
              Serial.println("Delaying 1.5s...");
              delay(1500);
              Serial.println("Delay complete.");
              
              if(scannedID == recentUserRFID)
                  consecutiveCheckIns++;
              else {
                  consecutiveCheckIns = 1;
                  recentUserRFID = scannedID;
                  Firebase.setString(recentUserRFID_path, scannedID);
                  recentUserName = Firebase.getString(scannedID_name_path);
              }
              Serial.print(recentUserName);
              Serial.println(" just checked in to this table!");
              
              reserveTimeLength = Firebase.getInt("Values/CheckoutTimeLength");
              Firebase.setString(deviceStatus_path, "Occupied");
              Firebase.setInt(scannedID_tables_path, scannedID_tables+1);
              Firebase.setInt(timeRemaining_path, reserveTimeLength);

              deviceStatus = "Occupied";
              checkInTime = millis();
              secondsElapsed = 0;
              minutesElapsed = 0;
          }  
      }
  }

}
