#ifndef _LED_BLINKER_H
#define _LED_BLINKER_H

#include "Arduino.h"

const int _on_state[] = {1000,0};
const int _off_state[] = {-1000,0};


class LedBlinker
{
public:

    void init(int led_pin) {
        pin = led_pin;
        pinMode(pin, OUTPUT);
    }

    void set_onoff_list(const int* statelist) {
        this->statelist = statelist;
        index  = 0;
        blink_time = millis();
        apply_led();
    }

    void set_state(int value) {
        if (value) {
            set_onoff_list(_on_state);
        } else {
            set_onoff_list(_off_state);
        }
    }

    void handle() {
        unsigned long diff = millis() - blink_time;

        if (diff>get_cur_led_state()) {
            next_state();
            blink_time = millis();
            apply_led();
        }
    }

private:
    int pin;
    const int * statelist;
    int index;
    unsigned long blink_time;


    void apply_led() {
        digitalWrite(pin, (statelist[index]>0)?1:0);
    }

    int get_cur_led_state() {
        int c = statelist[index];
        if (c<0) {
            c=-c;
        }
        return c;
    }

    void next_state() {
        index++;
        if (!statelist[index])  {
            index = 0;
        }
    }



};


#endif