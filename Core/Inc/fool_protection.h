#ifndef __FOOL_PROTECTION_H__
#define __FOOL_PROTECTION_H__
#include "main.h"

extern volatile uint8_t request_in_flight;
extern uint32_t request_started_at;

void button_irq_enable(void);

void button_irq_disable(void);

void weather_request_begin(void);

void weather_request_end(void);

#endif /* __FOOL_PROTECTION_H__ */