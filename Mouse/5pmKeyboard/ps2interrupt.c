#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"
#include <stdio.h>

#define X_RESOLUTION 320
#define Y_RESOLUTION 240

/* these globals are written by interrupt service routines; we have to declare
 * these as volatile to avoid the compiler caching their values in registers */
extern volatile char byte1, byte2,
    byte3; // modified by PS/2 interrupt service routine
extern volatile int timeout; // used to synchronize with the timer
volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

Mouse mouse; //also declared in PS2_ISR


/* function prototypes */
void HEX_PS2(char, char, char);
void plot_pixel(int x, int y, short int line_color);
void initMouse();
void clear_screen();
void moveMouse();
void clicked();
void keyboard_ISR();

/*******************************************************************************
 * This program performs the following:
 * Displays the last three bytes of data received from the PS/2 port
 * on the HEX displays. The PS/2 port is handled using interrupts
 ******************************************************************************/
int main(void) {

    /* Declare volatile pointers to I/O registers (volatile means that IO load
       and store instructions will be used to access these pointer locations,
       instead of regular memory loads and stores) */
    volatile int * interval_timer_ptr =
        (int *)TIMER_BASE;                    // interal timer base address
    volatile int * PS2_ptr = (int *)PS2_BASE; // PS/2 port address
    initMouse();
    clear_screen();

    /* initialize some variables */
    byte1   = 0;
    byte2   = 0;
    byte3   = 0; // used to hold PS/2 data
    timeout = 0; // synchronize with the timer

    /* set the interval timer period for scrolling the HEX displays */
    int counter = 20000000; // 1/(100 MHz) x (20000000) = 200 msec
    *(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
    *(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;

    /* start interval timer, enable its interrupts */
    *(interval_timer_ptr + 1) = 0x7; // STOP = 0, START = 1, CONT = 1, ITO = 1

    *(PS2_ptr) = 0xFF; /* reset */
    *(PS2_ptr + 1) =
        0x1; /* write to the PS/2 Control register to enable interrupts */

    NIOS2_WRITE_IENABLE(
        0x81); /* set interrupt mask bits for levels 0 (interval
                * timer) and 7 (PS/2) */

    NIOS2_WRITE_STATUS(1); // enable Nios II interrupts

    while (1) {
        while (!timeout)
            ; // wait to synchronize with timer
        /* display PS/2 data (from interrupt service routine) on HEX displays */
        HEX_PS2(byte1, byte2, byte3);
       // keyboard_ISR();
        timeout = 0;
    }
}

/*******************************************************************************
 * Subroutine to show a string of HEX data on the HEX displays
 ******************************************************************************/
 void plot_pixel(int x, int y, short int line_color) {
  volatile short int *one_pixel_address;
  int pixel_buffer_start = *(pixel_ctrl_ptr);
  one_pixel_address = (short int*) (pixel_buffer_start + (y << 10) + (x << 1));
  *one_pixel_address = line_color;
}

void HEX_PS2(char b1, char b2, char b3) {
    volatile int * HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

    /* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
     * a single 7-seg display, for the hex digits 0 - F */
    unsigned char seven_seg_decode_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int  shift_buffer, nibble;
    unsigned char code;
    int           i;

    shift_buffer = (b1 << 16) | (b2 << 8) | b3;
    for (i = 0; i < 6; ++i) {
        nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
        code   = seven_seg_decode_table[nibble];
        hex_segs[i]  = code;
        shift_buffer = shift_buffer >> 4;
    }
    /* drive the hex displays */
    *(HEX3_HEX0_ptr) = *(int *)(hex_segs);
    *(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);

    // printf("xposition: %d\n",mouse.x);
    // printf("yposition: %d\n",mouse.y);
    // printf("xPrev: %d\n",mouse.xPrev);
    // printf("yPrev: %d\n",mouse.yPrev);
    // printf("clicked: %d\n",mouse.leftClicked);
    // printf("xsign: %d\n",mouse.xSign);
    // printf("ysign: %d\n",mouse.ySign);
    moveMouse(); //will implement a nicer cursor once basic functionality is working 
   // if (mouse.leftClicked) plot_pixel(mouse.x, mouse.y, 0xffff);
}

void initMouse() {
  // The cursor starts in the centre of the screen.
  mouse.x = X_RESOLUTION / 2;
  mouse.y = Y_RESOLUTION / 2;

  mouse.xPrev = mouse.x;
  mouse.yPrev = mouse.y;

  mouse.leftClicked = 0;
  mouse.xSign = 0;
  mouse.ySign = 0;

  return;
}

void drawCursor(int x,int y, short int colour){

}

void moveMouse(){
    plot_pixel(mouse.x,mouse.y,0xccaf);
    plot_pixel(mouse.xPrev, mouse.yPrev, 0x0000);
    // if (mouse.leftClicked) clicked();
}

void updateCursorX(){
    int xRange = 8 * (2 * 255 + 1); //x movement range
    float pixelsPerMouseMovement = 320.0 / xRange;
   // mouse.xPrev = mouse.x;
    if (mouse.xSign== 0 && mouse.x < X_RESOLUTION) mouse.x += mouse.dx * pixelsPerMouseMovement;
    if (mouse.x > X_RESOLUTION) mouse.x = X_RESOLUTION;
    if (mouse.xSign == 1 && mouse.x > 0) mouse.x -= mouse.dx * pixelsPerMouseMovement;
    if (mouse.x < 0 ) mouse.x = 0;
}

void updateCursorY(){
    int yRange = 6 * (2 * 255 + 1); //y movement range
    float pixelsPerMouseMovement = 240.0 / yRange;
   // mouse.yPrev = mouse.y;
    if (mouse.ySign == 0) mouse.y -= mouse.dy * pixelsPerMouseMovement;
    if (mouse.ySign == 1) mouse.y += mouse.dy * pixelsPerMouseMovement;
    if (mouse.y > Y_RESOLUTION) mouse.y = Y_RESOLUTION; //final checks foroutofbounds
    if (mouse.y < 0 ) mouse.y = 0;
}

void clear_screen() {
  for (int y = 0; y <= Y_RESOLUTION - 1; y++) {
    for (int x = 0; x <= X_RESOLUTION - 1; x++) {
      plot_pixel(x, y, 0x0000);
    }
  }
}

void clicked() {
  for (int y = 0; y <= Y_RESOLUTION - 1; y++) {
    for (int x = 0; x <= X_RESOLUTION - 1; x++) {
      plot_pixel(x, y, 0x22fe);
    }
  }
}

// void keyboard_ISR(){
// 	volatile int *keyboard_ptr = (int *) PS2_BASE; //changed
// 	unsigned char byte1 = 0, byte2 = 0, byte3 = 0;

//  int PS2_data, RVALID;


//  PS2_data = *(keyboard_ptr);  // read data register in the PS / 2 port
//  RVALID =  (PS2_data & 0xFFFF0000) >> 16; // flag indicating if valid data is present
// //printf("%d", PS2_data);
// // printf("enteres the loopy");
// printf("%d\n", RVALID);

//  if(RVALID){
//        //reading from the reg provides the data at the head of the FIFO in the data field
//        // it also decrements the data register field by 1
//        //reading RAVAIL provides number of entries in the FIFO
//      byte1 = byte2; // to keep track of the recent history of key presses/releases
//      byte2 = byte3;
//      byte3 = PS2_data & 0xFF;


//      volatile unsigned int compareBytes;


//      compareBytes = (byte2 << 8) | byte3;
//      printf("%d\n", compareBytes);
   
//      //break code
//      if(compareBytes == (char) 0xf024){ //E key 
      
//      } else if (compareBytes == (char) 0xf032){ //B key
//        printf("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
//      } else if (compareBytes == (char) 0xf029){ // Space Bar
//         printf("BBBBBBBBBBBBBBBBorsirusoeirsoeirueoirueoriueoriueoriueoirueoiruB");
//      } else if (compareBytes == (char) 0xf044){
      
//      } else if (compareBytes == (char) 0xf076){
      
//      }
//      else{
//         printf("ERJERHKEJRHEKJRHEKJRH");
//      }
//    }
// }
