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
double increaseBrightnessTimestamp=0;

int redValue,greenValue,blueValue;

//Alarms
bool alarmTriggered  = false;
AlarmID_t lightsOnAlarmID,lightsOffAlarmID;

 int alarmHour=06,alarmMinute=30;
int alarmDurationMinutes;

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
  while(!Serial);
   setTime(22,00,00,02,10,2014); 
   
   /*
  tmElements_t lightsOnTimeElements;
    lightsOnTimeElements.Hour = 06;
    lightsOnTimeElements.Minute = 30;
    lightsOnTimeElements.Second = 0; 
    lightsOnTime=makeTime(lightsOnTimeElements);
*/
    


  alarmDurationMinutes=1;

 strip.begin();
 strip.setBrightness(0);
 strip.show(); // Initialize all pixels to 'off'
 
 //Initalize Bluth
  BTLEserial.setDeviceName("Window"); /* 7 characters max! */
  BTLEserial.begin();
  
  //Init colors
   redValue=255;
   greenValue=211;
   blueValue=50;
   
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
//  Serial.print(hourFormat12());
 Serial.print(hour());
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
  Serial.print("Parse message \t");Serial.println(message);

  if(message.length()>0){
    String annotation = message.substring(0,2);
    String data = message.substring(3,message.length());
    
    Serial.print("Aannotation \t");Serial.println(annotation);
   Serial.print("message \t");Serial.println(data);
  if(annotation.equals("AL") ){  setAlarm( data);}
  else if(annotation.equals("CL")){  setColor( data);}
  else if(annotation.equals("IC")){  setAlarmDuration( data);}
  else if(annotation.equals("ST")){   setTime( data);}
  }
}

void setColor(String data){
 Serial.println("Setting COLOR");
 
 
    String colorString = data+",";//Added a comma to trigger the last color
    Serial.print("colorsString  \t");Serial.println(colorString);
    
    
    int colorIndex=0;
    String curColorString="";
   
    for(int i =0;i<colorString.length();i++){
      if(colorString.charAt(i)==','){
        if(colorIndex==0){  redValue=curColorString.toInt();}
        else if(colorIndex==1){ greenValue=curColorString.toInt();}
        else if(colorIndex==2){ blueValue=curColorString.toInt();}
        colorIndex++     ;
        curColorString=""; 
      }else{
        curColorString+=colorString[i];
       }
    }


      Serial.print("redValue  \t");Serial.println(redValue);
      Serial.print("greenValue \t");Serial.println(greenValue);
      Serial.print("blueValue \t");Serial.println(blueValue);
}


void setTime(String data){
  Serial.println("Setting TIME");
}


void setAlarm(String data){
Serial.println("Setting alarm Time");
    
 
     alarmHour = data.substring(6,8).toInt();
     alarmMinute=data.substring(8,10).toInt();
    Serial.print("Alarm Hour \t");Serial.println(alarmHour);
    Serial.print("Alarm Minute \t");Serial.println(alarmMinute);


  int endAlarmHour;
   int endAlarmMinute;
   
   int totalMinutes = (alarmHour*60) +alarmMinute+alarmDurationMinutes;
   Serial.print("Total Minutes \t");Serial.println(totalMinutes);
  endAlarmHour = floor((totalMinutes/60));
  endAlarmMinute = (totalMinutes%60);
  
  if(endAlarmHour>23){
    endAlarmHour=0;
  }
    Serial.print("END Alarm Hour \t");Serial.println(endAlarmHour);
    Serial.print("END Alarm Minute \t");Serial.println(endAlarmMinute);
   
    //Alarm.disable(lightsOnAlarmID); 
    //Alarm.disable(lightsOffAlarmID);
     Alarm.write(lightsOnAlarmID,  Alarm.alarmRepeat(alarmHour,alarmMinute,0, LightsOnAlarm));
    lightsOnAlarmID =   Alarm.alarmRepeat(alarmHour,alarmMinute,0, LightsOnAlarm);
    lightsOffAlarmID =  Alarm.alarmRepeat(endAlarmHour,endAlarmMinute, 0, LightsOffAlarm);  // 8:30am every day
     
   
    
}


void setAlarmDuration(String data){
 Serial.println("Setting ALARM DURATION");
     alarmDurationMinutes = (data.toInt());
     Serial.print("alarmDuration \t");Serial.println(alarmDurationMinutes);
     
     
   int endAlarmHour;
   int endAlarmMinute;
   
   int totalMinutes = (alarmHour*60) +alarmMinute+alarmDurationMinutes;
   Serial.print("Total Minutes \t");Serial.println(totalMinutes);
  endAlarmHour = floor((totalMinutes/60));
  endAlarmMinute = (totalMinutes%60);
  
  if(endAlarmHour>23){
    endAlarmHour=0;
  }
    Serial.print("END Alarm Hour \t");Serial.println(endAlarmHour);
    Serial.print("END Alarm Minute \t");Serial.println(endAlarmMinute);
   
    Alarm.disable(lightsOnAlarmID); 
    Alarm.disable(lightsOffAlarmID);
  
    lightsOffAlarmID =  Alarm.alarmRepeat(endAlarmHour,endAlarmMinute, 0, LightsOffAlarm);  // 8:30am every day
     
   
     
}
