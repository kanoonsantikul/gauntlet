#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include <avr/io.h>

void timer_init();
uint32_t timer_millis();

#endif
