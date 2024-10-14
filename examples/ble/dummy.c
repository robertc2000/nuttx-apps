#include "sensors.h"

float get_temperature(void) {
    return 40;
}

float get_pressure(void) {
    return 1000;
}

float get_gas_resistance(void) {
    return 100;
}

float get_humidity(void) {
    return 30;
}

void *monitor_sensors(void *arg) {
    (void) arg;
    while(1) (void) 0;
    return NULL;
}

int init_sensors(void) {
    return 0;
}

void close_sensors_fd(void) {
    return;
}