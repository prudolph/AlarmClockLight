/* 
 * TimeSerial.pde
 * example code illustrating Time library set through serial port messages.
 *
 * Messages consist of the letter T followed by ten digit time (as seconds since Jan 1 1970)
 * you can send the text on the next line using Serial Monitor to set the clock to noon Jan 1 2010
 T1262347200  
 *
 * A Processing example sketch to automatically send the messages is inclided in the download
 */ 
 
#include <Time.h>  
#include <TimeAlarms.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include "Adafruit_BLE_UART.h"



float lightBrightness = 0;
int lightBrightnessInc = 1;
int lightBrightnessIncTimeMillis =8000 ;
float maxBrightness = 250;

bool alarmTriggered  = false;
double increaseBrightnessTimestamp=0;


//LED STRIP
int ledPin = 8; // Z pin connected to Analog 5
int ledCount = 22;

 Adafruit_NeoPixel strip = Adafruit_NeoPixel(ledCount, ledPin, NEO_GRB + NEO_KHZ800);


///////////////BLUETOOTH
// Connect CLK/MISO/MOSI to hardware SPI
// e.g. On UNO & compatible: CLK = 13, MISO = 12, MOSI = 11
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2     // This should be an interrupt pin, on Uno thats #2 or #3
#define ADAFRUITBLE_RST 9

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);



void setup()  {

  Serial.begin(9600);
  //while(!Serial);
   setTime(22,00,00,02,10,2014); 
   
  Alarm.alarmRepeat(06,30,0, LightsOnAlarm);  // 8:30am every day
  Alarm.alarmRepeat(07,15,00, LightsOffAlarm);  // 8:30am every day
  
 strip.begin();
 strip.setBrightness(0);
 strip.show(); // Initialize all pixels to 'off'
 
 //Initalize Bluth
  BTLEserial.setDeviceName("Window"); /* 7 characters max! */
  BTLEserial.begin();
 }
 
 /**************************************************************************/
/*!
    Constantly checks for new events on the nRF8001
*/
/**************************************************************************/
aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop(){    
 // Tell the nRF8001 to do whatever it should be working on.
  BTLEserial.pollACI();

 
   // Ask what is our current status
  aci_evt_opcode_t status = BTLEserial.getState();
  // If the status changed....
  if (status != laststatus) {
    // print it out!
    if (status == ACI_EVT_DEVICE_STARTED) {
        Serial.println(F("* Advertising started"));
    }
    if (status == ACI_EVT_CONNECTED) {
        Serial.println(F("* Connected!"));
    }
    if (status == ACI_EVT_DISCONNECTED) {
        Serial.println(F("* Disconnected or advertising timed out"));
    }
    // OK set the last status change to this one
    laststatus = status;
  }
 
 
 
  if (status == ACI_EVT_CONNECTED) {
    // Lets see if there's any data for us!
    if (BTLEserial.available()) {
      Serial.print("* "); Serial.print(BTLEserial.available()); Serial.println(F(" bytes available from BTLE"));
    }
    // OK while we still have something to read, get a character and print it out
   String message = "";
    while (BTLEserial.available()) {
      char c = BTLEserial.read();
      message+=c;
      Serial.print(c);
    }
  parseMessage(message);
  
  
  
  if(timeStatus()!= timeNotSet)   
  {
    digitalWrite(13,timeStatus() == timeSet); // on if synced, off if needs refresh  
    digitalClockDisplay();  
  }
  
  //Check if we should increase brightness
  if(alarmTriggered){
 
    if(millis()-increaseBrightnessTimestamp>lightBrightnessIncTimeMillis){
      increaseBrightnessTimestamp=millis();
        lightBrightness+=lightBrightnessInc;
        if(lightBrightness> maxBrightness) lightBrightness=maxBrightness;
        //currentLed++;
       // if(currentLed>ledCount)currentLed=0;
      //  strip.setPixelColor(currentLed,0, 245, 88);
          
    }
    Serial.print("Brightness: ");
    Serial.println(lightBrightness);
 
 
}
 strip.setBrightness((int)lightBrightness);
           for(int i =0;i<ledCount;i++) strip.setPixelColor(i, 255, 211, 50);
 
     
   strip.show(); 
     Alarm.delay(1000); 


}}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hourFormat12());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
Serial.print(isAM()?"AM":"PM");
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

// functions to be called when an alarm triggers:
void LightsOnAlarm(){
  Serial.println("Alarm: - turn lights ON"); 
  increaseBrightnessTimestamp = millis();  
  alarmTriggered=true;
}

void LightsOffAlarm(){
  Serial.println("Alarm: - turn lights OFF");    
  alarmTriggered=false;
  lightBrightness=0;
    strip.show();
}

void parseMessage(String message){
Serial.print("Parse message \t");
Serial.println(message);

if(message.length()>0){
  String annotation = message.substring(0,2);

  if(annotation =="AL" ){
    Serial.println("Setting alarm Time");
  //String parsedmessage = message.substring(2,message.length());
  
    //10 10 14  1032
  Serial.print("Month");
  Serial.println( message.substring(3,5));  
  int month = (message.substring(3,5)).toInt();

  Serial.print("Day");
  Serial.println( message.substring(5,7));  

  Serial.print("Year");
  Serial.println( message.substring(7,9));  

  Serial.print("Hour");
  Serial.println( message.substring(9,11)); 

  Serial.print("Min");
  Serial.println( message.substring(11,13));   
    
  
    
    
    
  }else if(annotation = "CL"){
    Serial.println("Setting COLOR");
  }else if(annotation = "IC"){
    Serial.println("Setting ALARM TIME");
  }

}
}
