//This is the code for the vending machine lock system on arduino nano. There are serial write commands intended for serial console debugging.
//This code is for use with the HZ1050 125k RFID reader, it will not work with the more common MFRC522 module for arduino.

#include <SoftwareSerial.h>
const int chipSelect = 4;
//list the ID numbers (decimal) of any card able to open the restocking door. This is the only one currently in use.
long approvedUsers[] = {**Removed for security reasons, copy these from the manual if authorised**}; 
//list the ID numbers (decimal) of any card able to open the maintnence door. This door was never wired up. 
long approvedDevs[] = {**Removed for security reasons, copy these from the manual if authorised**}; 
//one ID card can be on both lists, it just opens the first door, then after 2500 ms, the second door unlocks, if it is hooked up. 

int numApprovedUsers = 18; //must be equal to the length of the approvedUsers array.  C doesn't have a method to retrieve the length of an array.

int numApprovedDevs = 11; //must be equal to the length of the approvedDevs array. If this number is incorrect, it will read data from neigboring data locations (potentially other access arrays) and give errors.

SoftwareSerial inputData(7, 6); //RX, TX we need to put a TX pin to meet the softwareSerial requirments, but it goes unused. HZ1050 TX pin connects to arduino pin 7

const unsigned long TIMEOUT = 300;  // mS

int frontRelay = 9; //pin for the relay that contols the front (restocking) lock
int backRelay = 8; //pin for the relay that controls the back (development) lock.
int openTime = 2000; //the time to hold the lock open for after a sucessful card read in milliseconds.

//other variables for the code, no need to change these ever.
byte count;
byte backCount;

// the most recent card, zero if none
unsigned long cardCode;
boolean isLocked = true;
unsigned long timeLastSerialInput;

//Determines if a card has access approval, unlocks appropriate door. 
void processNewCard ()
 //Begin front door processing
{
  Serial.println(cardCode);
  for(int i=0;i<numApprovedUsers;i++)
 {
    if(approvedUsers[i]==cardCode)
   {
    Serial.println("**Access Granted**");
      Serial.println("**Unlocking Front Door**");
      digitalWrite(frontRelay, HIGH);
      delay(openTime);
      digitalWrite(frontRelay, LOW);
   }else{
    count++; //counts if not matching approvedUsers[i]
   }
  }
   if(count == numApprovedUsers){
   Serial.println("**Access Denied for Front**"); 
   }


  //Begin back door processing
  //Serial.println(cardCode);
  for(int i=0;i<numApprovedDevs;i++)
  {
    if(approvedDevs[i]==cardCode)
   {
    Serial.println("**Access Granted**");
      Serial.println("**Unlocking Back Door**");
      digitalWrite(backRelay, HIGH);
      delay(openTime);
      digitalWrite(backRelay, LOW);
   }else{
    backCount++; //counts if not matching approvedUsers[i]
   }
   }
   if(backCount == numApprovedUsers){
   Serial.println("**Access Denied for Back**"); 
   }
  }  // end of processNewCard

  //generates the cardCode whenever a card is detected.
void processIncomingByte (const byte inByte)
  {
  unsigned long now = millis ();
  
  // if a long time has elapsed, assume new card
  if (now - timeLastSerialInput >= TIMEOUT)
    {
    cardCode = 0;
    count = 0; 
    }  // end of timeout
  
  timeLastSerialInput = now;
  cardCode <<= 8;
  cardCode |= inByte;
  count++;
  
  if (count >= 4)
    {
    processNewCard ();
    cardCode = 0;
    count = 0; 
    }  // end of having 4 incoming bytes
   
  }  // end of processIncomingByte

  
 //initialises variables and ports
void setup()
  {
  inputData.begin (19200);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(frontRelay, OUTPUT);
  pinMode(backRelay, OUTPUT);
  // end of setup
}

//basically executes the other methods in the code when a card enters the reader's range.
void loop()
{
  // if serial data available, process it
  while (inputData.available () > 0)
    processIncomingByte (inputData.read ());
}  // end of loop
