#include "parkinggate.h"

#define BLINK_PERIOD_ON 300
#define BLINK_PERIOD_OFF 900

#define REPORT_PERIOD_SEC 30


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

    gate.init(servo_io, gate_low, gate_high);
    gate.close();

    send_full_report = true;
    gate_open_time = 10;
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
    gate.close();
    report_job.register_execute();
}

void ParkingGate::apply_key_value_cmd(JsonPair cmd) {
    // ToDo:
    auto key = cmd.key();

    int need_report = 2;


    if (key == "led") {
        cmd_led(cmd.value());
        need_report = 1;
    } else if (key == "gate") {
        cmd_gate(cmd.value());
        need_report = 1;
    } else if (key == "sensor_threshold_low") {
        int value = cmd.value();
        if (value<1 || value>4000) {
            value = entry_sensor.get_threshold_low();
        }
        exit_sensor.set_threshold_low(value);
        entry_sensor.set_threshold_low(value);
    } else if (key == "sensor_threshold_high") {
        int value = cmd.value();
        if (value<80 || value>4000) {
            value = entry_sensor.get_threshold_high();
        }
        exit_sensor.set_threshold_high(value);
        entry_sensor.set_threshold_high(value);
    } else if (key == "gate_open_time") {
        int value = cmd.value();
        if (value<3 || value>60) {
            value = gate_open_time;
        }
        gate_open_time = value;
    } else if (key == "gate_low") {
        int value = cmd.value();
        if (value>=5 && value<=175) {
            gate.close_value = value;
        }
    } else if (key == "gate_high") {
        int value = cmd.value();
        if (value>=5 && value<=175) {
            gate.open_value = value;
        }
    } else if (key == "report_period") {
        unsigned int period = cmd.value();
        if (period>(12*3600)) {
            // Don't allow update period bigger than 12h
            period = report_job.get_period();
        }

        report_job.set_period(period);
    } else {
        need_report = 0;
    }

    if (need_report) {
        if (need_report==2) {
            send_full_report = true;
        }
        report_job.register_execute();
    }
}

bool ParkingGate::send_periodic_report() {

    field_data_t fields[] = {
        {"led", field_data_t::STR, (void*)get_str_led_state()},
        {"led_last_update", field_data_t::NUM, (void*)(led_update_time/1000)},
        {"entry_sensor_last_update", field_data_t::NUM, (void*)(entry_sensor_changed_time/1000)},
        {"exit_sensor_last_update", field_data_t::NUM, (void*)(exit_sensor_changed_time/1000)},
        {"entry_sensor_value", field_data_t::NUM, (void*)entry_sensor.get_last_sensor_value()},
        {"exit_sensor_value", field_data_t::NUM, (void*)exit_sensor.get_last_sensor_value()},
        {"entry_sensor_state", field_data_t::NUM, (void*)entry_sensor.get_current_state()},
        {"exit_sensor_state", field_data_t::NUM, (void*)exit_sensor.get_current_state()},
        {"gate", field_data_t::STR, (void*)get_str_gate_state()},
        {"gate_last_update", field_data_t::NUM, (void*)(gate_update_time/1000)}, // READ NOTE -vv
        // NOTE: Don't change location of the "gate_last_update" in list
        // NOTE: parameters bellow "gate_last_update" will be reported only in full mode
        
        {"sensor_threshold_low", field_data_t::NUM, (void*)entry_sensor.get_threshold_low()},
        {"sensor_threshold_high", field_data_t::NUM, (void*)entry_sensor.get_threshold_high()},
        {"gate_open_time", field_data_t::NUM, (void*)gate_open_time},
        {"gate_low", field_data_t::NUM, (void*)gate.close_value},
        {"gate_high", field_data_t::NUM, (void*)gate.open_value},
        {"report_period", field_data_t::NUM, (void*)report_job.get_period()},
        {0}
    };

    if (send_full_report) {
        send_full_report = false;
    } else {
        int i=0;
        while (fields[i].name) {
            if (!strcmp(fields[i].name, "gate_last_update")) {
                i++;
                fields[i].name = 0;
                break;
            }
            i++;
        }
    }

    return device.send_to_platform(fields, millis()/1000);
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

void ParkingGate::cmd_led(const char * cmd) {
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

void ParkingGate::cmd_gate(const char * cmd) {
    if (cmd) {
        int update_time = millis();

        if (!strcmp(cmd, "OPEN")) {
            gate.open();
        } else if (!strcmp(cmd,"CLOSE")) {
            gate.close();
        } else if (!strcmp(cmd,"OPEN-CLOSE")) {
            gate.open();
            closegate_job.run(gate_open_time*1000);
        } else {
            update_time = gate_update_time;
        }
        gate_update_time = update_time;
    } 
} 

const char * ParkingGate::get_str_led_state() {
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

const char * ParkingGate::get_str_gate_state() {
    if (gate.state) {
        return "OPEN";
    } else {
        return "CLOSE";
    }
}

