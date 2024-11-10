[bits 32]
[extern kernel_main]

section .text
    global _start

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Call kernel
    call kernel_main
    
    ; Hang if kernel returns
    cli
    hlt
    jmp $

section .bss
align 4
stack_bottom:
    resb 16384  ; 16 KB stack
stack_top:
