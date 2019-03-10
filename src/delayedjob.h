#ifndef _DELAYED_JOB_H
#define _DELAYED_JOB_H


#include <Arduino.h>

class DelayedJob
{
public:
    
    void init(void (*func)(void*data), void* data) {
        this->func = func;
        this->callback_data = data;
        exec = false;
    }

    void run(unsigned int delay_ms) {
        exec_time = millis() + delay_ms;
        exec = true;
    }



    bool handle() {
        unsigned int cur_time = millis();

        if (exec && (cur_time>exec_time)) {
            exec = false;
            if (func) {
                func(callback_data);
            }
            return true;
        } else {
            return false;
        }
    }

private:
    void (*func)(void*);
    void * callback_data;
    bool exec;
    unsigned int exec_time;

};


#endif