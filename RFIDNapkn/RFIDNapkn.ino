#include"rfid1.h"

#define IS_BRUINCARD(card_type) (card_type[0]==0x44 && card_type[1]==0x03)
#define TIME_LIMIT 30000  // 30 seconds

bool currently_reserved;
unsigned long start_time;

RFID1 rfid;

uchar serNum[5];  // array to store your ID

void setup() {
  Serial.begin(9600);
  currently_reserved = false;
  rfid.begin(7, 5, 4, 3, 6, 2);  // params are pin numbers
  delay(100);
  rfid.init();
}

void loop() {
  uchar status;
  uchar str[MAX_LEN];
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

        // if table is currently reserved:
          // if id is the same as id used to reserve, unreserve table
        // else:
          // if id is not already in database:

            // reserve table, save id in global variable, save id, status to database, set start time
    }
  }

  
  


  delay(500);
  
  rfid.halt(); //command the card into sleep mode  
}
