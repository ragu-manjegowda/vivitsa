global idt_flush     ; Allows the C code to call gdt_flush().

; load_idt - Loads the interrupt descriptor table (IDT).
; stack: [esp + 4] the address of the first entry in the IDT
;        [esp    ] the return address
idt_flush:
mov     eax, [esp+4]    ; load the address of the IDT into register eax
lidt    [eax]           ; load the IDT
ret                     ; return to the calling function
