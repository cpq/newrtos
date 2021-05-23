// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#pragma once

int rtos_task_create(void (*)(void *), void *, int);
void rtos_schedule(void);                     // Run scheduler, never return
unsigned long rtos_msleep(unsigned long ms);  // Sleep millis, return systick
