// Could be defined here, or in the processor defines.
#define SYSTEM_CORE_CLOCK 12000000
#define SYSTICK_USE_HCLK

#include "ch32v003fun.h"
#include <stdio.h>
#include <string.h>
#include "rv003usb.h"

int main()
{
	SystemInit();
	Delay_Ms(1); // Ensures USB re-enumeration after bootloader or reset; Spec demand >2.5µs ( TDDIS )
	usb_setup();
	while(1){}
}

void usb_handle_user_in_request( struct usb_endpoint * e, uint8_t * scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal * ist )
{
	if( endp == 1 )
	{
		// Mouse (4 bytes)
		static int i;
		static uint8_t tsajoystick[4] = { 0x00, 0x00, 0x00, 0x00 };
		i++;
		int mode = i;

		// Move the mouse right, down, left and up in a square.
		switch( mode & 3 )
		{
		case 0: tsajoystick[1] =  1; tsajoystick[2] = 0; break;
		case 1: tsajoystick[1] =  0; tsajoystick[2] = 0; break;
		case 2: tsajoystick[1] = -1; tsajoystick[2] = 0; break;
		case 3: tsajoystick[1] =  0; tsajoystick[2] = 0; break;
		}
		usb_send_data( tsajoystick, 4, 0, sendtok );
	}
	else
	{
		// If it's a control transfer, empty it.
		usb_send_empty( sendtok );
	}
}


