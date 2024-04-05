#include "address_map_nios2.h"

extern volatile char byte1, byte2, byte3;
extern Mouse mouse;
int mouseInserted = 0;
void updateCursorX();
void updateCursorY();

/*******************************************************************************
 * PS2 - Interrupt Service Routine                                
 *                                                                          
 * This routine stores to last three bytes received from the PS/2 device.
 ******************************************************************************/
void PS2_ISR( void )
{
  	volatile int * PS2_ptr = (int *) PS2_BASE;		// PS/2 port address
	int PS2_data, RAVAIL;

	PS2_data = *(PS2_ptr);							// read the Data register in the PS/2 port
	RAVAIL = (PS2_data & 0xFFFF0000) >> 16;			// extract the RAVAIL field
	if (RAVAIL > 0)
	{
		/* always save the last three bytes received */
		byte1 = byte2;
		byte2 = byte3;
		byte3 = PS2_data & 0xFF;

		if (mouseInserted){
			mouse.dx = byte2;
			mouse.dy = byte3;
			//updateCursorX();
			updateCursorY();

			mouse.xSign = (byte1 & 0b10000) >> 4;
			mouse.ySign = (byte1 & 0b100000) >> 5;
			mouse.leftClicked = byte1 & 0b1;
		}

		if ( (byte2 == (char) 0xAA) && (byte3 == (char) 0x00) )
			// mouse inserted; initialize sending of data
			*(PS2_ptr) = 0xF4;
			mouseInserted = 1;
	}
	return;
}
