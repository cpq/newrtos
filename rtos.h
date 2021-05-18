// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#pragma once

void rtos_heap_init(void *start, void *end);  // Initialise malloc
int rtos_heap_used(void);                     // Return used heap size
int rtos_heap_available(void);                // Return available heap size

void rtos_task_create(void (*fn)(void *), void *data, int stksize, int prio);
void rtos_schedule(void);  // Run scheduler, never return

unsigned long rtos_msleep(unsigned long milliseconds);  // Sleep, return systick
