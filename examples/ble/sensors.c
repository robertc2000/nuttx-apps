#include <nuttx/sensors/sensor.h>
#include <nuttx/sensors/bme680.h>

#include "sensors.h"

#define NB_LOWERHALFS 3

struct pollfd pfds[3];
int baro_fd, hum_fd, gas_fd;

struct sensor_baro baro_data;
struct sensor_humi humi_data;
struct sensor_gas gas_data;

float get_temperature(void) {
  return baro_data.temperature;
}

float get_pressure(void) {
  return baro_data.pressure;
}

float get_gas_resistance(void) {
  return gas_data.gas_resistance;
}

float get_humidity(void) {
  return humi_data.humidity;
}

static void read_barometer(void) {
  int ret = read(baro_fd, &baro_data, sizeof(struct sensor_baro));
  if (ret != sizeof(struct sensor_baro)) {
    perror("Could not read temperature");
  }
  return;
}

static void read_humidity(void) {
  int ret = read(hum_fd, &humi_data, sizeof(struct sensor_humi));
  if (ret != sizeof(struct sensor_humi)) {
    perror("Could not read humidity");
  }
  return;
}

static void read_gas(void) {
  int ret = read(gas_fd, &gas_data, sizeof(struct sensor_gas));
  if (ret != sizeof(struct sensor_gas)) {
    perror("Could not read gas resistance");
  }
}

void *monitor_sensors(void *arg) {
  while (1) {
    int ret = poll(pfds, NB_LOWERHALFS, -1);
    if (ret < 0)
    {
      perror("Could not poll sensor.");
      return NULL;
    }
    read_barometer();
    read_humidity();
    read_gas();

    printf("Temperature [C] = %f\n"
      "Pressure [hPa] = %f\n"
      "Humidity [rH] = %f\n"
      "Gas resistance [kOhm] = %f\n",
      baro_data.temperature, baro_data.pressure,
      humi_data.humidity, gas_data.gas_resistance);

    sleep(1);
  }
  return NULL;
}

int init_sensors(void) {
  baro_fd = open("/dev/uorb/sensor_baro0", O_RDONLY | O_NONBLOCK);
  if (baro_fd < 0) {
    printf("Failed to open barometer lowerhalf.\n");
    return -1;
  }

  hum_fd = open("/dev/uorb/sensor_humi0", O_RDONLY | O_NONBLOCK);
  if (hum_fd < 0) {
    printf("Failed to open humidity sensor lowerhalf.\n");
    return -1;
  }

  gas_fd = open("/dev/uorb/sensor_gas0", O_RDONLY | O_NONBLOCK);
  if (gas_fd < 0) {
    printf("Failed to open gas lowerhalf.\n");
    return -1;
  }

  pfds[0].fd = baro_fd, pfds[1].fd = hum_fd, pfds[2].fd = gas_fd;
  pfds[0].events = pfds[1].events = pfds[2].events = POLLIN;

  struct bme680_config_s config;
  config.temp_os = BME680_OS_2X;
  config.press_os = BME680_OS_16X;
  config.filter_coef = BME680_FILTER_COEF3;
  config.hum_os = BME680_OS_1X;
  config.target_temp = 300;
  config.amb_temp = 30;
  config.heater_duration = 100;
  config.nb_conv = 0;

  int ret = ioctl(baro_fd, SNIOC_CALIBRATE, &config);
  if (ret < 0) {
    perror("Failed to configure sensor");
    return -1;
  }
  return 0;
}

void close_sensors_fd(void) {
  close(baro_fd);
  close(hum_fd);
  close(gas_fd);
}