#include "debounce.h"

debounce::debounce(u8 u8_port, u8 u8_bounce_delay){
    this->u8_port = u8_port;
    this->u8_bounce_delay = u8_bounce_delay;
    this->b_last_state=digitalRead(u8_port);
    this->b_return_state = this->b_last_state;
}

bool debounce::get_debounced(){
    
    bool b_current_state = digitalRead(u8_port);
    //if state change start counter
    if (b_current_state != b_last_state){
        this->last_debounce_time = millis();
    }
    //if delay time is reached then check if the state is currently different
    if((millis()-last_debounce_time)>u8_bounce_delay){
        //if retun state is different from current state, change return value
        if(b_current_state != b_return_state){
            b_return_state = b_current_state;
        }
    }
    b_last_state = b_current_state;
    return b_return_state;
}