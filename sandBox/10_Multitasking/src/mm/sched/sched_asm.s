[GLOBAL read_eip]
read_eip:
  ; Get the return address
  pop eax
  ; Can't use RET because return address popped off the stack.
  jmp eax

; Here we:
; * Stop interrupts so we don't get interrupted.
; * Temporarily put the new EIP location in ECX.
; * Temporarily put the new page directory's physical address in EAX.
; * Set the base and stack pointers
; * Set the page directory
; * Put a dummy value (0x12345) in EAX so that above we can recognize that
;   we've just switched task.
; * Restart interrupts. The STI instruction has a delay - it doesn't take
;   effect until after the next instruction.
; * Jump to the location in ECX (remember we put the new EIP in there).

[GLOBAL task_switch]
task_switch:
     cli;
     mov ecx, [esp+4]   ; EIP
     mov eax, [esp+8]   ; physical address of current directory
     mov ebp, [esp+12]  ; EBP
     mov esp, [esp+16]  ; ESP
     mov cr3, eax       ; set the page directory
     mov eax, 0x12345   ; magic number to detect a task switch
     sti;
     jmp ecx
