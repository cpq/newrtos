// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdint.h>
#include <string.h>

#include "rtos.h"

void rtos_heap_init(void *start, void *end);  // Initialise malloc

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

struct systick {
  volatile uint32_t CTRL, LOAD, VAL, CALIB;
};
#define SysTick ((struct systick *) 0xe000e010)

struct nvic {
  volatile uint32_t ISER[8], RESERVED0[24], ICER[8], RSERVED1[24], ISPR[8],
      RESERVED2[24], ICPR[8], RESERVED3[24], IABR[8], RESERVED4[56], IP[240],
      RESERVED5[644], STIR;
} NVIC_Type;
#define NVIC ((struct nvic *) 0xe000e100)

struct scb {
  volatile uint32_t CPUID, ICSR, VTOR, AIRCR, SCR, CCR;
  volatile uint8_t SHP[12];
  volatile uint32_t SHCSR, CFSR, HFSR, DFSR, MMFAR, BFAR, AFSR, PFR[2], DFR,
      ADR, MMFR[4], ISAR[5], RESERVED0[5], CPACR;
};
#define SCB ((struct scb *) 0xe000ed00)

static inline void NVIC_SetPriority(int irq, uint32_t prio) {
  if (irq < 0) {
    SCB->SHP[(((uint32_t) irq) & 15) - 4] = prio << 4;
  } else {
    NVIC->IP[irq] = prio << 4;
  }
}

static inline uint32_t SysTick_Config(uint32_t ticks) {
  if ((ticks - 1) > 0xffffff) return 1;  // Systick timer is 24 bit
  SysTick->LOAD = ticks - 1;
  NVIC_SetPriority(-1, 15);
  SysTick->VAL = 0;
  SysTick->CTRL = BIT(0) | BIT(1) | BIT(2);
  return 0;
}
