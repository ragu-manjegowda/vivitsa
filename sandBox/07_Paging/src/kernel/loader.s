; Entry symbol for ELF
global loader

; size of stack in bytes
KERNEL_STACK_SIZE equ 4096

;setting up the Multiboot header - see GRUB docs for details

; align loaded modules on page boundaries
MODULEALIGN equ 1 << 0

; provide memory map
MEMINFO equ 1 << 1

; this is the Multiboot 'flag' field
FLAGS equ MODULEALIGN | MEMINFO

; 'magic number' lets bootloader find the header
MAGIC_NUMBER equ 0x1BADB002

; calculate the checksum (all options + checksum should equal 0)
CHECKSUM equ - (MAGIC_NUMBER + FLAGS)

; Kernel Start Physical Address is exported from linker script
extern KERNEL_PHYSICAL_START

; Kernel End Physical Address is exported from linker script
extern KERNEL_PHYSICAL_END

; start of the bss section
section .bss
; align at 4 bytes
align 4
; label points to beginning of memory
KERNEL_STACK:
    ; reserve stack for the kernel
    resb KERNEL_STACK_SIZE

; start of the text (code) section
section .text
; Align at 4 bytes
align 4
    ; write the magic number to the machine code,
    dd MAGIC_NUMBER
    ; the flags,
    dd FLAGS
    ; the checksum
    dd CHECKSUM

; the loader label (defined as entry point in linker script)
loader:
    ; point esp to the start of the stack (end of memory area)
    mov esp, KERNEL_STACK + KERNEL_STACK_SIZE
    ; kmain function is defined elsewhere
    extern kmain
    ; the kernel end address
    push $KERNEL_PHYSICAL_END
    ; and the kernel start address
    push $KERNEL_PHYSICAL_START
    ; call kernel main function.
    call kmain
    .loop:
    ; loop forever
      jmp .loop
