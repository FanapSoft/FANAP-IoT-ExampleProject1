#ifndef _PARKING_SLOT_H_
#define _PARKING_SLOT_H_

#include "ledblinker.h"
#include "sensorcontroller.h"
#include "fanenc.h"


typedef bool (*mqtt_client_pub_t)(void*, const char *, const char *);


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

    void init(char *device_id,  char *enc_key, bool enc_en, int led_pin, int sensor_io,
              int sensor_low_thershold, int sensor_high_threshold);

    void set_mqtt_publish_access(mqtt_client_pub_t pub_func, void * client_data);

    void set_led(LedState state);

    void handle();

    int get_sensor_state()
    {
        return sensor.get_current_state();
    }

    int get_sensor_last_value()
    {
        return sensor.last_sensor_value;
    }

  private:
    char *device_id;
    bool enc_en;
    int led_pin;
    int sensor_io;

    void * mqtt_client_data;
    mqtt_client_pub_t pub_func;

    LedBlinker blinker;
    SensorController sensor;
    FanEnc enc;
};

#endif