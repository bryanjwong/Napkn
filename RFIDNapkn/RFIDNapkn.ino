#include"rfid1.h"

#define IS_BRUINCARD(card_type) (card_type[0]==0x44 && card_type[1]==0x03)
#define TIME_LIMIT 30000  // 30 seconds

bool currently_reserved;
unsigned long start_time;
int current_id;

RFID1 rfid;

unsigned char serNum[5];  // array to store your ID

void setup() {
  Serial.begin(9600);
  currently_reserved = false;
  current_id = -1;
  rfid.begin(7, 5, 4, 3, 6, 2);  // params are pin numbers
  delay(100);
  rfid.init();
}

void loop() {
  unsigned char status;
  unsigned char str[MAX_LEN];
  status = rfid.request(PICC_REQIDL, str);

  //check if currently reserved and if time is up
  if(currently_reserved && millis() - start_time >= TIME_LIMIT) { // if time is up, unreserve table
    currently_reserved = false;
  }
  if (status != MI_OK) {
    return;
  }

  if (IS_BRUINCARD(str)) {
    Serial.println("This is a bruincard.");

    status = rfid.anticoll(str);  // determine status and card ID
    
    if (status == MI_OK) {  // check if status is good
        Serial.println("Status is good.");

         memcpy(serNum, str, 5);
       
         // convert serNum to a datatype we can use
        unsigned int intId;
        memcpy(&intId, str, 5);
       
        if (currently_reserved == true){
          if (current_id == intId){
            currently_reserved = false; //unreserve table 
          }
        } else {
          // if name is not in UserStatus
            currently_reserved = true;
            current_id = intId;
            // create new Napkn node w/ DeviceStatus, RecentUserID, TimeLimit
            // push to firebase
            start_time = millis(); //push time?
        }
    }
  }

  
  


  delay(500);
  
  rfid.halt(); //command the card into sleep mode  
}
