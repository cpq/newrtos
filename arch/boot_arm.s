.cpu cortex-m3
.thumb

.section .vectors,"a",%progbits   // Place interrupt vectors into its own section
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
.word   PendSV_Handler       // 14 PendSV
.word   SysTick_Handler      // 15 SysTick
// STM32 specific handlers
.word   halt,halt,halt,halt,halt,halt,halt,halt,halt,halt
.word   halt,halt,halt,halt,halt,halt,halt,halt,halt,halt
.word   halt,halt,halt,halt,halt,halt,halt,halt,halt,halt
.word   halt,halt,halt,halt,halt,halt,halt,halt,halt,halt
.word   halt,halt,halt,halt,halt,halt,halt,halt,halt,halt

.section .text      // Place interrupt handlers into the .text section

.thumb_func         // Important. Marks ARM thumb mode function, see
                    // http://twins.ee.nctu.edu.tw/courses/ip_core_01/lab_hw_pdf/lab_1.pdf
                    // .thumb_func makes linker add a state change code block
.global _reset      // Make _reset handler visible to the linker
_reset:
  bl main           // Jump to main()
  b .               // Should not be here. Halt

.thumb_func
halt:   b .         // `b` is jump, `.` is current address: infinite loop

.thumb_func         // `bx` jumps to an address in `lr` register
pass:   bx lr       // `lr` is a link register, contains return address
