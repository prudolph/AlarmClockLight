AlarmClockLight
====================
This project was created to help with the sunrise become later in the winter months. Ive combined and Arduino Uno, a Bluetooth BLE 4.0 radio and a strip of LED to create a light that will gradually become brighter a specific time every day. I have placed the strip of LEDs between my window and blinds to try to mimic natural sunlight. There is an accompanying  iOS project that is used to set the current time, alarm time and light color via bluetooth.  Since there is no battery to maintain the clock time, the ios application will update the current time each time it makes a connection with the Arduino. 


Parts Needed:<br>
Arduino Uno R3<br>
Adafruit nrf8001 Bluetooth Breakout board<br>
Adafruit NeoPixel Digital RGB LED Strip<br>
9 volt wall adapter<br>


IOS Application Features:<br>
-Control light trigger  time  and duration<br>
-Control light color<br>
-Force the light to turn on without disrupting the alarm trigger times <br>

Wiring Diagram
![alt tag](https://github.com/prudolph/BluetoothWindowLight/blob/master/WindowLightArduino/WindowLight/WindowLight_bb.jpg)


Windows Example
![alt tag](https://github.com/prudolph/BluetoothWindowLight/blob/master/WindowLightExample.JPG)


The Adafruit "Getting Started with the nRF8001 Bluefruit LE Breakout" was invaluable for getting this put together. It will be very helpful for getting starting with working with the bluetooth breakout board<br>
https://learn.adafruit.com/getting-started-with-the-nrf8001-bluefruit-le-breakout/introduction
