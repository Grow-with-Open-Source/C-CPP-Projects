# Compiler and flags
CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -fno-pie -ffreestanding -fno-builtin -O2 -Wall -Wextra -fno-stack-protector -nostdlib -nodefaultlibs
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld --oformat binary

# Source files
BOOT_SRC = boot.asm
KERNEL_ENTRY_SRC = kernel_entry.asm
KERNEL_SRC = kernel.c
KEYBOARD_SRC = keyboard.c

# Object files
BOOT_BIN = boot.bin
KERNEL_ENTRY_OBJ = kernel_entry.o
KERNEL_OBJ = kernel.o
KEYBOARD_OBJ = keyboard.o
KERNEL_BIN = kernel.bin
OS_IMAGE = os-image.bin

# Build rules
all: $(OS_IMAGE)

$(BOOT_BIN): $(BOOT_SRC)
	$(AS) -f bin -o $@ $<

$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY_SRC)
	$(AS) $(ASFLAGS) -o $@ $<

$(KERNEL_OBJ): $(KERNEL_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(KEYBOARD_OBJ): $(KEYBOARD_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(KERNEL_ENTRY_OBJ) $(KERNEL_OBJ) $(KEYBOARD_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	# Create a blank 1.44MB floppy image
	dd if=/dev/zero of=$@ bs=1024 count=1440
	# Write boot sector to first sector
	dd if=$(BOOT_BIN) of=$@ conv=notrunc
	# Write kernel starting at second sector
	dd if=$(KERNEL_BIN) of=$@ seek=1 conv=notrunc

run: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE)

debug: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE) -monitor stdio

clean:
	rm -f *.bin *.o

.PHONY: all clean run debug
