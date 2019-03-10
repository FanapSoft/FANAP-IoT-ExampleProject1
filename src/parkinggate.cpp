#include "parkinggate.h"

#define BLINK_PERIOD_ON 300
#define BLINK_PERIOD_OFF 900

#define REPORT_PERIOD_SEC 5


static const int _blink1_list[] = {BLINK_PERIOD_ON,-(BLINK_PERIOD_OFF),0};
static const int _blink2_list[] = {BLINK_PERIOD_OFF, -(BLINK_PERIOD_ON) ,0};


static void job_callback(void * data) {
    ((ParkingGate*)data)->send_periodic_report();
}

static void cmd_callback(JsonPair field, void *data) {
    ((ParkingGate*)data)->apply_key_value_cmd(field);
}

static void closegate_callback(void * data) {
    ((ParkingGate*)data)->closegate();
}

void ParkingGate::init(char *device_id, char *enc_key, bool enc_en,
                       int led_pin, int entry_sensor_io, int exit_sensor_io, int servo_io,
                       int sensor_low_threshold, int sensor_high_threshold,
                       int gate_low, int gate_high)
{
    device.init(device_id, enc_key, enc_en);

    entry_sensor.init(entry_sensor_io, sensor_low_threshold, sensor_high_threshold);
    exit_sensor.init(exit_sensor_io, sensor_low_threshold, sensor_high_threshold);

    blinker.init(led_pin);
    set_led(OFF);
    led_update_time = millis();

    // ToDo: Change default value of the report period rate
    report_job.init(REPORT_PERIOD_SEC, job_callback, (void*)this);
    report_job.register_execute();

    entry_sensor_changed_time = millis();
    exit_sensor_changed_time = millis();

    closegate_job.init(closegate_callback, (void*)this);

}



void ParkingGate::set_led(LedState state)
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

void ParkingGate::set_mqtt_publish_access(mqtt_client_pub_t pub_func) {

    device.set_callbacks(pub_func, cmd_callback, (void*)this);
}

void ParkingGate::closegate() {
    // ToDo:

}

void ParkingGate::apply_key_value_cmd(JsonPair cmd) {
    // ToDo:
    Serial.printf("Gate: %s\n", cmd.key().c_str());
}

void ParkingGate::send_periodic_report() {
    // ToDo:
}

void ParkingGate::handle() {
    blinker.handle();
    exit_sensor.handle();
    entry_sensor.handle();


    // Report state if sensor is changed:
    int change = entry_sensor.get_change_state();
    if (change) {
        report_job.register_execute();
        entry_sensor_changed_time = millis();
    }

    change = exit_sensor.get_change_state();
    if (change) {
        report_job.register_execute();
        exit_sensor_changed_time = millis();
    }


    // Report handler should last handler
    closegate_job.handle();
    report_job.handle();
}

bool ParkingGate::process_received_message(char * topic, char * payload, int msg_size) { 
    return device.process_received_message(topic, payload, msg_size);
}
