# smart-backlight
This is a DIY project that makes a LED lighting strip smart #Alexa #Smarthome

This project is still work in progress. Please excuse minor bugs. I hope it inspires you! Please feel free to use the sources for your project.

## Project discription 
There is a huge range of available backlight and lighting strip solutions on the market. But if you want to create your own smart backlight system that does not require cloud access and is simple to integrate into the Alexa world, then this project is what you are looking for. It uses an LED lighting strip from IKEA and makes it smart with an ESP8266 development board. The main features of this smart-backlight are:
* Controll the light with your voice via alexa or a push button
* The light dims up and down when it is turned on or off (it is so cool)
* It's possible to dim the light via a alexa comand
* It's possible to add a second 12V device that is controlled by the ESP8266
* The ESP8266 and the circuit is sourounded with a 3D printed housing

Because it is work in progress some features are available in the future:
* A scematic of the circuit
* Easy WiFi Setup without changing the source code
* MQTT-Support for real cloudless smart home

But see the projct in action:

<a href="http://www.youtube.com/watch?feature=player_embedded&v=YOUTUBE_VIDEO_ID_HERE
" target="_blank"><img src="http://img.youtube.com/vi/YOUTUBE_VIDEO_ID_HERE/0.jpg" 
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="10" /></a>

## How to use the sources
First of all you need to setup the hardware. After that please use the [example.wifi_pass.h](/pio_project/src/example.wifi_pass.h) to enter your wifi configuration. After you have done this remove the "example." from the filename. 
I use PlatformIO for development, but it is easy to port the source to the Arduino IDE. The sources requires the following dependencies. Please follow the links to get this packages:
* [Arduino core for ESP8266 WiFi chip](https://github.com/esp8266/Arduino)
* [FauxmoESP](https://github.com/simap/fauxmoesp)

Compile and upload the sources to the ESP8266. Search for devices with your alexa. Controll it with the button or your voice:

```
Alexa, turn on the TV-light
```

```
Alexa, turn off the TV-light
```

```
Alexa, set TV-light to 40%
```

And of course have a lot of fun with your smart-backlight!
## Used Parts
|Part|Price (approx)|
|---|---|
|LEDBERG lighting strip from IKEA (The lightstrip that I bougth inclueds a 12V powersupply)|8€|
|NodeMCU development board (with ESP8266-12E)  |3€|
|Prototype PCB|<1€|
|Some resitors|<1€|
|Transistor|<1€|
|screw terminals for PCBs|1€|
|Button|1€|
|Some code|priceless (but you get it for free)|

