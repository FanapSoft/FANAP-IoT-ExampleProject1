#include "Arduino.h"
#include "parkingslot.h"

#define BLINK_PERIOD_ON 300
#define BLINK_PERIOD_OFF 900

#define MAX_MESSAGE_SIZE 1024
#define MAX_JSON_SIZE 512

static const int _blink1_list[] = {BLINK_PERIOD_ON,-(BLINK_PERIOD_OFF),0};
static const int _blink2_list[] = {BLINK_PERIOD_OFF, -(BLINK_PERIOD_ON) ,0};

void ParkingSlot::init(char *device_id, char *enc_key, bool enc_en, int led_pin, int sensor_io,
              int sensor_low_thershold, int sensor_high_threshold)
{
    this->device_id = device_id;
    sensor.init(sensor_io, sensor_low_thershold, sensor_high_threshold);
    blinker.init(led_pin);
    set_led(OFF);
    enc.set_key((const unsigned char*)enc_key, enc_en);


    snprintf(platform_topic,sizeof(platform_topic),"/%s/p2d",device_id);
}


void ParkingSlot::set_mqtt_publish_access(mqtt_client_pub_t pub_func) {
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
        pub_func("D2P_topic_HI", (const char *) device_id);
        //pub_func("D2P_topic", "{\"state\"=\"HI\"}");

    } else if (change<0) {
        pub_func("D2P_topic_LOW", (const char *) device_id);
        //pub_func("D2P_topic", "{\"state\"=\"HI\"}");
    }
}

const char * ParkingSlot::from_platform_topic() {
    return platform_topic;
}


bool ParkingSlot::process_received_message(char * topic, char * payload, int msg_size) {    
    if (strcmp(topic, platform_topic)!=0) {
        // Message dose not belong to this device
        return false;
    }

    char buffer[MAX_MESSAGE_SIZE];
    int size = enc.dec(payload, msg_size, buffer);


    if (size<=0) {
        // ToDo: Generate log for invalid message
        return false;
    }

    StaticJsonDocument<MAX_JSON_SIZE> jb;
    DeserializationError err = deserializeJson(jb, buffer, size);

    if (err) {
        Serial.printf("%s: message error: %s\n",device_id, err.c_str());
        return false;
    } 

    JsonObject obj = jb.as<JsonObject>();
    JsonObject data = obj["data"];

    if (data.isNull()) {
        Serial.printf("%s: error. \"DATA\" is missed!\n", device_id);
        return false;
    }

    for(JsonPair cmd: data) {
        apply_key_value_cmd(cmd);
    }
}

void ParkingSlot::apply_key_value_cmd(JsonPair cmd) {
    const char * key = cmd.key().c_str();

    if (strcmp(key, "led")==0) {
        cmd_led(cmd.value());
    } 

    Serial.printf("%s %s\n",device_id, key);
}

void ParkingSlot::cmd_led(const char * cmd) {
    if (cmd) {
        report_update = true;
        if (!strcmp(cmd, "G")) {
            set_led(ON);
        } else if (!strcmp(cmd,"R")) {
            set_led(OFF);
        } else if (!strcmp(cmd,"B1")) {
            set_led(BLINK1);
        } else if (!strcmp(cmd,"B2")) {
            set_led(BLINK2);
        }
    } 
}
