MEMORY {
  ram(rwx)  : ORIGIN = 0x20000000, LENGTH = 320k
  rom(rx)   : ORIGIN = 0x08000000, LENGTH = 1024k
}
