#include "parkingcontroller.h"

void ParkingController::initialize(
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
