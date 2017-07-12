#ifndef TIMER_H
#define TIMER_H

#define TIMER_IRQ 0x00
#define PIT_CHANNEL0 0x40 //PIT channel 0's data register port
#define PIT_CHANNEL1 0x41 //PIT channels 1's data register port, 
#define PIT_CHANNEL2 0x42 //PIT channels 2's data register port,
#define PIT_CMDREG   0x43 //PIT chip's command register port.
#define MAX_FREQ_PIT	1193180
#include "i8259.h"
#include "lib.h"
 /*
 * low-level interface to configure the PIT
 *
 * channel
 * - 0: system timer
 * - 2: speaker
 *  mode
 * - 2: rate generator (for periodic interrupts)
 * - 3: square wave (for audio signals)
 * frequency a number between 19 and 1193182 Hz
 * return 0 if parameters were correct, -1 on error
*/
void init_timer(void);

void handle_tick(void);

int32_t timer_set(int hz);
int32_t pit_read();
void speaker_set(int hz);
#endif
