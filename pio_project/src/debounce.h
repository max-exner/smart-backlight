#pragma once

#include <Arduino.h>

class debounce {
    public:
        debounce(u8 u8_port, u8 u8_bounce_delay);
        ~debounce();
        bool get_debounced();
    private:
        bool b_last_state;
        bool b_return_state;
        u8 u8_bounce_delay;
        u8 u8_port;        
        unsigned long last_debounce_time;
};
