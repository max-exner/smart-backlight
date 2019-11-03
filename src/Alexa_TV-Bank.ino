#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include "wifi_pass.h"

#define SERIAL_BAUDRATE                 9600
#define LED_STRIP                       5
#define FAN                             4
#define SWITCH                          14 

fauxmoESP fauxmo;
bool b_tvlicht = false;
bool b_tvlicht_dim = false;
bool b_fan = false;
bool b_switch = false;
bool b_switch_old = false;
int i;

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED && millis()<30000) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    if(WiFi.status() == WL_CONNECTED){
      Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }
    else{
      Serial.printf("[WIFI] No Connection to SSID: %s. Try again with a NodeMCU reset.\n[INFO] The button funktion are activated now\n", WIFI_SSID);
    }
}

void setup() {

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // Wifi
    wifiSetup();

    // Ports
    pinMode(LED_STRIP, OUTPUT);
    pinMode(FAN, OUTPUT);
    pinMode(SWITCH, INPUT_PULLUP);
    //digitalWrite(LED_STRIP, HIGH);

    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // Add virtual devices
    fauxmo.addDevice("TV Licht");
    fauxmo.addDevice("Fan");

    // fauxmoESP 2.0.0 has changed the callback signature to add the device_id, this WARRANTY
    // it's easier to match devices to action without having to compare strings.
    fauxmo.onMessage([](unsigned char device_id, const char * device_name, bool state) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
        if (device_id == 0){          //TV Licht
          if(b_tvlicht!=state){       //Verriegelung damit Licht nicht "zum dimmen an/aus geht"
            b_tvlicht = state;          
            b_tvlicht_dim = true;
          }
        }
        else if(device_id ==1){       //PS4 Luft
          b_fan = state;
        }
    });

}

void loop() {

    // Since fauxmoESP 2.0 the library uses the "compatibility" mode by
    // default, this means that it uses WiFiUdp class instead of AsyncUDP.
    // The later requires the Arduino Core for ESP8266 staging version
    // whilst the former works fine with current stable 2.3.0 version.
    // But, since it's not "async" anymore we have to manually poll for UDP
    // packets
    fauxmo.handle();

    //Taster Handling fallende Flanke erkennen, da Input Pullup
    b_switch=digitalRead(SWITCH);
    if(b_switch == 0 && b_switch_old == 1){
       Serial.printf("[SWITCH] Taster wurde betätigt\n");
       b_tvlicht ^= true;
       b_tvlicht_dim = true;
    }
    delay(10);                  //Pollen abfangen
    b_switch_old = b_switch;
    
    
    
    
    //DIM Handling:
    //dim und tvlicht var werden von FAUXMO gesetzt
    if(b_tvlicht_dim){                          //ist eine neuer Befehl zum dimmen von Alexa eingegangen?
      fauxmo.enable(false);
      if (b_tvlicht){
        for(i=0; i<=PWMRANGE; i++){             //Für Licht an zähle hoch und schreibe in analogWrite (PWMRANGE=1023)!!!
          analogWrite(LED_STRIP, i);
//          Serial.printf("[FOR_+] %d\n", i);
          delay(3);
        }
//      Serial.printf("[FOR_+] %d\n", i); 
      }
      else{
        for(i=PWMRANGE; i>=0; i--){
          analogWrite(LED_STRIP, i);
//          Serial.printf("[FOR_-] %d\n", i);
          delay(3);
        }
//       Serial.printf("[FOR_-] %d\n", i);

      }
      b_tvlicht_dim = false;
      fauxmo.enable(true);
    }


    //Fan Ausgang setzten
    digitalWrite(FAN, b_fan);


    //Status ausgeben alle 5s
    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("[MAIN_VAR] b_tvlicht = %s, b_fan = %s\n", b_tvlicht ? "ON" : "OFF", b_fan ? "ON" : "OFF");
    }

}
