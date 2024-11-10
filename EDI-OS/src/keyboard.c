#include "keyboard.h"

char command_buffer[KEYBOARD_BUFFER_SIZE];
uint32_t buffer_position = 0;
static uint8_t shift_pressed = 0;

// Scancode to ASCII mapping (US QWERTY layout)
static const char ascii_lower[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
    0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static const char ascii_upper[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

void init_keyboard(void) {
    // Clear command buffer
    for (uint32_t i = 0; i < KEYBOARD_BUFFER_SIZE; i++) {
        command_buffer[i] = 0;
    }
    buffer_position = 0;
}

char get_ascii(uint8_t scancode) {
    if (scancode >= sizeof(ascii_lower)) {
        return 0;
    }
    
    return shift_pressed ? ascii_upper[scancode] : ascii_lower[scancode];
}

void keyboard_handler(void) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Handle shift keys
    if (scancode == SC_LSHIFT || scancode == SC_RSHIFT) {
        shift_pressed = 1;
        return;
    } else if (scancode == SC_LSHIFT_REL || scancode == SC_RSHIFT_REL) {
        shift_pressed = 0;
        return;
    }
    
    // Only handle key press events (ignore key release)
    if (scancode & 0x80) {
        return;
    }
    
    // Handle special keys
    if (scancode == SC_ENTER) {
        print_string("\n");
        command_buffer[buffer_position] = '\0';
        handle_command(command_buffer);
        buffer_position = 0;
        print_colored("> ", make_color(VGA_GREEN, VGA_BLUE));
        return;
    } else if (scancode == SC_BACKSPACE && buffer_position > 0) {
        buffer_position--;
        command_buffer[buffer_position] = 0;
        // Move cursor back and clear character
        if (terminal_column > 0) {
            terminal_column--;
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        }
        return;
    }
    
    // Convert scancode to ASCII and handle regular keys
    char ascii = get_ascii(scancode);
    if (ascii && buffer_position < KEYBOARD_BUFFER_SIZE - 1) {
        command_buffer[buffer_position++] = ascii;
        terminal_putchar(ascii);
    }
}

void handle_command(char* command) {
    if (strcmp(command, "help") == 0) {
        print_string("Available commands:\n");
        print_string("  help     - Show this help message\n");
        print_string("  clear    - Clear the screen\n");
        print_string("  version  - Show OS version\n");
        print_string("  about    - About EDI-OS\n");
    } else if (strcmp(command, "clear") == 0) {
        clear_screen();
        print_colored("> ", make_color(VGA_GREEN, VGA_BLUE));
    } else if (strcmp(command, "version") == 0) {
        print_string("EDI-OS version 0.2\n");
    } else if (strcmp(command, "about") == 0) {
        print_colored("EDI-OS v0.2\n", make_color(VGA_CYAN, VGA_BLUE));
        print_string("A simple operating system for learning purposes\n");
    } else if (command[0] != '\0') {
        print_string("Unknown command: ");
        print_string(command);
        print_string("\nType 'help' for available commands\n");
    }
}
