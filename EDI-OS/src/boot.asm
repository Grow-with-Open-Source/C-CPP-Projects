[org 0x7c00]
[bits 16]

; Constants
KERNEL_OFFSET equ 0x1000
VIDEO_MODE equ 0x03        ; 80x25 text mode
STACK_BASE equ 0x9000
SECTORS_TO_READ equ 15     ; Number of sectors for kernel

; Initialize segments and stack
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, STACK_BASE

; Save boot drive number
mov [BOOT_DRIVE], dl

; Set video mode
mov ah, 0x00
mov al, VIDEO_MODE
int 0x10

; Print initial boot message
mov si, MSG_BOOT
call print_string
mov cx, 5              ; Number of delay iterations
call long_delay

; Print memory check message
mov si, MSG_MEM_CHECK
call print_string
mov cx, 4
call long_delay

; Reset disk system and show message
mov si, MSG_DISK_RESET
call print_string
xor ah, ah
mov dl, [BOOT_DRIVE]
int 0x13
jc disk_error
mov cx, 4
call long_delay

; Load kernel message
mov si, MSG_LOAD_KERNEL
call print_string

; Load kernel
mov bx, KERNEL_OFFSET    ; Destination address
mov dh, SECTORS_TO_READ  ; Sectors to read
call load_disk
mov cx, 4
call long_delay

; Switch to protected mode message
mov si, MSG_PROT_MODE
call print_string
mov cx, 4
call long_delay

; Switch to protected mode
cli                      ; Clear interrupts
lgdt [GDT_DESCRIPTOR]    ; Load GDT
mov eax, cr0
or eax, 0x1             ; Set protected mode bit
mov cr0, eax
jmp CODE_SEG:init_pm    ; Far jump to 32-bit code

; Function to add a longer delay
; cx = number of iterations
long_delay:
    push cx             ; Save outer loop counter
.outer_loop:
    push cx
    mov cx, 0xFFFF      ; Maximum 16-bit value
.inner_loop1:
    push cx
    mov cx, 0x0FFF      ; Increased inner delay
.inner_loop2:
    loop .inner_loop2
    pop cx
    loop .inner_loop1
    pop cx
    loop .outer_loop
    pop cx              ; Restore outer loop counter
    ret

load_disk:
    push dx             ; Save DX
    
    mov ah, 0x02       ; BIOS read function
    mov al, dh         ; Number of sectors
    mov ch, 0          ; Cylinder 0
    mov dh, 0          ; Head 0
    mov cl, 2          ; Start from sector 2
    mov dl, [BOOT_DRIVE]
    
    int 0x13
    jc disk_error      ; Check for error
    
    pop dx             ; Restore DX
    cmp dh, al         ; Compare sectors read
    jne disk_error     ; Error if not equal
    
    ; Print success message
    mov si, MSG_LOAD_OK
    call print_string
    ret

disk_error:
    mov si, DISK_ERROR_MSG
    call print_string
    jmp $

print_string:
    mov ah, 0x0E       ; BIOS teletype function
.loop:
    lodsb              ; Load next character
    test al, al        ; Check for null terminator
    jz .done           ; If zero, we're done
    int 0x10          ; Otherwise, print
    jmp .loop
.done:
    ret

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov ebp, STACK_BASE
    mov esp, ebp
    
    jmp KERNEL_OFFSET

; Data
BOOT_DRIVE db 0
MSG_BOOT db 'EDI-OS Bootloader starting...', 0x0D, 0x0A, 0
MSG_MEM_CHECK db 'Checking memory map...OK', 0x0D, 0x0A, 0
MSG_DISK_RESET db 'Resetting disk system...', 0x0D, 0x0A, 0
MSG_LOAD_KERNEL db 'Loading kernel into memory...', 0x0D, 0x0A, 0
MSG_LOAD_OK db 'Kernel loaded successfully!', 0x0D, 0x0A, 0
MSG_PROT_MODE db 'Switching to protected mode...', 0x0D, 0x0A, 0
DISK_ERROR_MSG db 'Disk read error!', 0x0D, 0x0A, 0

; GDT
GDT_START:
    dq 0               ; Null descriptor

GDT_CODE:
    dw 0xFFFF          ; Limit (0-15)
    dw 0               ; Base (0-15)
    db 0               ; Base (16-23)
    db 10011010b       ; Access byte
    db 11001111b       ; Flags and limit (16-19)
    db 0               ; Base (24-31)

GDT_DATA:
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0

GDT_END:

GDT_DESCRIPTOR:
    dw GDT_END - GDT_START - 1
    dd GDT_START

CODE_SEG equ GDT_CODE - GDT_START
DATA_SEG equ GDT_DATA - GDT_START

; Boot signature
times 510-($-$$) db 0
dw 0xAA55
