// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#pragma once

int rtos_task_create(void (*)(void *), void *, int);  // Create RTOS task
void rtos_schedule(void);                     // Run scheduler, never return
int rtos_heap_used(void);                     // Return used heap size
int rtos_heap_available(void);                // Return available heap size
unsigned long rtos_msleep(unsigned long ms);  // Sleep millis, return systick
