[GLOBAL read_eip]
read_eip:
  ; Get the return address
  pop eax
  ; Can't use RET because return address popped off the stack.
  jmp eax
