#include "address_map_nios2.h"
#include <stdio.h>

extern volatile char byte1, byte2, byte3;
int frequency;
int frequencyDigits; 
int started = 0;


//If I key pressed, enter into INPUT FREQUENCY mode - then parse the numbers inputted
//select wave - s, t, w, r
//If R, reset and return to home page
//If up/down - change frequency


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

			volatile unsigned int breakCode;
			breakCode = (byte2 >> 8) + byte3;


			if(byte2 == (char) 0xF0 && byte3 == (char) 0x45){ //1
				printf("0 pressed");
			}
			if(byte2 == (char)0xF0 && byte3 == (char) 0x16){ //1
				printf("1 pressed");
			}
            if(byte2 == (char) 0xF0 && byte3 == (char) 0x1E){ //1
				printf("2 pressed");
			}
			 if(byte2 == (char) 0xF0 && byte3 == (char) 0x26){ //1
				printf("3 pressed");
			}
		    if(byte2 == (char) 0xF0 && byte3 == (char) 0x25){ //1
				printf("4 pressed");
			}
             if(byte2 == (char) 0xF0 && byte3 == (char) 0x2E){ //1
				printf("5 pressed");
			}
             if(byte2 == (char) 0xF0 && byte3 == (char) 0x36){ //1
				printf("6 pressed");
			}
            if(byte2 == (char) 0xF0 && byte3 == (char) 0x3D){ //1
				printf("7 pressed");
			}
            if(byte2 == (char)0xF0 && byte3 == (char) 0x3E){ //1
				printf("8 pressed");
			}
            if(byte2 == (char) 0xF0 && byte3 == (char) 0x46){ //1
				printf("9 pressed");
			}
			if(byte2 == (char) 0xF0 && byte3 == (char) 0x43){ //1
				printf("I pressed");
			}
			if(byte2 == (char) 0xF0 && byte3 == (char) 0x2D){ //1
				printf("R pressed");
			}
			if(byte2 == (char) 0xF0 && byte3 == (char) 0x1B){ //1
				printf("S pressed");
			}
			if(byte2 == (char) 0xF0 && byte3 == (char) 0x15){ //1
				printf("Q pressed");
			}
			if(byte2 == (char) 0xF0 && byte3 == (char) 0x2C){ //1
				printf("T pressed");
			}
			if(byte2 == (char) 0xF0 && byte3 == (char) 0x1D){ //1
				printf("W pressed");
			}
			//printf("another key pressed");
	
		
		if ( (byte2 == (char) 0xAA) && (byte3 == (char) 0x00) )
			// mouse inserted; initialize sending of data
			*(PS2_ptr) = 0xF4;
		
	}
	return;
}
