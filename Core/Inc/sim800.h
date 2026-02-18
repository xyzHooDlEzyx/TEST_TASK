#ifndef SIM800_H
#define SIM800_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include <string.h>
#include <stdio.h>

#define SIM800C_BUFFER_SIZE 1024
#define SIM800C_TIMEOUT_MS 500

void sim800_init(void);
bool sim800_is_ready(void);
void sim800_send_command(const char* command, uint32_t timeout_ms);
void sim800_get_weather(const char* city);
char* sim800_read_response(uint32_t timeout_ms);

#endif // SIM800_H