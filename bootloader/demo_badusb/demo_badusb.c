#include "ch32v003fun.h"
#include <stdio.h>
#include <string.h>
#include "rv003usb.h"

int main(){
	SystemInit();
	Delay_Ms(2); // Ensures USB re-enumeration after bootloader or reset; Spec demand >2.5µs ( TDDIS )
	usb_setup();
	while(1){}
}

// Function to map ASCII characters to HID keycodes and set Shift modifier if needed
uint8_t ascii_to_hid(char c, uint8_t *modifier) {
	*modifier = 0x00;  // Reset modifier byte

	// Check if character is a capital letter (requires Shift)
	if (c >= 'A' && c <= 'Z') {
		*modifier = 0x02;  // Left Shift
		return c - 'A' + 0x04;  // HID keycodes for 'A' to 'Z'
	}

	// Lowercase letters
	if (c >= 'a' && c <= 'z') {
		return c - 'a' + 0x04;  // HID keycodes for 'a' to 'z'
	}

	// Numbers and symbols (handle Shift for symbols)
	if (c >= '0' && c <= '9') {
		return c - '0' + 0x1E;  // HID keycodes for '0' to '9'
	}

	// Common symbols and punctuation
	switch (c) {
		case ' ': return 0x2C;  // Space
		case '.': return 0x37;  // Period
		case ',': return 0x36;  // Comma
		case '-': return 0x2D;	// Dash
        case '/': return 0x38;  // Slash
		case '\'': return 0x34;  // Single quote
		case '~': *modifier = 0x02; return 0x35;  // Tilde
		case '!': *modifier = 0x02; return 0x1E;  // Exclamation mark (Shift + 1)
		case '@': *modifier = 0x02; return 0x1F;  // At symbol (Shift + 2)
		case '#': *modifier = 0x02; return 0x20;  // Hash symbol (Shift + 3)
		case '$': *modifier = 0x02; return 0x21;  // Dollar sign (Shift + 4)
		case '%': *modifier = 0x02; return 0x22;  // Percent sign (Shift + 5)
		case '^': *modifier = 0x02; return 0x23;  // Caret (Shift + 6)
		case '&': *modifier = 0x02; return 0x24;  // Ampersand (Shift + 7)
		case '*': *modifier = 0x02; return 0x25;  // Asterisk (Shift + 8)
		case '(': *modifier = 0x02; return 0x26;  // Left parenthesis (Shift + 9)
		case ')': *modifier = 0x02; return 0x27;  // Right parenthesis (Shift + 0)
		case '\n': return 0x28;  // Enter
		case '\r': *modifier = 0x08; return 0x2C; // Left GUI + Space
		default: return 0x00;  // Unsupported characters
	}
}

void usb_handle_user_in_request(struct usb_endpoint *e, uint8_t *scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal *ist) {
	if (endp == 2) {
		static char payload[] = "\r\tterminal\t\n\t\tfind ~/\n";
		static int i = 0;
		static uint8_t tsajoystick[8] = { 0x00 };  // Keyboard (8 bytes)
		static int delay_counter = 0;  // Counter for the delay
		static int delay_mode = 0;  // Enable delay mode to pause typing
		int length = sizeof(payload) - 1;  // Exclude null terminator

		// Simulate a delay of ~1 second (assuming the function is called frequently)
		if (delay_counter < 225) {  // Adjust this value as per call frequency (~1 second)
			delay_counter++;
			return;  // Exit the function until the delay is over
		}

		if(delay_mode == 1){
			Delay_Ms(500);
			delay_mode = 0;
			i++;
			return;
		}

		// Send 8-byte keyboard report
		usb_send_data(tsajoystick, 8, 0, sendtok);

		// If all characters are sent, stop or reset
		if (i < length) {
			if (payload[i] == '\t') {
                delay_mode = 1;  // Enter delay mode
                return;  // Exit the function until delay is over
            }

			// Get the modifier and keycode for the current character
			uint8_t modifier = 0x00;
			tsajoystick[4] = ascii_to_hid(payload[i], &modifier);
			tsajoystick[0] = modifier;  // Set modifier byte (Shift if needed)
			i++;
		} else {
			tsajoystick[4] = 0x00;  // No key pressed after message is sent
			tsajoystick[0] = 0x00;  // Reset modifiers
		}
	} else {
		// If it's a control transfer, send an empty packet
		usb_send_empty(sendtok);
	}
}


// 