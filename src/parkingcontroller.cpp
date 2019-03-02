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

    char buffer[201];
    
    int cpy_size = size;

    if (cpy_size>200) {
        cpy_size=200;
    }

    strncpy(buffer, (const char *)payload,cpy_size);

    Serial.printf("Topic:%s msg:%s len:%d\n",topic, buffer, size);
}


void ParkingController::mqtt_subscribe(bool(*subscribe)(const char*)) {
    char topic_name[MAX_TOPIC_SIZE];
    for(int i=0; i<NumSlots; i++) {
        slots[i].from_platform_topic(topic_name);
        bool res = subscribe(topic_name);
        if (!res) {
            Serial.printf("Failed to subscribe to \"%s\"\n", topic_name);
        }
    }
}


void ParkingController::mqtt_publish(bool(*publish)(const char* topic, const char* msg)) {
    for(int i=0; i<NumSlots; i++) {
        slots[i].set_mqtt_publish_access(publish);
    }
}

