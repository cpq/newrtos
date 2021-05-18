MEMORY {
  ram(rwx)  : ORIGIN = 0x20000000, LENGTH = 12k
  rom(rx)   : ORIGIN = 0x08000000, LENGTH = 64k
}
