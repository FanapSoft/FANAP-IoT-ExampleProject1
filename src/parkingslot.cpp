#include "Arduino.h"
#include "parkingslot.h"

#define BLINK_PERIOD_ON 300
#define BLINK_PERIOD_OFF 900

#define MAX_MESSAGE_SIZE 1024
#define MAX_JSON_SIZE 512

#define REPORT_PERIOD_SEC 20

static const int _blink1_list[] = {BLINK_PERIOD_ON,-(BLINK_PERIOD_OFF),0};
static const int _blink2_list[] = {BLINK_PERIOD_OFF, -(BLINK_PERIOD_ON) ,0};

static void job_callback(void * data) {
    ((ParkingSlot*)data)->create_send_report();
}

void ParkingSlot::init(char *device_id, char *enc_key, bool enc_en, int led_pin, int sensor_io,
              int sensor_low_thershold, int sensor_high_threshold)
{
    this->device_id = device_id;
    sensor.init(sensor_io, sensor_low_thershold, sensor_high_threshold);
    blinker.init(led_pin);
    set_led(OFF);
    enc.set_key((const unsigned char*)enc_key, enc_en);


    snprintf(platform_topic,sizeof(platform_topic),"/%s/p2d",device_id);
    led_update_time = millis();

    // ToDo: Change default value of the report period rate
    report_job.init(REPORT_PERIOD_SEC, job_callback, (void*)this);
    report_job.register_execute();
}


void ParkingSlot::set_mqtt_publish_access(mqtt_client_pub_t pub_func) {
    this->pub_func = pub_func;
}

void ParkingSlot::set_led(LedState state)
{
    led_state = state;
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
    report_job.handle();

    // // This is test code:
    // int change = sensor.get_change_state();

    // if (change>0) {
    //     pub_func("D2P_topic_HI", (const char *) device_id);
    //     //pub_func("D2P_topic", "{\"state\"=\"HI\"}");

    // } else if (change<0) {
    //     pub_func("D2P_topic_LOW", (const char *) device_id);
    //     //pub_func("D2P_topic", "{\"state\"=\"HI\"}");
    // }
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
    JsonObject data = obj["data"][0];

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

    if (!strcmp(key, "led")) {
        cmd_led(cmd.value());
    } 

}

void ParkingSlot::cmd_led(const char * cmd) {
    if (cmd) {
        LedState state;
        bool change = false;
        if (!strcmp(cmd, "G")) {
            state = ON;
            change = true;
        } else if (!strcmp(cmd,"R")) {
            state = OFF;
            change = true;
        } else if (!strcmp(cmd,"B1")) {
            state = BLINK1;
            change = true;
        } else if (!strcmp(cmd,"B2")) {
            state = BLINK2;
            change = true;
        }

        if (change) {
            set_led(state);
            report_job.register_execute();
            led_update_time = millis();
        }
    } 
}


const char * ParkingSlot::get_str_led_state() {
    switch (led_state)
    {
        case ON:
            return "G";
        case OFF:
            return "R";
        case BLINK1:
            return "B1";
        case BLINK2:
            return "B2";
        default:
            break;
    }
    return "X";
}


bool ParkingSlot::send_current_state_to_platform() {
    StaticJsonDocument<MAX_JSON_SIZE> doc;
    char buffer[MAX_MESSAGE_SIZE];

    JsonArray data_array = doc.createNestedArray("data");
    
    JsonObject data = data_array.createNestedObject();

    data["led"] = get_str_led_state();
    data["led_last_update"] = led_update_time;

    serializeJson(doc, buffer, sizeof(buffer));

    // ToDo: Add packet encryption here!

    char topic[MAX_TOPIC_LEN];
    snprintf(topic,sizeof(topic),"/%s/d2p",device_id);


    return pub_func(topic, buffer);
}


void ParkingSlot::create_send_report() {
    send_current_state_to_platform();
}



