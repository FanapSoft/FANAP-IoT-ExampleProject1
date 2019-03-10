#ifndef _PARKING_GATE_H_
#define _PARKING_GATE_H_

#include "ledblinker.h"
#include "sensorcontroller.h"
#include "periodicjob.h"
#include <ArduinoJson.h>
#include "fanaccess.h"
#include "gatecontroller.h"
#include "delayedjob.h"

typedef bool (*mqtt_client_pub_t)(const char *, const char *);

class ParkingGate
{
  public:
    enum LedState
    {
        ON,
        OFF,
        BLINK1,
        BLINK2
    };

    FanAccess device;
    void init(char *device_id, char *enc_key, bool enc_en,
              int led_pin, int entry_sensor_io, int exit_sensor_io, int servo_io,
              int sensor_low_threshold, int sensor_high_threshold,
              int gate_low, int gate_high);

    void set_mqtt_publish_access(mqtt_client_pub_t pub_func);

    void apply_key_value_cmd(JsonPair cmd);

    void closegate();

    void send_periodic_report();

    void handle();

  private:
    GateController gate;
    LedBlinker blinker;
    SensorController entry_sensor;
    SensorController exit_sensor;
    PeriodicJob report_job;
    DelayedJob closegate_job;
    void set_led(LedState state);
    int led_update_time; // ToDo: Replace it with actual time

    int entry_sensor_changed_time; // ToDo: Replace it with actual time
    int exit_sensor_changed_time; // ToDo: Replace it with actual time
    LedState led_state;

};

#endif