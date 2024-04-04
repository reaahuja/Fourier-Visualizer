#include <stdio.h>
// https://www-ug.eecg.utoronto.ca/desl/nios_devices_SoC/ARM/datasheets/PS2%20Mouse%20Protocol.htm
// Pointers to various control registers for the De1-SoC Board.

/* definitions taht do NOt need to be copied (in mainTestign)*/
#define PS2_BASE 0xFF200100;
#define LED_BASE 0xFF200000;
#define X_RESOLUTION 320
#define Y_RESOLUTION 240

unsigned char byte0, byte1, byte2;
volatile int *PS2_ptr = (int *)PS2_BASE;  // PS/2 port address
volatile int *LED_ptr = (int *)LED_BASE;  // PS/2 port address
unsigned char leftClicked;
void initMouse(Mouse mouse);

/*******************************************************************************
 * PS2 - Interrupt Service Routine
 *
 * This routine stores to last three bytes received from the PS/2 device.
 ******************************************************************************/

struct Mouse {
  int x, y;
  int xPrev, yPrev;
  int dx, dy;
};

typedef struct Mouse Mouse;

int main() {
  Mouse mouse;
  initMouse(mouse);
  while (1) PS2_ISR();
}

void initMouse(Mouse mouse) {
  // The cursor starts in the centre of the screen.
  mouse.x = X_RESOLUTION / 2;
  mouse.y = Y_RESOLUTION / 2;

  mouse.xPrev = mouse.x;
  mouse.yPrev = mouse.y;

  return;
}

void PS2_ISR(void) {
  int PS2_data, RAVAIL;

  PS2_data = *(PS2_ptr);       // read the Data register in the PS/2 port
  RAVAIL = PS2_data & 0x8000;  // extract the RAVAIL field
  // FOR BOARD: (PS2_data & 0xFFFF0000) >> 16;	, RAVAIL > 0
  if (RAVAIL != 0) {
    /* always save the last three bytes received */
    byte0 = byte1;
    byte1 = byte2;
    byte2 = PS2_data & 0xFF;
    if ((byte1 == (char)0xAA) && (byte2 == (char)0x00))
      // mouse inserted; initialize sending of data
      *(PS2_ptr) = 0xF4;
    *LED_ptr = byte0;
    leftClicked = byte0 & 0x1;  // if 1, means left button clicked
  }
  return;
}