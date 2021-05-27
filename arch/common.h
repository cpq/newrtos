// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdint.h>
#include <string.h>

#include "rtos.h"

void rtos_heap_init(void *start, void *end);  // Initialise malloc

#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))

static inline void init_ram(void) {
  extern uint32_t _sbss, _ebss;
  extern uint32_t _sdata, _edata, _sidata;
  memset(&_sbss, 0, (size_t)(((char *) &_ebss - (char *) &_sbss)));
  memcpy(&_sdata, &_sidata, (size_t)(((char *) &_edata - (char *) &_sdata)));

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

static inline void Set_NVIC_Prio(int irq, uint32_t prio) {
  NVIC->IP[irq] = prio << 4;
}

static inline uint32_t SysTick_Config(uint32_t ticks) {
  if ((ticks - 1) > 0xffffff) return 1;  // Systick timer is 24 bit
  SysTick->LOAD = ticks - 1;
  SysTick->VAL = 0;
  SysTick->CTRL = BIT(0) | BIT(1) | BIT(2);
  return 0;
}

struct flash {
  volatile uint32_t ACR, KEYR, OPTKEYR, SR, CR, AR, RESERVED, OBR, WRPR;
};
#define FLASH ((struct flash *) 0x40022000)

enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };
enum { GPIO_OTYPE_PUSH_PULL, GPIO_OTYPE_OPEN_DRAIN };
enum { GPIO_SPEED_LOW, GPIO_SPEED_MEDIUM, GPIO_SPEED_HIGH, GPIO_SPEED_INSANE };
enum { GPIO_PULL_NONE, GPIO_PULL_UP, GPIO_PULL_DOWN };
