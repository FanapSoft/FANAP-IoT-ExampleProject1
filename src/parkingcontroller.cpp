#include "parkingcontroller.h"
#include <string.h>



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
