#include "display_helper.h"
#include "main.h"
#include "n51.h"
#include <stdio.h>

extern volatile City current_city;
extern int year, month, day_n, hour, minute;
extern float temperature;
extern float windspeed;
extern int weathercode;
extern int is_day;

void updateDisplay() {
  char temp_str[35];


  clear();
  set_cursor(0, 0);
  print_string("City:");

  set_cursor(30, 0);
  print_string(current_city == Lviv ? "LVIV" : "KYIV");

  sprintf(temp_str, "%02d:%02d", hour, minute);
  set_cursor(0, 1);
  print_string(temp_str);
  set_cursor(33, 1);
  print_string(is_day ? "DAY" : "NIGHT");
  set_cursor(0, 2);
  sprintf(temp_str, "%02d-%02d-%04d", day_n, month, year);
  print_string(temp_str);



  int t_int = (int)temperature;
  int t_dec = (int)((temperature - (float)t_int) * 10.0f);
  if (t_dec < 0) t_dec = -t_dec; 


  sprintf(temp_str, "Temp:%d.%dC", t_int, t_dec);
  set_cursor(0, 3);
  print_string(temp_str);

  int w_int = (int)windspeed;
  int w_dec = (int)((windspeed - (float)w_int) * 10.0f);
  if (w_dec < 0) w_dec = -w_dec;

  sprintf(temp_str, "Wind:%d.%dkm/h", w_int, w_dec);
  set_cursor(0, 4);
  print_string(temp_str);

  set_cursor(0, 5);
  sprintf(temp_str, "%s", getWeatherDesc(weathercode));
  print_string(temp_str);

}

void error_display() {
  clear();
  set_cursor(0, 0);
  print_string("Error fetching");
  set_cursor(0, 1);
  print_string("weather data");
  set_cursor(0, 2);
  print_string("Try resetting the device");
}

const char* getWeatherDesc(int code) {  
  if (code == 0) return "Clear";
  if (code >= 1 && code <= 3) return "Cloudy";
  if (code >= 45 && code <= 48) return "Foggy";
  if (code >= 51 && code <= 55) return "Drizzle";
  if (code >= 61 && code <= 65) return "Rainy";
  if (code >= 71 && code <= 77) return "Snowy";
  if (code >= 95) return "Thunderstorm";
  return "Unknown";
}
