global loader                                             ; the entry symbol for ELF

KERNEL_STACK_SIZE equ 4096                                ; size of stack in bytes
;setting up the Multiboot header - see GRUB docs for details
MODULEALIGN equ  1<<0					                            ; align loaded modules on page boundaries
MEMINFO		equ  1<<1					                              ; provide memory map
FLAGS		equ  MODULEALIGN | MEMINFO	                      ; this is the Multiboot 'flag' field
MAGIC_NUMBER		equ  0x1BADB002				                    ; 'magic number' lets bootloader find the header
; calculate the checksum (all options + checksum should equal 0)
CHECKSUM	equ  -(MAGIC_NUMBER + FLAGS)		                ; checksum required

section .bss
align 4                                                   ; align at 4 bytes
kernel_stack:                                             ; label points to beginning of memory
    resb KERNEL_STACK_SIZE                                ; reserve stack for the kernel

section .text                                             ; start of the text (code) section
align 4                                                   ; the code must be 4 byte aligned
    dd MAGIC_NUMBER                                       ; write the magic number to the machine code,
    dd FLAGS                                              ; the flags,
    dd CHECKSUM                                           ; and the checksum

loader:                                                   ; the loader label (defined as entry point in linker script)
    mov esp, kernel_stack + KERNEL_STACK_SIZE             ; point esp to the start of the
                                                          ; stack (end of memory area)
    extern kmain                                          ; the function sum_of_three is defined elsewhere
    mov eax, 0xAB
    mov eax, 0xCD
    push ebx
    call kmain                                            ; call the function, the result will be in eax
.loop:
    jmp .loop                                             ; loop forever
