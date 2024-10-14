#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <stdio.h>

float get_temperature(void);
float get_pressure(void);
float get_gas_resistance(void);
float get_humidity(void);

void *monitor_sensors(void *arg);
int init_sensors(void);
void close_sensors_fd(void);
