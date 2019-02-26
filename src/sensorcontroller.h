#ifndef _SENSOR_CONTROLLER_H
#define _SENSOR_CONTROLLER_H

#include "Arduino.h"

#define SENSOR_WAIT_MS  600

class SensorController
{
  private:
    int sensor_pin;
    int threshold_low;
    int threshold_high;
    int cur_state;
    int next_state;
    unsigned long int change_time;
    bool state_changed; // Indicates state is changed in last handle called
    
  public:
    int last_sensor_value;

    void init(int sensor_pin, int threshold_low, int threshold_high)
    {
        this->sensor_pin = sensor_pin;
        this->threshold_high = threshold_high;
        this->threshold_low = threshold_low;

        int avg = 0;
        for (int i = 0; i < 10; i++)
        {
            last_sensor_value = analogRead(sensor_pin);
            avg += last_sensor_value;
        }
        avg = avg / 10;

        // Decide for initial state
        cur_state = (avg >= threshold_high);
    }

    void handle() {
        int cur_sensor = analogRead(sensor_pin);
        int t = millis();
        int wait_ok = (t-change_time)>=SENSOR_WAIT_MS; 
        int change = (cur_state)?(cur_sensor<threshold_low):(cur_sensor>threshold_high);
        last_sensor_value = cur_sensor;
        state_changed = false;

        if (change) {
            if (next_state==cur_state) {
                change_time = t;
                next_state = !cur_state;
            } else {
                if (wait_ok) {
                    cur_state = next_state;
                    state_changed = true;
                }
            }
        } else {
            next_state = cur_state;
        }
    }

    int get_current_state() {
        return cur_state;
    }

    // return 0=unchanged 1=High, -1=Low
    int get_change_state() {
        if (state_changed) {
            return (cur_state)?1:-1;
        }
        return 0;
    }
};

#endif