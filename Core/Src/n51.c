#include "n51.h"
#include <stdbool.h>

void send_cmd(uint8_t cmd) {
  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_SET);
}

void send_data(uint8_t data) {
  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
  HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_SET);
}

void send_char(uint8_t c) {
  if(c >= 0x20 && c <= 0x80) {
    for (int i = 0; i <5; i++){
      send_data(Symbols[c - 0x20][i]);
    }
    send_data(0x00);
  }
}

void clear () {
  for (int i = 0; i < 504; i++) {
    send_data(0x00);
  }
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
