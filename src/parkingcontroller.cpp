#include "parkingcontroller.h"
#include <string.h>

#define MAX_TOPIC_SIZE 100

void ParkingController::init_slots(
    char *device_id_list[],
    char *enc_key_list[],
    bool *enc_en_list,
    int *led_pin_list,
    int *sensor_io_list,
    int *sensor_low_threshold_list,
    int *sensor_high_threshold_list)
{
    for (int i = 0; i < NumSlots; i++)
    {
        slots[i].init(
            device_id_list[i],
            enc_key_list[i],
            enc_en_list[i],
            led_pin_list[i],
            sensor_io_list[i],
            sensor_low_threshold_list[i],
            sensor_high_threshold_list[i]);
    }
}


void ParkingController::loop_handle() {
    for(int i = 0; i < NumSlots; i++)
    {
        slots[i].handle();
    }
    
}


void ParkingController::mqtt_callback(char* topic, uint8_t * payload, unsigned int size){
    for(int i = 0; i < NumSlots; i++) 
    {
        slots[i].process_received_message(topic, (char *)payload, size);
    }
}


void ParkingController::mqtt_subscribe(bool(*subscribe)(const char*)) {
    for(int i=0; i<NumSlots; i++) {
        bool res = subscribe(slots[i].device.from_platform_topic());
        if (!res) {
            Serial.printf("Failed to subscribe to \"%s\"\n", slots[i].device.from_platform_topic());
        }
    }
}


void ParkingController::mqtt_publish(bool(*publish)(const char* topic, const char* msg)) {
    for(int i=0; i<NumSlots; i++) {
        slots[i].set_mqtt_publish_access(publish);
    }
}

