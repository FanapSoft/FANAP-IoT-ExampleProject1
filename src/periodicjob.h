#ifndef _PERIODIC_JOB_H
#define _PERIODIC_JOB_H


#include <Arduino.h>

class PeriodicJob
{
public:
    
    void init(unsigned int period_sec, void (*func)(void*data), void* data) {
        this->period_sec = period_sec;
        this->func = func;
        this->callback_data = data;
        last_execute = millis();
        force_execute = false;
    }

    void register_execute() {
        force_execute = true;
    }

    bool handle() {
        unsigned int cur_time = millis();
        unsigned int diff = cur_time - last_execute;

        if (force_execute || (period_sec && diff>=period_sec*1000)) {
            force_execute = false;
            if (func) {
                func(callback_data);
            }
            last_execute = millis();
            return true;
        } else {
            return false;
        }
    }

    unsigned int get_period() {
        return period_sec;
    }

    void set_period(unsigned int period_sec) {
        this->period_sec = period_sec;
    }
    
private:
    void (*func)(void*);
    void * callback_data;

    unsigned int last_execute;
    bool force_execute;
    unsigned int period_sec;

};


#endif