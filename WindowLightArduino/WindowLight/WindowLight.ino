
/* 
 * Window Light Alarm Clock
 * This project makes use of the Bluefruit LE - Bluetooth Low Energy (BLE 4.0) - nRF8001 Breakout 
 * and Adafruit NeoPixel Digital RGB LED Strip - White 60 LED
 *
 * Time, alarm , and light color can be set with the accompaning ios application
 *
 * 
 */

#include <Time.h>  
#include <TimeAlarms.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include "Adafruit_BLE_UART.h"
#define USE_SPECIALIST_METHODS true

byte redValue=255,greenValue=211,blueValue = 50;


byte lightBrightness = 0;
int  lightBrightnessIncTimeMillis =8500 ;
byte maxBrightness = 250;

bool alarmTriggered  = false;
double increaseBrightnessTimestamp=0;

AlarmID_t lightsOnAlarmID,ligthsOffAlarmID;
byte alarmHour=0,alarmMin=0;
byte alarmDuration = 30;

//LEDSTRIP
Adafruit_NeoPixel strip = Adafruit_NeoPixel(22, 8, NEO_GRB + NEO_KHZ800);


///////////////BLUETOOTH
// Connect CLK/MISO/MOSI to hardware SPI
// e.g. On UNO & compatible: CLK = 13, MISO = 12, MOSI = 11
//#define ADAFRUITBLE_REQ 10
//#define ADAFRUITBLE_RDY 2     // This should be an interrupt pin, on Uno thats #2 or #3
//#define ADAFRUITBLE_RST 9

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(10, 2, 9);
aci_evt_opcode_t curStatus = ACI_EVT_DEVICE_STARTED;
void setup()  {

  Serial.begin(9600);

// Initialize all pixels to 'off'
  strip.begin();
  strip.setBrightness(0);
  for(int i =0;i<22;i++) strip.setPixelColor(i, 255, 211, 50);
  strip.show(); 

  //Initalize Blue tooth
  BTLEserial.setDeviceName("Window"); /* 7 characters max! */
  BTLEserial.begin();

//Set default light alarms
  lightsOnAlarmID= Alarm.alarmRepeat(  07,00,00, LightsOnAlarm);  // 8:30am every day
  ligthsOffAlarmID=Alarm.alarmRepeat(07,30,00, LightsOffAlarm);  // 8:30am every day


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
  curStatus = BTLEserial.getState();
  // If the status changed....
  if (curStatus != laststatus)laststatus = curStatus;
  //If we have a connection check if there is data to read
  if (curStatus == ACI_EVT_CONNECTED) {
    //If the time is not set ask the phone to set the time
    if(timeStatus()== timeNotSet){
      sendData("GT");
    }
    // Lets see if there's any data for us!
    // OK while we still have something to read, get a character and print it out
    String message = "";
    while (BTLEserial.available()) {
      char c = BTLEserial.read();
      message+=c;
    }
    parseMessage(message);
  }


  if(timeStatus()!= timeNotSet)   
  {
    digitalWrite(13,timeStatus() == timeSet); // on if synced, off if needs refresh  
    digitalClockDisplay();  
  }

  //Check if we should increase brightness
  if(alarmTriggered){

    if(millis()-increaseBrightnessTimestamp>lightBrightnessIncTimeMillis){
      increaseBrightnessTimestamp=millis();
      lightBrightness+=1;
      if(lightBrightness>= maxBrightness){ 
        lightBrightness=maxBrightness;
      }


    }
    Serial.print("Brightness: ");
    Serial.println(lightBrightness);




  }

  strip.setBrightness((int)lightBrightness);
  for(int i =0;i<22;i++) strip.setPixelColor(i, redValue, greenValue, blueValue);
  strip.show(); 

  Alarm.delay(1000); 
}



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
  Serial.println(F("Alarm: - turn lights ON")); 
  increaseBrightnessTimestamp = millis();  
  alarmTriggered=true;
}

void LightsOffAlarm(){
  Serial.println(F("Alarm: - turn lights OFF"));    
  alarmTriggered=false;
  lightBrightness=0;
  strip.show();
}

