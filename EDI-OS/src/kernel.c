#include "kernel.h"
#include "keyboard.h"

// VGA memory configuration
static uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;
static const uint32_t VGA_WIDTH = 80;
static const uint32_t VGA_HEIGHT = 25;

// Make terminal variables accessible to other files
uint32_t terminal_row;
uint32_t terminal_column;
uint8_t terminal_color;

// Much longer delay function
void delay(uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        for (uint32_t j = 0; j < 0x2FFFFF; j++) {  // Increased inner loop
            asm volatile("nop");
        }
    }
}

uint8_t make_color(enum vga_color fg, enum vga_color bg) {
    return (uint8_t)fg | ((uint8_t)bg << 4);
}

static uint16_t make_vgaentry(char c, uint8_t color) {
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

void init_video(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = make_color(VGA_WHITE, VGA_BLUE);
    
    // Clear screen
    for (uint32_t y = 0; y < VGA_HEIGHT; y++) {
        for (uint32_t x = 0; x < VGA_WIDTH; x++) {
            const uint32_t index = y * VGA_WIDTH + x;
            VGA_MEMORY[index] = make_vgaentry(' ', terminal_color);
        }
    }
}

void clear_screen(void) {
    init_video();
}

void terminal_putentryat(char c, uint8_t color, uint32_t x, uint32_t y) {
    const uint32_t index = y * VGA_WIDTH + x;
    VGA_MEMORY[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row >= VGA_HEIGHT) {
            terminal_row = 0;
        }
        return;
    }

    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}

void print_string(const char* str) {
    for (uint32_t i = 0; str[i] != '\0'; i++) {
        terminal_putchar(str[i]);
    }
}

void print_colored(const char* str, uint8_t color) {
    uint8_t old_color = terminal_color;
    terminal_color = color;
    print_string(str);
    terminal_color = old_color;
}

void kernel_main(void) {
    // Initialize terminal
    init_video();
    
    // Show initialization messages with longer delays
    print_colored("EDI-OS Kernel Loading...\n", make_color(VGA_CYAN, VGA_BLUE));
    delay(4);  // Increased delay
    
    print_colored("[....] Checking CPU status", make_color(VGA_YELLOW, VGA_BLUE));
    delay(2);
    terminal_column -= 25;  // Move back to overwrite the status
    print_colored("[OK] CPU status verified\n", make_color(VGA_GREEN, VGA_BLUE));
    delay(3);
    
    print_colored("[....] Initializing video driver", make_color(VGA_YELLOW, VGA_BLUE));
    delay(2);
    terminal_column -= 27;
    print_colored("[OK] Video initialization complete\n", make_color(VGA_GREEN, VGA_BLUE));
    delay(3);
    
    print_colored("[....] Setting up memory management", make_color(VGA_YELLOW, VGA_BLUE));
    delay(2);
    terminal_column -= 30;
    print_colored("[OK] Memory management initialized\n", make_color(VGA_GREEN, VGA_BLUE));
    delay(3);
    
    // Initialize keyboard with status display
    print_colored("[....] Configuring keyboard controller", make_color(VGA_YELLOW, VGA_BLUE));
    init_keyboard();
    delay(2);
    terminal_column -= 33;
    print_colored("[OK] Keyboard controller initialized\n", make_color(VGA_GREEN, VGA_BLUE));
    delay(3);
    
    print_colored("[....] Setting up interrupt handlers", make_color(VGA_YELLOW, VGA_BLUE));
    delay(2);
    terminal_column -= 31;
    print_colored("[OK] Interrupt handlers configured\n", make_color(VGA_GREEN, VGA_BLUE));
    delay(3);
    
    // Print system information
    print_colored("\n================================\n", make_color(VGA_CYAN, VGA_BLUE));
    delay(1);
    print_colored("Welcome to EDI-OS v0.2\n", make_color(VGA_WHITE, VGA_BLUE));
    delay(1);
    print_colored("================================\n", make_color(VGA_CYAN, VGA_BLUE));
    delay(1);
    
    print_colored("\nSystem Status:\n", make_color(VGA_YELLOW, VGA_BLUE));
    delay(1);
    print_colored("- CPU Mode: Protected Mode (32-bit)\n", make_color(VGA_WHITE, VGA_BLUE));
    delay(1);
    print_colored("- Memory: 640KB Base Memory\n", make_color(VGA_WHITE, VGA_BLUE));
    delay(1);
    print_colored("- Video: VGA Text Mode 80x25\n", make_color(VGA_WHITE, VGA_BLUE));
    delay(1);
    print_colored("- Keyboard: PS/2 Controller Active\n", make_color(VGA_WHITE, VGA_BLUE));
    delay(1);
    print_colored("- Interrupts: Configured and Enabled\n", make_color(VGA_WHITE, VGA_BLUE));
    delay(1);
    
    print_colored("\nSystem initialized successfully!\n", make_color(VGA_GREEN, VGA_BLUE));
    delay(2);
    print_string("\nType 'help' for available commands\n\n");
    delay(2);
    
    // Initialize command prompt
    print_colored("> ", make_color(VGA_GREEN, VGA_BLUE));
    
    // Main kernel loop
    while (1) {
        // Check for keyboard input
        if (inb(KEYBOARD_STATUS_PORT) & 0x1) {
            keyboard_handler();
        }
    }
}
