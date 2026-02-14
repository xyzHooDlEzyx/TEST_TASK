#ifndef DISPLAY_HELPER_H
#define DISPLAY_HELPER_H

#include <stdint.h>
#include <stdbool.h>

void displayWeatherInfo(float temperature, float windspeed, int weathercode, int is_day);
const char* getWeatherDesc(int code);
const unsigned char* getWeatherIcon(int code);

#endif