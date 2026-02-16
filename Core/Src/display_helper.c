#include "display_helper.h"
#include "icon.h"
#include "main.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

extern volatile City current_city;
extern int year, month, day_n, hour, minute;
extern float temperature;
extern float windspeed;
extern int weathercode;
extern int is_day;

void updateDisplay() {
  char temp_str[35];


  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("City:", Font_7x10, White);
  
  ssd1306_SetCursor(35, 0);
  ssd1306_WriteString(current_city == Lviv ? "LVIV" : "KYIV", Font_7x10, White);

  ssd1306_DrawBitmap(104, 0, is_day ? day : night, 24, 24, White);
  ssd1306_DrawBitmap(104, 30, (uint8_t*)getWeatherIcon(weathercode), 24, 24, White);

  sprintf(temp_str, "%02d:%02d", hour, minute);
  ssd1306_SetCursor(0, 10);
  ssd1306_WriteString(temp_str, Font_11x18, White);
  ssd1306_SetCursor(60, 14);
  sprintf(temp_str, "%02d-%02d", day_n, month);
  ssd1306_WriteString(temp_str, Font_7x10, White);



  int t_int = (int)temperature;
  int t_dec = (int)((temperature - (float)t_int) * 10.0f);
  if (t_dec < 0) t_dec = -t_dec; 


  sprintf(temp_str, "Temp:%d.%d C", t_int, t_dec);
  ssd1306_SetCursor(0, 28);
  ssd1306_WriteString(temp_str, Font_7x10, White);

  int w_int = (int)windspeed;
  int w_dec = (int)((windspeed - (float)w_int) * 10.0f);
  if (w_dec < 0) w_dec = -w_dec;

  sprintf(temp_str, "Wind:%d.%d km/h", w_int, w_dec);
  ssd1306_SetCursor(0, 38);
  ssd1306_WriteString(temp_str, Font_7x10, White);

  ssd1306_SetCursor(0, 48);
  sprintf(temp_str, "%s", getWeatherDesc(weathercode));
  ssd1306_WriteString(temp_str, Font_7x10, White);

  ssd1306_UpdateScreen();
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

const unsigned char* getWeatherIcon(int code) {
  if (code == 0) return clearSky;
  if (code >= 1 && code <= 3) return cloud;
  if (code >= 45 && code <= 48) return fog;
  if (code >= 51 && code <= 55) return drizzle;
  if (code >= 61 && code <= 65) return rain;
  if (code >= 71 && code <= 77) return snow;
  if (code >= 95) return thunderstorm;
  return cloud;
}