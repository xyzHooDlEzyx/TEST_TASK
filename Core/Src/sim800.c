#include "sim800.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart1;
extern char rx_line[512];

static bool sim800_get_city_coords(const char* city, const char** lat, const char** lon) {
  if (!city || !lat || !lon) {
    return false;
  }

  if (strcmp(city, "Lviv") == 0 || strcmp(city, "LVIV") == 0) {
    *lat = "49.84";
    *lon = "24.03";
    return true;
  }
  if (strcmp(city, "Kyiv") == 0 || strcmp(city, "KYIV") == 0 || strcmp(city, "Kiev") == 0) {
    *lat = "50.45";
    *lon = "30.52";
    return true;
  }

  return false;
}

void sim800_init() {
    sim800_send_command("AT", SIM800C_TIMEOUT_MS);
    HAL_Delay(100);
    sim800_send_command("AT", SIM800C_TIMEOUT_MS);
    HAL_Delay(100);
    sim800_send_command("ATE0", SIM800C_TIMEOUT_MS);
    HAL_Delay(100);
    sim800_send_command("AT+CPIN=\"9359\"", SIM800C_TIMEOUT_MS+4500);
}

void sim800_send_command(const char* command, uint32_t timeout_ms) {
  if (HAL_UART_Transmit(&huart1, (const uint8_t *)command, strlen(command), timeout_ms) != HAL_OK) {
    return; 
  }
  HAL_UART_Transmit(&huart1, (const uint8_t *)"\r\n", 2, 100); 
}

char* sim800_read_response(uint32_t timeout_ms) {
  static char buffer[SIM800C_BUFFER_SIZE];
  memset(buffer, 0, SIM800C_BUFFER_SIZE);
  uint32_t time_ms = HAL_GetTick();
  while (HAL_GetTick() - time_ms < timeout_ms) {
    if (HAL_UART_Receive(&huart1, (uint8_t *)buffer, SIM800C_BUFFER_SIZE, timeout_ms) == HAL_OK) {
      return buffer;
    }
  }
  return NULL;
}

void sim800_get_weather(const char* city) {
  sim800_send_command("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 1000);
  HAL_Delay(100);
  sim800_send_command("AT+SAPBR=3,1,\"APN\",\"internet\"", 1000); 
  HAL_Delay(100);
  sim800_send_command("AT+SAPBR=1,1", 1000);
  HAL_Delay(2000);

  sim800_send_command("AT+HTTPINIT", 1000);
  HAL_Delay(200);
  sim800_send_command("AT+HTTPPARA=\"CID\",1", 1000);
  HAL_Delay(100);

  {
    const char* lat = NULL;
    const char* lon = NULL;
    char command[220];
    if (!sim800_get_city_coords(city, &lat, &lon)) {
      lat = "49.84"; lon = "24.03";
    }
    snprintf(command, sizeof(command),
             "AT+HTTPPARA=\"URL\",\"http://api.open-meteo.com/v1/forecast?latitude=%s&longitude=%s&current_weather=true\"",
             lat, lon);
    sim800_send_command(command, 1000);
  }
  HAL_Delay(200);

  sim800_send_command("AT+HTTPACTION=0", 1000);
  HAL_Delay(5000);

  sim800_send_command("AT+HTTPREAD", 3000);
  HAL_Delay(2000); 

  sim800_send_command("AT+HTTPTERM", 1000);
  HAL_Delay(100);
  sim800_send_command("AT+SAPBR=0,1", 1000);
}

uint8_t wait_for_response(const char* resp, uint32_t timeout) {
    uint32_t start = HAL_GetTick();
    while (HAL_GetTick() - start < timeout) {
        if (strstr(rx_line, resp)) {
            memset(rx_line, 0, 512);
            return 1;
        }
        HAL_Delay(10);
    }
    return 0; // Таймаут
}