#include "Arduino.h"
#include "parkingslot.h"

#define BLINK_PERIOD_ON 300
#define BLINK_PERIOD_OFF 900

static const int _blink1_list[] = {BLINK_PERIOD_ON,-(BLINK_PERIOD_OFF),0};
static const int _blink2_list[] = {BLINK_PERIOD_OFF, -(BLINK_PERIOD_ON) ,0};

void ParkingSlot::init(char *device_id, char *enc_key, bool enc_en, int led_pin, int sensor_io,
              int sensor_low_thershold, int sensor_high_threshold)
{
    this->device_id = device_id;
    this->enc_en = enc_en;
    sensor.init(sensor_io, sensor_low_thershold, sensor_high_threshold);
    blinker.init(led_pin);
    set_led(OFF);
    enc.set_key((const unsigned char*)enc_key);
}


void ParkingSlot::set_mqtt_publish_access(mqtt_client_pub_t pub_func, void * client_data) {
    this->mqtt_client_data = client_data;
    this->pub_func = pub_func;
}

void ParkingSlot::set_led(LedState state)
{
    switch (state)
    {
        case ON:
            blinker.set_state(1);
            break;
        case OFF:
            blinker.set_state(0);
            break;
        case BLINK1:
            blinker.set_pattern(_blink1_list);
            break;
        case BLINK2:
            blinker.set_pattern(_blink2_list);
            break;
        default:
            break;
    }
}


void ParkingSlot::handle() {
    blinker.handle();
    sensor.handle();

    // This is test code:
    int change = sensor.get_change_state();

    if (change>0) {
        pub_func(mqtt_client_data,"D2P_topic", "{\"state\"=\"HI\"}");

    } else if (change<0) {
        pub_func(mqtt_client_data,"D2P_topic", "{\"state\"=\"LOW\"}");
    }
}

void ParkingSlot::from_platform_topic(char * buffer) {
    sprintf(buffer, "%s/p2d",device_id);
}
