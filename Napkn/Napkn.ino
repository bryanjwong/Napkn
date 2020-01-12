
/*---WiFi---*/
// WiFi Libraries
#include <WiFi.h>                                                   // esp32 library
#include <IOXhop_FirebaseESP32.h>                                   // firebase library
#define FIREBASE_HOST "napkn-test-d7a85.firebaseio.com"             // the project name address from firebase id
#define FIREBASE_AUTH "o1DICNIM0C1pJboj0sFu0VH5mMZ4U9zQZwq8M0jq"    // the secret key generated from firebase
//#define WIFI_SSID "AndroidAP"                                       // input your home or public wifi name
//#define WIFI_PASSWORD "wxai8878"                                    // password of wifi ssid
#define WIFI_SSID "UCLA_WEB"                                        // input your home or public wifi name
#define WIFI_PASSWORD ""

// Unique Device ID
const String DEVICE_ID = "Napkn1";                                  // Format: Napkn[X]

// Database Paths
const String deviceStatus_path = "Napkn/" + DEVICE_ID + "/DeviceStatus";
const String recentUserRFID_path = "Napkn/" + DEVICE_ID + "/RecentUserID";
const String timeRemaining_path = "Napkn/" + DEVICE_ID + "/TimeRemaining";
const String tablesOpen_path = "Napkn/Values/TablesOpen";
/*----------*/

/*---RFID---*/
// RFID Reader Libraries
#include <SPI.h>
#include <MFRC522.h>

#define IS_BRUINCARD(uid_size) (uid_size == 7)
#define IS_GUESTCARD(rfid) (rfid == "09 5D 5E 63") 
#define IS_EASTEREGG(rfid) (rfid == "04 8F 42 0A EB 62 80")

#define SS_PIN 25
#define RST_PIN 32
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
/*----------*/

/*---OLED Display---*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_ADDR       0x3C
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char szString[20];
byte mins, secs;
/*------------------*/

/*---Speaker---*/
#include <pitches.h>
#define SPEAKER 26 

const int CHANNEL = 0;
const int RESOLUTION = 8;
int NOTE_DUR = 78;

// Play a Note
void play(int note, int dur) {
    ledcWriteTone(CHANNEL, note);
    delay(dur * NOTE_DUR);
    ledcWriteTone(CHANNEL, 0);
    delay(dur * NOTE_DUR / 3);
}

void checkinJingle() {
  play(NOTE_G5, 1);
  play(NOTE_E5, 1);
  play(NOTE_G5, 1);
  play(NOTE_C6, 1);
}

void checkoutJingle() {
  play(NOTE_G5, 1);
  play(NOTE_E5, 1);
  play(NOTE_G5, 1);
  play(NOTE_C5, 1);
}

void badJingle() {
  play(NOTE_F5, 2);
  play(NOTE_B4, 4);
}
/*-------------*/

/*---Napkn Globals---*/
#define GREEN_LED 12
#define RED_LED 27

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
  String content = "";
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
  if (secondsLeft == 60) {
    minutesLeft++;
    secondsLeft = 0;
  }
  return String(minutesLeft) + ':' + String(secondsLeft);
}

// Write the time remaining and who is currently reserving the table to the OLED display
void displayTimeRemaining() {

  int minutesLeft = reserveTimeLength - minutesElapsed - 1;
  int secondsLeft = 60 - (secondsElapsed % 60);
  if (secondsLeft == 60) {
    minutesLeft++;
    secondsLeft = 0;
  }
  sprintf( szString, " %02d:%02d", minutesLeft, secondsLeft );
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(" Reserved");
  display.print(" by ");
  display.println(recentUserName);
  display.println("");
  display.print("  ");
  display.print(szString);
  display.display();
}
/*----------------------*/

void setup() {
  Serial.begin(115200);  
  delay(1000);

  Serial.print("Initialized ");
  Serial.println(DEVICE_ID);
  Serial.println("------------------");
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

  // Sync Local Variables with Database
  deviceStatus = Firebase.getString(deviceStatus_path);
  recentUserRFID = Firebase.getString(recentUserRFID_path);
  recentUserName = Firebase.getString("Users/" + recentUserRFID + "/Name");
  reserveTimeLength = Firebase.getInt(timeRemaining_path);

  // Initialize LED
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  if (deviceStatus == "Open") {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
  } else {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
  }

  // Initialize Speaker
  ledcSetup(CHANNEL, 2000, RESOLUTION);
  ledcAttachPin(SPEAKER, CHANNEL);
  ledcWrite(CHANNEL, 255);
  ledcWriteTone(CHANNEL, 2000);
  checkinJingle();
  
  // Initialize OLED Display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  if(deviceStatus == "Open") {
      display.clearDisplay();
      display.setCursor(3, 10);
      display.println("This Napkn");
      display.println(" is open! ");
      display.display();
  } 
  
  // Initialize RFID Reader
  SPI.begin();      // Initiate  SPI bus
  Serial.println("Initiating MFRC522");
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  // Reset Timings
  checkInTime = millis();
  secondsElapsed = 0;
  minutesElapsed = 0;
}

