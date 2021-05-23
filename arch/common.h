// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdint.h>
#include <string.h>

#include "rtos.h"

void rtos_heap_init(void *start, void *end);  // Initialise malloc
int rtos_heap_used(void);                     // Return used heap size
int rtos_heap_available(void);                // Return available heap size
int rtos_scheduler_init(void);                // Initialise scheduler

#define BIT(x) ((uint32_t) 1 << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))

static inline void init_ram(void) {
  extern uint32_t _sbss, _ebss;
  extern uint32_t _sdata, _edata, _sidata;
  memset(&_sbss, 0, ((char *) &_ebss - (char *) &_sbss));
  memcpy(&_sdata, &_sidata, ((char *) &_edata - (char *) &_sdata));

  extern uint32_t _end, _estack;
  rtos_heap_init(&_end, &_estack);
}

static inline void spin(volatile uint32_t count) {
  while (count--) asm("nop");
}
