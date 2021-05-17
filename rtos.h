// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#pragma once

void rtos_heap_init(void *ptr, size_t size);  // Initialise malloc
size_t rtos_heap_used(void);                  // Return used heap size
size_t rtos_heap_available(void);             // Return available heap size

void rtos_task_create(void (*fn)(void *), void *data, size_t stksize, int prio);
void rtos_schedule(void);
