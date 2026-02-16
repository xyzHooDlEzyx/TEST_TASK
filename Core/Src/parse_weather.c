#include "parse_weather.h"
#include <string.h>
#include <stdio.h>

extern int year, month, day_n, hour, minute;
extern float temperature;
extern float windspeed;
extern int weathercode;
extern int is_day;

extern void updateDisplay();
extern char rx_line[512];
extern uint16_t line_idx;

void parseWeatherData(const char* json) {

  const char* cw = strstr(json, "\"current_weather\":{");
  if (!cw) {
    return;
  }
  
  char *p;
  p = strstr(cw, "\"time\":");
  if (p) {
    sscanf(p, "\"time\":\"%d-%d-%dT%d:%d\"", &year, &month, &day_n, &hour, &minute);
    hour+=2;
    if (hour >= 24) {
      hour -= 24;
      day_n += 1;
    }
  }
  p = strstr(cw, "\"temperature\":");
  if (p) {
    sscanf(p, "\"temperature\": %f", &temperature);
  }
  p = strstr(cw, "\"windspeed\":");
  if (p) {
    sscanf(p, "\"windspeed\": %f", &windspeed);
  }

  p = strstr(cw, "\"weathercode\":");
  if (p) {
    sscanf(p, "\"weathercode\": %d", &weathercode);
  }

  p = strstr(cw, "\"is_day\":");
  if (p) {
    sscanf(p, "\"is_day\": %d", &is_day);
  }

  updateDisplay();
  memset(rx_line, 0, sizeof(rx_line));
  line_idx = 0;
}