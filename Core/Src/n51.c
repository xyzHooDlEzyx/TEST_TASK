#include "n51.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void send_cmd(uint8_t cmd) {
  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_SET);
}

void send_data_buffer(const uint8_t *data, uint16_t len) {
  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi1, (uint8_t *)data, len, 1000);
  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_SET);
}

void send_char(uint8_t c) {
  if(c >= 0x20 && c <= 0x80) {
    uint8_t buffer[6];
    const uint8_t *tmp = Symbols[c - 0x20];
    for (int i = 0; i < 5; i++){
      buffer[i] = tmp[i];
    }
    buffer[5] = 0x00;
    send_data_buffer(buffer, 6);
  }
}

void clear () {
  static const uint8_t blank[504] = {0};
  send_data_buffer(blank, 504);
}

void init(){
  send_cmd(0x21); // Extended instruction set
  send_cmd(0xA0); // Set Vop (contrast)
  send_cmd(0x04); // Set temp coefficient
  send_cmd(0x14); // LCD bias mode 1:48
  send_cmd(0x20); // Basic instruction set
  send_cmd(0x0C); // Normal display mode
  n51_backlight(false);
}

void reset(){
  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
}

void set_cursor(uint8_t x, uint8_t y) {
  send_cmd(0x80 | x);
  send_cmd(0x40 | y);
}

void print_string(const char* str) {
  while (*str) {
    send_char(*str++);
  }
}

void n51_backlight(bool state) {
  if(state){
    HAL_GPIO_WritePin(LIGHT_GPIO_Port, LIGHT_Pin, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(LIGHT_GPIO_Port, LIGHT_Pin, GPIO_PIN_RESET);
  }
}
