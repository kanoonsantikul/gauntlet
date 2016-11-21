#include <avr/interrupt.h> 

#include "timer.h"

#define TCNT_START  6

static volatile uint32_t millis;

void timer_init()
{
    TCCR0A = 0x00;

    // Clock source = ClkIO/64
    // => 250 KHz
    TCCR0B = (0<<CS02)|(1<<CS01)|(1<<CS00);

    // Enable Timer0 overflow interrupt
    TIMSK0 = (1<<TOIE0);

    // Start counting with desired overflow interval
    // so that it will overflow (>255) in 250 ticks
    TCNT0 = TCNT_START;
}

uint32_t timer_millis()
{
    uint16_t t;
	uint8_t saved_sreg = SREG;

    cli();
    t = millis;
    SREG = saved_sreg;

    return t;
}

ISR(TIMER0_OVF_vect)
{
    millis++;
    TCNT0 = TCNT_START;
}
