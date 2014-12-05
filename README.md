BluetoothWindowLight
====================
This project was created to help with the winter fading morning sun. Ive combined and arduino using Bluetooth Low Energy (BLE 4.0) - nRF8001 Breakout board and a strip of neopixels to create a light that will continue to grow brighter at a specific time each day. There is a ios project that is used to set the current time, alarm time and light color via bluetooth.  Since there is no battery to maintain the clock time, the ios application will update the current time each time it makes a connection with the arduino. 

I have tacked my led strip to sit behind my window's blinds to give the most sun like feel.

Features:
-Control over light color
-Control over how long the lights stay on
-Force the light to turn on without disrupting the alarms. 


![alt tag](https://github.com/prudolph/BluetoothWindowLight/blob/master/WindowLightArduino/WindowLight/WindowLight_bb.jpg)