void loop() {

  if (deviceStatus == "Occupied") {
    int currentTime = millis();
    int elapsedTime =  currentTime - checkInTime;
    if (elapsedTime >= reserveTimeLength * 60000) {
      deviceStatus = "Open";
      digitalWrite(RED_LED, LOW);       // Turn GREEN LED on
      digitalWrite(GREEN_LED, HIGH);
      
      Serial.print(recentUserName);
      Serial.println(" has been checked out of this table");
      
      display.clearDisplay();
      display.setCursor(0, 10);
      display.print("  ");
      display.println(recentUserName);
      display.println("timed out!");
      display.display();
      // Play checkout music
      checkoutJingle();

      Firebase.setString(deviceStatus_path, "Open");
      String recentUserRFID_tables_path = "Users/" + recentUserRFID + "/Tables";
      Firebase.setInt(recentUserRFID_tables_path, Firebase.getInt(recentUserRFID_tables_path) - 1);

      display.clearDisplay();
      display.setCursor(3, 10);
      display.println("This Napkn");
      display.println(" is open! ");
      display.display();
    }
    else if (elapsedTime - 1000 * secondsElapsed >= 1000) {
      secondsElapsed += (elapsedTime - 1000 * secondsElapsed) / 1000;
      Serial.println(countdownTime());
      display.clearDisplay();
      displayTimeRemaining();
      if (secondsElapsed - 60 * minutesElapsed >= 60) {
        minutesElapsed++;
        Firebase.setInt(timeRemaining_path, reserveTimeLength - minutesElapsed);
      }
    }
  }
  
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
  {
    String scannedID = processRFID();
    Serial.print("Read RFID: ");
    Serial.println(scannedID);
    if(IS_EASTEREGG(scannedID)) {
       easterEgg();
       return;
    }
    if (!IS_GUESTCARD(scannedID) && !IS_BRUINCARD(mfrc522.uid.size)) {      // Check for invalid cards
      Serial.println("Please enter a valid BruinCard.");
      
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("   Not    ");
      display.println(" a valid  ");
      display.println("BruinCard.");
      display.display();
      
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      badJingle();
      
      if (deviceStatus == "Open") {
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        display.clearDisplay();
        display.setCursor(3, 10);
        display.println("This Napkn");
        display.println(" is open! ");
        display.display();
      }
      return;
    }
    play(NOTE_C6, 1);

    // Generate user information database paths using Scanned ID
    String scannedID_name_path = "Users/" + scannedID + "/Name";
    //String scannedID_UID_path = "Users/" + scannedID + "/UID";
    String scannedID_tables_path = "Users/" + scannedID + "/Tables";

    if (deviceStatus == "Occupied") {
      if (scannedID == recentUserRFID) {      // User is checking out
        digitalWrite(RED_LED, LOW);         // Turn GREEN LED on
        digitalWrite(GREEN_LED, HIGH);

        Serial.print(recentUserName);
        Serial.println(" has been checked out of this table");
        display.clearDisplay();
        display.setCursor(0, 10);
        display.println("Thanks for");
        display.println("  using   ");
        display.println("  Napkn!  ");
        display.display();
        checkoutJingle();
        delay(2500);
        
        deviceStatus = "Open";
        display.clearDisplay();
        display.setCursor(3, 10);
        display.println("This Napkn");
        display.println(" is open! ");
        display.display();
        Firebase.setString(deviceStatus_path, "Open");
        int scannedID_tables = Firebase.getInt(scannedID_tables_path);
        Firebase.setInt(scannedID_tables_path, scannedID_tables - 1);
      }
      else {                                // User is attempting to check out a table that is already reserved.
        Serial.print("Sorry! This table is currently being reserved by ");
        Serial.println(recentUserName);

        display.clearDisplay();
        display.setCursor(0, 10);
        display.println("This Napkn");
        display.println("    is    ");
        display.println(" reserved.");
        display.display();
        badJingle();
      }
    } else if (deviceStatus == "Open") {
      int maxTables;
      int maxConsecutive;
      int scannedID_tables;
      if (scannedID == recentUserRFID && ((maxConsecutive = Firebase.getInt("Values/MaxConsecutive")) <= consecutiveCheckIns)) { // User tried to check out table multiple times in a row
        Serial.print("Sorry! You've checked out this table too many times! Max is: ");
        Serial.print(maxConsecutive);
        Serial.println(" times.");

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println(" Checked  ");
        display.println("this table");
        display.println(" out too  ");
        display.println("many times");
        display.display();
        
        digitalWrite(RED_LED, HIGH);        // Turn RED LED on
        digitalWrite(GREEN_LED, LOW);
        badJingle();

        display.clearDisplay();
        display.setCursor(3, 10);
        display.println("This Napkn");
        display.println(" is open! ");
        display.display();
        
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
      }
      else if ((maxTables = Firebase.getInt("Values/MaxTables")) <= (scannedID_tables = Firebase.getInt(scannedID_tables_path))) {
        Serial.print("Sorry! You've checked out too many tables at once! Max is: ");
        Serial.print(maxTables);
        Serial.println(" tables.");

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println(" Checked  ");
        display.println(" out too  ");
        display.println("   many   ");
        display.println("  tables! ");
        display.display();
        
        digitalWrite(RED_LED, HIGH);          // Turn RED LED On
        digitalWrite(GREEN_LED, LOW);
        badJingle();

        display.clearDisplay();
        display.setCursor(3, 10);
        display.println("This Napkn");
        display.println(" is open! ");
        display.display();
        
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
      } else {
        digitalWrite(RED_LED, HIGH);      // Turn RED On
        digitalWrite(GREEN_LED, LOW);
        checkinJingle();
        
        if (scannedID == recentUserRFID)
          consecutiveCheckIns++;
        else {
          consecutiveCheckIns = 1;
          recentUserRFID = scannedID;
          Firebase.setString(recentUserRFID_path, scannedID);
          recentUserName = Firebase.getString(scannedID_name_path);
        }
        Serial.print(recentUserName);
        Serial.println(" just checked in to this table!");

        display.clearDisplay();
        display.setCursor(0, 10);
        display.println("Thanks for checking");
        display.print("in, ");
        display.print(recentUserName);
        display.println("!");
        display.display();

        reserveTimeLength = Firebase.getInt("Values/CheckoutTimeLength");
        Firebase.setString(deviceStatus_path, "Occupied");
        Firebase.setInt(scannedID_tables_path, scannedID_tables + 1);
        Firebase.setInt(timeRemaining_path, reserveTimeLength);

        deviceStatus = "Occupied";
        checkInTime = millis();
        secondsElapsed = 0;
        minutesElapsed = 0;
      }
    }
  }
}

