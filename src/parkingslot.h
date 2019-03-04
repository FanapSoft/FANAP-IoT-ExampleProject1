#ifndef _PARKING_SLOT_H_
#define _PARKING_SLOT_H_

#include "ledblinker.h"
#include "sensorcontroller.h"
#include "fanenc.h"
#include "periodicjob.h"
#include <ArduinoJson.h>

#define MAX_TOPIC_LEN 80

typedef bool (*mqtt_client_pub_t)(const char *, const char *);


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

    void set_mqtt_publish_access(mqtt_client_pub_t pub_func);

    const char * from_platform_topic();

    bool process_received_message(char * topic, char * payload, int msg_size);

    void handle();

    int get_sensor_state()
    {
        return sensor.get_current_state();
    }

    int get_sensor_last_value()
    {
        return sensor.last_sensor_value;
    }
    void create_send_report();

private:
    char *device_id;
    int led_pin;
    LedState led_state;
    int sensor_io;
    char platform_topic[MAX_TOPIC_LEN];
    mqtt_client_pub_t pub_func;
    int led_update_time; // ToDo: Replace it with actual time
    LedBlinker blinker;
    SensorController sensor;
    FanEnc enc;
    PeriodicJob report_job;

    void set_led(LedState state);
    void apply_key_value_cmd(JsonPair cmd);
    void cmd_led(const char * cmd);
    bool send_current_state_to_platform();
    const char * get_str_led_state();
};

#endif