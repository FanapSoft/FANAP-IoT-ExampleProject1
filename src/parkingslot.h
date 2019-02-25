#ifndef _PARKING_SLOT_H_
#define _PARKING_SLOT_H_

#include "ledblinker.h"

class ParkingSlot
{
public:
    enum LedState
    {
        ON,
        OFF,
        BLINK1,
        BLINK2
    };

    void init(char *device_id, char *enc_key, int led_pin, int sensor_io,
              int sensor_low_thershold, int sensor_high_threshold);

    void set_led(LedState state);

    void handle();

private:
    char *device_id;
    char *enc_key;
    int led_pin;
    int sensor_io;

    LedBlinker blinker;
};

#endif