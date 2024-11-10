#ifndef KERNEL_H
#define KERNEL_H

// Type definitions
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

// VGA colors enum
enum vga_color {
    VGA_BLACK,
    VGA_BLUE,
    VGA_GREEN,
    VGA_CYAN,
    VGA_RED,
    VGA_MAGENTA,
    VGA_BROWN,
    VGA_LIGHT_GREY,
    VGA_DARK_GREY,
    VGA_LIGHT_BLUE,
    VGA_LIGHT_GREEN,
    VGA_LIGHT_CYAN,
    VGA_LIGHT_RED,
    VGA_LIGHT_MAGENTA,
    VGA_YELLOW,
    VGA_WHITE,
};

// String comparison function
static inline int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// Port I/O functions
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

// Terminal state variables
extern uint32_t terminal_row;
extern uint32_t terminal_column;
extern uint8_t terminal_color;

// Function declarations
void kernel_main(void);
void clear_screen(void);
void print_string(const char* str);
void print_colored(const char* str, uint8_t color);
void init_video(void);
uint8_t make_color(enum vga_color fg, enum vga_color bg);
void terminal_putentryat(char c, uint8_t color, uint32_t x, uint32_t y);
void terminal_putchar(char c);
void delay(uint32_t count);

#endif
