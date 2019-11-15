#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include "wifi_pass.h"
#include "debounce.h"

#define SERIAL_BAUDRATE                 9600
#define LED_STRIP                       5
#define DECICE_2                             4
#define SWITCH                          14 
#define VALUERANGE                      255

//define device names (that are the names that Alexa will show)
#define ID_DEVICE_1                     "TV Licht"
#define ID_DEVICE_2                     "I don't know"


//define global variables
fauxmoESP fauxmo;
bool b_backlight_state = false;
bool b_backlight_dim = false;
unsigned int ui_backlight_value = 255;
unsigned int pwm_value_last = 0;
bool b_device_2_state = false;
bool b_switch = false;
bool b_switch_old = false;
unsigned int pwm_value_current;
debounce * db_switch;

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);
    wifi_set_sleep_type(MODEM_SLEEP_T);
    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    // Wait for WIFI
    while (WiFi.status() != WL_CONNECTED && millis()<30000) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();
    // If connected than show connection info, if not go on (then just the button for device one works)
    if(WiFi.status() == WL_CONNECTED){
      Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }
    else{
      Serial.printf("[WIFI] No Connection to SSID: %s. Try again with a NodeMCU reset.\n[INFO] The button function are activated now\n", WIFI_SSID);
    }
}
// -----------------------------------------------------------------------------
// SETUP
// -----------------------------------------------------------------------------
void setup() {

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();
    // Wifi
    wifiSetup();
    // Ports
    pinMode(LED_STRIP, OUTPUT);
    pinMode(DECICE_2, OUTPUT);
    pinMode(SWITCH, INPUT_PULLUP);
    db_switch = new debounce(SWITCH, 10);

    // By default, fauxmoESP creates it's own webserver on the defined port
    // The TCP port must be 80 for gen3 devices (default is 1901)
    // This has to be done before the call to enable()
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices
    

    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // Add virtual devices
    fauxmo.addDevice(ID_DEVICE_1);
    fauxmo.addDevice(ID_DEVICE_2);

    // fauxmoESP set state event
    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
        if (device_id == 0){                      //Backlight
          if(b_backlight_state!=state || ui_backlight_value != value){           //lock states (prevent a dim routine when the light is still on)"
            ui_backlight_value = value;           //set the current value to the backlights value
            b_backlight_state = state;            //set the current state to the backlight state
            b_backlight_dim = true;               //set the backligth dim flag
          }
        }
        else if(device_id ==1){       //Device 2
          b_device_2_state= state;
        }
    });

}
// -----------------------------------------------------------------------------
// LOOP
// -----------------------------------------------------------------------------
void loop() {
    unsigned long ul_last = millis();
    //do fauxmo handle
    fauxmo.handle();

    //Detect falling edge on the switch (The switch is connectet to a input pullup)
    b_switch = db_switch->get_debounced();
    if(b_switch == 0 && b_switch_old == 1){
       Serial.printf("[SWITCH] The switch was toggeld\n");
       if (!b_backlight_state){
        ui_backlight_value = 255;
       }
       b_backlight_state ^= true;
       b_backlight_dim = true;       
    }
    b_switch_old = b_switch;
    
            
    //DIM Handling:
    //the flags are set by the fauxmo on set state function


    if(b_backlight_dim){                          //is there a new dim comand?
      unsigned int pwm_value_target = round(PWMRANGE/double(VALUERANGE) * ui_backlight_value);  //calculate the pwm_value_target (PWMRANGE=1023 & VALUERANGE = 255)
      //Serial.printf("[MAIN] The backlight value is: %i\n", ui_backlight_value);
      //Serial.printf("[MAIN] The max pwm_value_target is: %i\n", pwm_value_target);
      if (b_backlight_state){                                  //light state is on
        //light up
        if(pwm_value_last < pwm_value_target){
          for(pwm_value_current=pwm_value_last; pwm_value_current<=pwm_value_target; pwm_value_current++){             //to dim up count from the old pwm value to the new pwm value
            analogWrite(LED_STRIP, pwm_value_current);
            delay(3);
          }
        }
        //light down
        else
        {
          for(pwm_value_current=pwm_value_last; pwm_value_current>pwm_value_target; pwm_value_current--){             //to dim down count reverse
            analogWrite(LED_STRIP, pwm_value_current);
            delay(3);
          }
        }
      }
      else{                                                    //light state is off
        for(pwm_value_current=pwm_value_last; pwm_value_current>0; pwm_value_current--){                            //dim down to zero
          analogWrite(LED_STRIP, pwm_value_current);
          delay(3);
        }
      }      
      pwm_value_last = pwm_value_current;                     //set last pwm value to current
      b_backlight_dim = false;                                //disable dim flag
    }

    //DECICE_2 Ausgang setzten
    digitalWrite(DECICE_2, b_device_2_state);
    
    //Make sure that the loop works in (soft) realtime
    //The realtime requirements are missed when a dim action is done
    //The delay is needed to reduce the power consumtion
    unsigned long ul_delta = millis()-ul_last;
    if (ul_delta<200){
      delay(200-ul_delta);
    }
    else
    {
      Serial.printf("[TIMING] The loop miss required timing: %i\n", ul_delta);
    }
    
    //Status every 5sec
    /*static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("[MAIN_VAR] b_backlight = %s, b_DECICE_2 = %s\n", b_backlight_state ? "ON" : "OFF", b_device_2_state ? "ON" : "OFF");
        Serial.printf("[MAIN] The current pwm value is: %i\n", pwm_value_current);
    }*/

}
