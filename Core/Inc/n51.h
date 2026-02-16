#ifndef __N51_H__
#define __N51_H__

#include "stdint.h"
#include "stdbool.h"
#include "n51_font.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"

extern SPI_HandleTypeDef hspi1;

void send_cmd(uint8_t cmd);

void send_data(uint8_t data) ;

void send_char(uint8_t c) ;

void clear ();

void init();

void reset();

void set_cursor(uint8_t x, uint8_t y);

void print_string(const char* str);

void n51_backlight(bool state);


#endif // __N51_H__