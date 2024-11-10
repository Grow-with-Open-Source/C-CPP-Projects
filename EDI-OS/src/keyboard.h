#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "kernel.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_BUFFER_SIZE 256

// Scan code constants
#define SC_ENTER 0x1C
#define SC_BACKSPACE 0x0E
#define SC_LSHIFT 0x2A
#define SC_RSHIFT 0x36
#define SC_LSHIFT_REL 0xAA
#define SC_RSHIFT_REL 0xB6

// Function declarations
void init_keyboard(void);
void keyboard_handler(void);
char get_ascii(uint8_t scancode);
void handle_command(char* command);

// Command buffer
extern char command_buffer[KEYBOARD_BUFFER_SIZE];
extern uint32_t buffer_position;

#endif
