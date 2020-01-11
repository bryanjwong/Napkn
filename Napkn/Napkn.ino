#include <WiFi.h>                                                   // esp32 library
#include <IOXhop_FirebaseESP32.h>                                   // firebase library
#define FIREBASE_HOST "napkn-test-d7a85.firebaseio.com"             // the project name address from firebase id
#define FIREBASE_AUTH "o1DICNIM0C1pJboj0sFu0VH5mMZ4U9zQZwq8M0jq"    // the secret key generated from firebase
#define WIFI_SSID "AndroidAP"                                       // input your home or public wifi name
#define WIFI_PASSWORD "wxai8878"                                    // password of wifi ssid

// Unique Device ID
const String DEVICE_ID = "Napkn1";

// Database Paths
const String deviceStatus_path = DEVICE_ID + "/DeviceStatus";
const String recentUserID_path = DEVICE_ID + "/RecentUserID";
const String timeLimit_path = DEVICE_ID + "/TimeLimit";

String fireStatus = "";                                                     // led status received from firebase
int led = 2;

void setup() {

  Serial.begin(9600);
  delay(1000);
  pinMode(2, OUTPUT);
  
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
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                                       // connect to firebase
  Firebase.setString("LED_STATUS", "OFF");                                          //send initial string of led status
}

void loop() {


    
    String deviceStatus = Firebase.getString(deviceStatus_path); 
    String recentUserID = Firebase.getString(recentUserID_path);
    String timeLimit = Firebase.getString(timeLimit_path);
    Serial.print("Device Status: ");
    Serial.println(deviceStatus);
//  fireStatus = Firebase.getString("LED_STATUS");                     // get led status input from firebase
//  Firebase.setString("LED_STATUS", "Hello");

//  if (fireStatus == "ON") {                         // compare the input of led status received from firebase
//    Serial.println("Led Turned ON");
//    digitalWrite(2, HIGH);                                                         // make output led ON
//  }
//
//  else if (fireStatus == "OFF") {              // compare the input of led status received from firebase
//    Serial.println("Led Turned OFF");
//    digitalWrite(2, LOW);                                                         // make output led OFF
//  }
//
//  else {
//    Serial.println("Wrong Credential! Please send ON/OFF");
//  }

}