void parseMessage(String message){
  Serial.println(message);

  if(message.length()>0){
    String annotation     = message.substring(0,2);
    String parsedmessage  = message.substring(3,message.length());

    if(annotation.equals("AL") ){
      Serial.println(F("Setting alarm Time"));
      
      alarmHour = message.substring(9,11).toInt();
      alarmMin = message.substring(11,13).toInt();


      Serial.print(F("Hour"));
      Serial.println( (int)alarmHour); 

      Serial.print(F("Min"));
      Serial.println( (int)alarmMin);   


      Alarm.free(lightsOnAlarmID);
      lightsOnAlarmID= Alarm.alarmRepeat(  (int)alarmHour,//Hours
      (int)alarmMin,//Minutes
      0, LightsOnAlarm);  // 8:30am every day



      String response = "AS:";
      response+=message.substring(9,11);
      response+=message.substring(11,13);
      sendData(response);                                
      int totalMinutes =  (alarmHour*60)+alarmMin+alarmDuration;

      byte lightOffHour=floor(totalMinutes/60);
      byte lightOffminute=floor(totalMinutes%60);

      if(lightOffHour>23){ 
        lightOffHour =0;
      }

      Serial.print(F("lightOffHour"));
      Serial.println( (int)lightOffHour);   

      Serial.print(F("lightOffminute"));
      Serial.println( (int)lightOffminute);     


      Alarm.free(ligthsOffAlarmID);
      ligthsOffAlarmID=Alarm.alarmRepeat((int)lightOffHour,(int)lightOffminute,00, LightsOffAlarm);  // 8:30am every day
      LightsOffAlarm();

    }


    if(annotation.equals( "CL")){

      Serial.println(F("Setting COLOR"));


      byte colorIndex=0;
      parsedmessage+=",";

      String curColorString="";
      for(byte i =0;i<parsedmessage.length();i++){

        if(parsedmessage.charAt(i)==','){

          if(colorIndex==0){  
            redValue=curColorString.toInt();
          }
          else if(colorIndex==1){ 
            blueValue  =curColorString.toInt();
          }
          else if(colorIndex==2){ 
            greenValue=curColorString.toInt();

          }
          colorIndex++     ;
          curColorString=""; 
        }
        else{
          curColorString+=parsedmessage[i];
        }
      }


      Serial.print(F("redValue  \t"));
      Serial.println(redValue);
      Serial.print(F("greenValue \t"));
      Serial.println(greenValue);

      Serial.print(F("blueValue \t"));
      Serial.println(blueValue);
      //Set the new color
      for(int i =0;i<22;i++) strip.setPixelColor(i, redValue, greenValue, blueValue);
    }

    if(annotation.equals( "IC")){
      Serial.println(F("Setting ALARM DRUATION"));
      alarmDuration= parsedmessage.toInt();

      int totalMinutes =  (alarmHour*60)+alarmMin+alarmDuration;
      Serial.print(F("TotalMins"));
      Serial.println( totalMinutes);   

      byte lightOffHour=floor(totalMinutes/60);
      byte lightOffminute=floor(totalMinutes%60);
      if(lightOffHour>23){ 
        lightOffHour =0;
      }

      Serial.print(F("lightOffHour"));
      Serial.println( lightOffHour);   

      Serial.print(F("lightOffminute"));
      Serial.println( lightOffminute);     

      Alarm.free(ligthsOffAlarmID);
      ligthsOffAlarmID=Alarm.alarmRepeat(lightOffHour,lightOffminute,00, LightsOffAlarm);  // 8:30am every day

      LightsOffAlarm();
    }

    if(annotation.equals( "ST")){
      Serial.println(F("Setting CLOCK TIME"));
      /*
         Serial.print(F("Month"));
       Serial.println( message.substring(3,5));  
       
       Serial.print(F("Day"));
       Serial.println( message.substring(5,7));  
       
       Serial.print(F("Year"));
       Serial.println( message.substring(7,9));  
       
       Serial.print(F("Hour"));
       Serial.println( message.substring(9,11)); 
       
       Serial.print(F("Min"));
       Serial.println( message.substring(11,13)); 
       */
      setTime(((int)message.substring(9,11).toInt()), 
      ((int)message.substring(11,13).toInt()),
      00,
      ((int)message.substring(5,7).toInt()),
      ((int)message.substring(3,5).toInt()), 
      ((int)message.substring(7,9).toInt())); 


      String response = "TS:";
      response+=message.substring(9,11);
      response+=message.substring(11,13);
      sendData(response);
    }

    if(annotation.equals( "FL")){
      Serial.println(F("Force Lights on/off"));
      if(parsedmessage.equals("0")){
        LightsOffAlarm();
      }
      else{

        LightsOnAlarm();
        lightBrightness=50;

      }
      Serial.println(parsedmessage);
      String response = "FL"+parsedmessage;
      response+=message.substring(9,11);
      response+=message.substring(11,13);
      sendData(response);
    }


  }
}


void sendData(String message){
  if (curStatus == ACI_EVT_CONNECTED) {
    // We need to convert the line to bytes, no more than 20 at this time
    uint8_t sendbuffer[20];
    message.getBytes(sendbuffer, 20);
    char sendbuffersize = min(20, message.length());

    Serial.print(F("\n* Sending -> \"")); 
    Serial.print((char *)sendbuffer); 
    Serial.println("\"");

    // write the data
    BTLEserial.write(sendbuffer, sendbuffersize);
  }

}

