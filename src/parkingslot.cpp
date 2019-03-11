#include "Arduino.h"
#include "parkingslot.h"

#define BLINK_PERIOD_ON 300
#define BLINK_PERIOD_OFF 900

#define REPORT_PERIOD_SEC 5

static const int _blink1_list[] = {BLINK_PERIOD_ON,-(BLINK_PERIOD_OFF),0};
static const int _blink2_list[] = {BLINK_PERIOD_OFF, -(BLINK_PERIOD_ON) ,0};

static void job_callback(void * data) {
    ((ParkingSlot*)data)->create_send_report();
}

static void cmd_callback(JsonPair field, void *data) {
    ((ParkingSlot*)data)->apply_key_value_cmd(field);
}


void ParkingSlot::init(char *device_id, char *enc_key, bool enc_en, int led_pin, int sensor_io,
              int sensor_low_thershold, int sensor_high_threshold)
{
    device.init(device_id, enc_key, enc_en);
    sensor.init(sensor_io, sensor_low_thershold, sensor_high_threshold);
    blinker.init(led_pin);
    set_led(OFF);
    led_update_time = millis();

    // ToDo: Change default value of the report period rate
    report_job.init(REPORT_PERIOD_SEC, job_callback, (void*)this);
    report_job.register_execute();

    sensor_changed_time = millis();
}


void ParkingSlot::set_mqtt_publish_access(mqtt_client_pub_t pub_func) {

    device.set_callbacks(pub_func, cmd_callback, (void*)this);
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
    


    // Report state if sensor is changed:
    int change = sensor.get_change_state();
    if (change) {
        report_job.register_execute();
        sensor_changed_time = millis();
    }

    // Report handler should last handler
    report_job.handle();
}

bool ParkingSlot::process_received_message(char * topic, char * payload, int msg_size) { 
    return device.process_received_message(topic, payload, msg_size);
}

void ParkingSlot::apply_key_value_cmd(JsonPair cmd) {
    const char * key = cmd.key().c_str();

    if (!strcmp(key, "led")) {
        cmd_led(cmd.value());
    } else if (!strcmp(key, "report_period")) {
        unsigned int period = cmd.value();
        if (period>(12*3600)) {
            // Don't allow update period bigger than 12h
            period = report_job.get_period();
        }

        report_job.set_period(period);
        report_job.register_execute();
    } else if (!strcmp(key, "sensor_threshold_low")) {
        int value = cmd.value();
        if (value<1 || value>4000) {
            value = sensor.get_threshold_low();
        }
        sensor.set_threshold_low(value);
        report_job.register_execute();
    } else if (!strcmp(key, "sensor_threshold_high")) {
        int value = cmd.value();
        if (value<80 || value>4000) {
            value = sensor.get_threshold_high();
        }
        sensor.set_threshold_high(value);
        report_job.register_execute();
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


    field_data_t fields[] = {
        {"led", field_data_t::STR, (void*)get_str_led_state()},
        {"led_last_update", field_data_t::NUM, (void*)(led_update_time/1000)},
        {"sensor_state", field_data_t::NUM, (void*)sensor.get_current_state()},
        {"report_period", field_data_t::NUM, (void*)report_job.get_period()},
        {"sensor_last_update", field_data_t::NUM, (void*)(sensor_changed_time/1000)},
        {"sensor_threshold_low", field_data_t::NUM, (void*)sensor.get_threshold_low()},
        {"sensor_threshold_high", field_data_t::NUM, (void*)sensor.get_threshold_high()},
        {"sensor_value", field_data_t::NUM, (void*)sensor.get_last_sensor_value()},
        {0}
    };
    return device.send_to_platform(fields, millis()/1000);
}


void ParkingSlot::create_send_report() {
    send_current_state_to_platform();
}



