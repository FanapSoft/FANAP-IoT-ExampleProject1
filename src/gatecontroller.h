#ifndef _GATE_CONTROLLER_H_
#define _GATE_CONTROLLER_H_

#include <ESP32Servo.h>

class GateController
{
public:
    void init(int servo_io, int servo_close_value, int servo_open_value) {
        servo.attach(servo_io);
        open_value = servo_open_value;
        close_value = servo_close_value;
        state = 0;
    }

    int open() {
        servo.write(open_value);
        state = 1;
    }

    int close() {
        servo.write(close_value);
        state = 0;
    }

    Servo servo;
    int close_value;
    int open_value;
    int state;
};



#endif