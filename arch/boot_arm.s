.cpu cortex-m4
.thumb

.section .vectortab,"a",%progbits
// Cortex-M standard interrupt handlers
.word   _estack              // 0 Stack top address
.word   _reset               // 1 Reset
.word   pass                 // 2 NMI
.word   halt                 // 3 Hard Fault
.word   halt                 // 4 MM Fault
.word   halt                 // 5 Bus Fault
.word   halt                 // 6 Usage Fault
.word   0                    // 7 RESERVED
.word   0                    // 8 RESERVED
.word   0                    // 9 RESERVED
.word   0                    // 10 RESERVED
.word   halt                 // 11 SV call
.word   halt                 // 12 Debug reserved
.word   0                    // 13 RESERVED
.word   halt                 // 14 PendSV
.word   SysTick_Handler      // 15 SysTick
// STM32 specific handlers
.word   halt,halt,halt,halt,halt,halt,halt,halt,halt,halt
.word   halt,halt,halt,halt,halt,halt,halt,halt,halt,halt
.word   halt,halt,halt,halt,halt,halt,halt,halt,halt,halt
.word   halt,halt,halt,halt,halt,halt,halt,halt,halt,halt
.word   halt,halt,halt,halt,halt,halt,halt,halt,halt,halt

.section .text
.thumb_func
.global _reset
_reset:
  bl main   // Jump to main()
  b .       // If main() returns - just halt here

.thumb_func
halt:   b halt

.thumb_func
pass:   bx lr