/*---Easter Egg---*/
void easterEgg() {
    NOTE_DUR = 58;  // Set tempo to be faster
    play(NOTE_D4, 1);
    play(NOTE_FS4, 1);
    play(NOTE_B4, 1);
    play(NOTE_D5, 1);
    play(NOTE_CS5, 1);
    play(NOTE_B4, 1);
    play(NOTE_AS4, 1);
    play(NOTE_B4, 16); //m2
    delay(NOTE_DUR*6);
    play(NOTE_B4, 6);
    play(NOTE_CS5, 6);
    play(NOTE_D5, 6);
    play(NOTE_E5, 6); //m3
    play(NOTE_FS5, 7);
    play(NOTE_D5, 7);
    play(NOTE_FS5, 8);
    play(NOTE_D5, 8);
    play(NOTE_FS5, 22);
    delay(NOTE_DUR*4);
    play(NOTE_FS4, 2); //m4
    play(NOTE_D5, 6);
    play(NOTE_E4, 2);
    play(NOTE_CS5, 6);
    play(NOTE_D4, 2);
    play(NOTE_B4, 4); //m5
    play(NOTE_D4, 2);
    play(NOTE_FS4, 4);
    play(NOTE_B4, 2);
    play(NOTE_D4, 4);
    play(NOTE_B4, 4);
    play(NOTE_D4, 2);
    play(NOTE_FS4, 2);
    play(NOTE_D5, 2);
    play(NOTE_CS5, 2);
    play(NOTE_B4, 2);
    play(NOTE_A4, 2);
    play(NOTE_B4, 4); //m6
    play(NOTE_D4, 2);
    play(NOTE_G4, 4);
    play(NOTE_B4, 2);
    play(NOTE_D4, 4);
    play(NOTE_B4, 4);
    play(NOTE_D4, 2);
    play(NOTE_G4, 2);
    play(NOTE_D5, 2);
    play(NOTE_CS5, 2);
    play(NOTE_B4, 2);
    play(NOTE_A4, 2);
    play(NOTE_D5, 4); //m7
    play(NOTE_FS4, 2);
    play(NOTE_A4, 4);
    play(NOTE_D5, 2);
    play(NOTE_FS4, 4);
    play(NOTE_D5, 4);
    play(NOTE_FS4, 2);
    play(NOTE_A4, 2);
    play(NOTE_D5, 2);
    play(NOTE_CS5, 2);
    play(NOTE_B4, 2);
    play(NOTE_A4, 2);
    play(NOTE_E5, 12); //m8
    delay(NOTE_DUR*2);
    play(NOTE_FS4, 2);
    play(NOTE_D5, 6);
    play(NOTE_E4, 2);
    play(NOTE_CS5, 6);
    play(NOTE_D4, 2);
    play(NOTE_B4, 10); //m9
}
/*----------------*/
