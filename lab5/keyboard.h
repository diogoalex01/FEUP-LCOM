#pragma once

//global variables
extern uint32_t status; //global variable that contains the scan of the buffer
extern uint32_t stat;
extern uint32_t counter;
extern int timer_counter;
extern bool error2;

//prototypes
int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

uint8_t kbd_int_handler();

int kbd_scan_poll();

int kbd_poll_cmd();

void kbd_poll();
