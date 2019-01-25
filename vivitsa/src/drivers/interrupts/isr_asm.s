; isr handler is implemented in isr.c
extern interrupt_handler

;macro for no error code handler starts from here
%macro no_error_code_interrupt_handler 1  ; define a macro, taking one parameter

global isr%1                              ; %1 accesses the first parameter.
  isr%1:
    cli                                   ; Disable interrupts
    push    dword 0                       ; push 0 as error code
    push    dword %1                      ; push the interrupt number
    jmp     common_interrupt_handler      ; jump to the common handler

%endmacro
; no error code handler macro end


;macro for error code handler starts from here
%macro error_code_interrupt_handler 1     ; define a macro, taking one parameter

global isr%1
  isr%1:
    cli                                   ; Disable interrupts
    push    dword %1                      ; push the interrupt number
    jmp     common_interrupt_handler      ; jump to the common handler

%endmacro
;macro for error code handler end

;macro for interrupt handler for PIC starts
; This macro creates a stub for an IRQ - the first parameter is
; the IRQ number, the second is the ISR number it is remapped to.
%macro irq_pic_interrupt_handler 2        ; define a macro, taking two parameter

global irq%1                              ; %1 accesses the first parameter.
  irq%1:
    cli                                   ; Disable interrupts
    push    dword 0                       ; push 0 as error code
    push    dword %2                      ; push the interrupt number
    jmp     common_interrupt_handler      ; jump to the common handler for irq

%endmacro
; interrupt handler for PIC macro end

; define macros for all 32 interrupts, 8, 10, 11, 12, 13, 14 generate error code
no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
error_code_interrupt_handler    8
no_error_code_interrupt_handler 9
error_code_interrupt_handler    10
error_code_interrupt_handler    11
error_code_interrupt_handler    12
error_code_interrupt_handler    13
error_code_interrupt_handler    14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
no_error_code_interrupt_handler 17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20
no_error_code_interrupt_handler 21
no_error_code_interrupt_handler 22
no_error_code_interrupt_handler 23
no_error_code_interrupt_handler 24
no_error_code_interrupt_handler 25
no_error_code_interrupt_handler 26
no_error_code_interrupt_handler 27
no_error_code_interrupt_handler 28
no_error_code_interrupt_handler 29
no_error_code_interrupt_handler 30
no_error_code_interrupt_handler 31

; define macros for all 16 IRQ interrupts
irq_pic_interrupt_handler 0,32
irq_pic_interrupt_handler 1,33
irq_pic_interrupt_handler 2,34
irq_pic_interrupt_handler 3,35
irq_pic_interrupt_handler 4,36
irq_pic_interrupt_handler 5,37
irq_pic_interrupt_handler 6,38
irq_pic_interrupt_handler 7,39
irq_pic_interrupt_handler 8,40
irq_pic_interrupt_handler 9,41
irq_pic_interrupt_handler 10,42
irq_pic_interrupt_handler 11,43
irq_pic_interrupt_handler 12,44
irq_pic_interrupt_handler 13,45
irq_pic_interrupt_handler 14,46
irq_pic_interrupt_handler 15,47

; Common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
common_interrupt_handler:                 ; the common parts of the generic
                                          ; interrupt handler
    ; save the registers
    pusha                                 ; Pushes eax, ebx, ecx, edx, edi, esi,
                                          ; ebp, esp,

    mov     ax, ds                        ; Lower 16-bits of eax = ds.
    push    eax                           ; save the data segment descriptor

    mov     ax, 0x10                      ; load the kernel data segment
    mov     ds, ax                        ; descriptor, this is because ISR is
    mov     es, ax                        ; defined in Kernel's data segment.
    mov     fs, ax
    mov     gs, ax

    ; call the C function
    call    interrupt_handler

    ; restore the registers
    pop     eax                           ; reload the original data segment
    mov     ds, ax                        ; descriptor
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    popa                                  ; Pops edi,esi,ebp...

    ; restore the esp
    add     esp, 8                        ; Cleans up the pushed error code and
                                          ; pushed ISR number

    sti                                   ; set interrupts

    ; return to the code that got interrupted
    ; When an interrupt fires, the processor automatically pushes information
    ; about the processor state onto the stack. The code segment, instruction
    ; pointer, flags register, stack segment and stack pointer are pushed. The
    ; IRET instruction is specifically designed to return from an interrupt. It
    ; pops these values off the stack and returns the processor to the state it
    ; was in originally.
    iret
