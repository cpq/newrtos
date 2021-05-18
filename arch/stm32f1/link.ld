MEMORY {
  ram(rwx)  : ORIGIN = 0x20000000, LENGTH = 20k
  rom(rx)   : ORIGIN = 0x08000000, LENGTH = 128k
}
