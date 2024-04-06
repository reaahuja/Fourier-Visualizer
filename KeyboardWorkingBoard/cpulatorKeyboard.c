#include <stdio.h>

#define BOARD "DE1-SoC"

/* Memory */
#define DDR_BASE 0x40000000
#define DDR_END 0x7FFFFFFF
#define A9_ONCHIP_BASE 0xFFFF0000
#define A9_ONCHIP_END 0xFFFFFFFF
#define SDRAM_BASE 0x00000000
#define SDRAM_END 0x03FFFFFF
#define FPGA_PIXEL_BUF_BASE 0x08000000
#define FPGA_PIXEL_BUF_END 0x0803FFFF
#define FPGA_CHAR_BASE 0x09000000
#define FPGA_CHAR_END 0x09001FFF

/* Cyclone V FPGA devices */
#define LED_BASE 0xFF200000
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define JP1_BASE 0xFF200060
#define JP2_BASE 0xFF200070
#define PS2_BASE 0xFF200100
#define PS2_DUAL_BASE 0xFF200108
#define JTAG_UART_BASE 0xFF201000
#define IrDA_BASE 0xFF201020
#define TIMER_BASE 0xFF202000
#define TIMER_2_BASE 0xFF202020
#define AV_CONFIG_BASE 0xFF203000
#define RGB_RESAMPLER_BASE 0xFF203010
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030
#define AUDIO_BASE 0xFF203040
#define VIDEO_IN_BASE 0xFF203060
#define EDGE_DETECT_CTRL_BASE 0xFF203070
#define ADC_BASE 0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE 0xFF709000
#define I2C0_BASE 0xFFC04000
#define I2C1_BASE 0xFFC05000
#define I2C2_BASE 0xFFC06000
#define I2C3_BASE 0xFFC07000
#define HPS_TIMER0_BASE 0xFFC08000
#define HPS_TIMER1_BASE 0xFFC09000
#define HPS_TIMER2_BASE 0xFFD00000
#define HPS_TIMER3_BASE 0xFFD01000
#define FPGA_BRIDGE 0xFFD0501C

#define NIOS2_READ_STATUS(dest) \
  do {                          \
    dest = __builtin_rdctl(0);  \
  } while (0)

#define NIOS2_WRITE_STATUS(src) \
  do {                          \
    __builtin_wrctl(0, src);    \
  } while (0)

#define NIOS2_READ_ESTATUS(dest) \
  do {                           \
    dest = __builtin_rdctl(1);   \
  } while (0)

#define NIOS2_READ_BSTATUS(dest) \
  do {                           \
    dest = __builtin_rdctl(2);   \
  } while (0)

#define NIOS2_READ_IENABLE(dest) \
  do {                           \
    dest = __builtin_rdctl(3);   \
  } while (0)

#define NIOS2_WRITE_IENABLE(src) \
  do {                           \
    __builtin_wrctl(3, src);     \
  } while (0)

#define NIOS2_READ_IPENDING(dest) \
  do {                            \
    dest = __builtin_rdctl(4);    \
  } while (0)

#define NIOS2_READ_CPUID(dest) \
  do {                         \
    dest = __builtin_rdctl(5); \
  } while (0)

/* these globals are written by interrupt service routines; we have to declare
 * these as volatile to avoid the compiler caching their values in registers */
extern volatile char byte1, byte2,
    byte3;                    // modified by PS/2 interrupt service routine
extern volatile int timeout;  // used to synchronize with the timer

// NEW
int frequency = 0;
int freqInputEn;
char *frequencyInput;

void HEX_PS2(char, char, char);
void interrupt_handler(void);
void interval_timer_ISR(void);
void PS2_ISR(void);
volatile char byte1, byte2, byte3;  // PS/2 variables
volatile int timeout;               // used to synchronize with the timer

/* The assembly language code below handles CPU reset processing */
void the_reset(void) __attribute__((section(".reset")));
void the_reset(void)
/*******************************************************************************
 * Reset code. By giving the code a section attribute with the name ".reset" we
 * allow the linker program to locate this code at the proper reset vector
 * address. This code just calls the main program.
 ******************************************************************************/
{
  asm(".set		noat");      // Magic, for the C compiler
  asm(".set		nobreak");   // Magic, for the C compiler
  asm("movia		r2, main");  // Call the C language main program
  asm("jmp		r2");
}

/* The assembly language code below handles CPU exception processing. This
 * code should not be modified; instead, the C language code in the function
 * interrupt_handler() can be modified as needed for a given application.
 */
void the_exception(void) __attribute__((section(".exceptions")));
void the_exception(void)
/*******************************************************************************
 * Exceptions code. By giving the code a section attribute with the name
 * ".exceptions" we allow the linker program to locate this code at the proper
 * exceptions vector address.
 * This code calls the interrupt handler and later returns from the exception.
 ******************************************************************************/
{
  asm(".set		noat");     // Magic, for the C compiler
  asm(".set		nobreak");  // Magic, for the C compiler
  asm("subi		sp, sp, 128");
  asm("stw		et, 96(sp)");
  asm("rdctl	et, ctl4");
  asm("beq		et, r0, SKIP_EA_DEC");  // Interrupt is not external
  asm("subi		ea, ea, 4"); /* Must decrement ea by one instruction
                                      * for external interupts, so that the
                                      * interrupted instruction will be run */

  asm("SKIP_EA_DEC:");
  asm("stw	r1,  4(sp)");  // Save all registers
  asm("stw	r2,  8(sp)");
  asm("stw	r3,  12(sp)");
  asm("stw	r4,  16(sp)");
  asm("stw	r5,  20(sp)");
  asm("stw	r6,  24(sp)");
  asm("stw	r7,  28(sp)");
  asm("stw	r8,  32(sp)");
  asm("stw	r9,  36(sp)");
  asm("stw	r10, 40(sp)");
  asm("stw	r11, 44(sp)");
  asm("stw	r12, 48(sp)");
  asm("stw	r13, 52(sp)");
  asm("stw	r14, 56(sp)");
  asm("stw	r15, 60(sp)");
  asm("stw	r16, 64(sp)");
  asm("stw	r17, 68(sp)");
  asm("stw	r18, 72(sp)");
  asm("stw	r19, 76(sp)");
  asm("stw	r20, 80(sp)");
  asm("stw	r21, 84(sp)");
  asm("stw	r22, 88(sp)");
  asm("stw	r23, 92(sp)");
  asm("stw	r25, 100(sp)");  // r25 = bt (skip r24 = et, because it is saved
                                 // above)
  asm("stw	r26, 104(sp)");  // r26 = gp
  // skip r27 because it is sp, and there is no point in saving this
  asm("stw	r28, 112(sp)");  // r28 = fp
  asm("stw	r29, 116(sp)");  // r29 = ea
  asm("stw	r30, 120(sp)");  // r30 = ba
  asm("stw	r31, 124(sp)");  // r31 = ra
  asm("addi	fp,  sp, 128");

  asm("call	interrupt_handler");  // Call the C language interrupt handler

  asm("ldw	r1,  4(sp)");  // Restore all registers
  asm("ldw	r2,  8(sp)");
  asm("ldw	r3,  12(sp)");
  asm("ldw	r4,  16(sp)");
  asm("ldw	r5,  20(sp)");
  asm("ldw	r6,  24(sp)");
  asm("ldw	r7,  28(sp)");
  asm("ldw	r8,  32(sp)");
  asm("ldw	r9,  36(sp)");
  asm("ldw	r10, 40(sp)");
  asm("ldw	r11, 44(sp)");
  asm("ldw	r12, 48(sp)");
  asm("ldw	r13, 52(sp)");
  asm("ldw	r14, 56(sp)");
  asm("ldw	r15, 60(sp)");
  asm("ldw	r16, 64(sp)");
  asm("ldw	r17, 68(sp)");
  asm("ldw	r18, 72(sp)");
  asm("ldw	r19, 76(sp)");
  asm("ldw	r20, 80(sp)");
  asm("ldw	r21, 84(sp)");
  asm("ldw	r22, 88(sp)");
  asm("ldw	r23, 92(sp)");
  asm("ldw	r24, 96(sp)");
  asm("ldw	r25, 100(sp)");  // r25 = bt
  asm("ldw	r26, 104(sp)");  // r26 = gp
  // skip r27 because it is sp, and we did not save this on the stack
  asm("ldw	r28, 112(sp)");  // r28 = fp
  asm("ldw	r29, 116(sp)");  // r29 = ea
  asm("ldw	r30, 120(sp)");  // r30 = ba
  asm("ldw	r31, 124(sp)");  // r31 = ra

  asm("addi	sp,  sp, 128");

  asm("eret");
}

/******************************************************************************
 * Interrupt Service Routine
 *  Determines what caused the interrupt and calls the appropriate
 *  subroutine.
 *
 * ipending - Control register 4 which has the pending external interrupts
 ******************************************************************************/

int main(void) {
  /* Declare volatile pointers to I/O registers (volatile means that IO load
     and store instructions will be used to access these pointer locations,
     instead of regular memory loads and stores) */
  volatile int *interval_timer_ptr =
      (int *)TIMER_BASE;                    // interal timer base address
  volatile int *PS2_ptr = (int *)PS2_BASE;  // PS/2 port address

  /* initialize some variables */
  byte1 = 0;
  byte2 = 0;
  byte3 = 0;    // used to hold PS/2 data
  timeout = 0;  // synchronize with the timer

  /* set the interval timer period for scrolling the HEX displays */
  int counter = 20000000;  // 1/(100 MHz) x (20000000) = 200 msec
  *(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
  *(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;

  /* start interval timer, enable its interrupts */
  *(interval_timer_ptr + 1) = 0x7;  // STOP = 0, START = 1, CONT = 1, ITO = 1

  *(PS2_ptr) = 0xFF; /* reset */
  *(PS2_ptr + 1) =
      0x1; /* write to the PS/2 Control register to enable interrupts */

  NIOS2_WRITE_IENABLE(0x81); /* set interrupt mask bits for levels 0 (interval
                              * timer) and 7 (PS/2) */

  NIOS2_WRITE_STATUS(1);  // enable Nios II interrupts

  while (1) {
    while (!timeout)
      ;  // wait to synchronize with timer
    /* display PS/2 data (from interrupt service routine) on HEX displays */
    HEX_PS2(byte1, byte2, byte3);
    timeout = 0;
  }
}

void HEX_PS2(char b1, char b2, char b3) {
  volatile int *HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
  volatile int *HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

  /* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
   * a single 7-seg display, for the hex digits 0 - F */
  unsigned char seven_seg_decode_table[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D,
                                            0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7C,
                                            0x39, 0x5E, 0x79, 0x71};
  unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
  unsigned int shift_buffer, nibble;
  unsigned char code;
  int i;

  shift_buffer = (b1 << 16) | (b2 << 8) | b3;
  for (i = 0; i < 6; ++i) {
    nibble = shift_buffer & 0x0000000F;  // character is in rightmost nibble
    code = seven_seg_decode_table[nibble];
    hex_segs[i] = code;
    shift_buffer = shift_buffer >> 4;
  }
  /* drive the hex displays */
  *(HEX3_HEX0_ptr) = *(int *)(hex_segs);
  *(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}

void interrupt_handler(void) {
  int ipending;
  NIOS2_READ_IPENDING(ipending);
  if (ipending & 0x01)  // interval timer is interrupt level 0
  {
    interval_timer_ISR();
  }
  if (ipending & 0x80)  // PS/2 port is interrupt level 7
  {
    PS2_ISR();
  }
  // else, ignore the interrupt
  return;
}

void PS2_ISR(void) {
  volatile int *PS2_ptr = (int *)PS2_BASE;  // PS/2 port address
  int PS2_data, RAVAIL;

  PS2_data = *(PS2_ptr);         // read the Data register in the PS/2 port
  RAVAIL = (PS2_data & 0x8000);  // extract the RAVAIL field
  if (RAVAIL > 0) {
    /* always save the last three bytes received */

    byte1 = byte2;
    byte2 = byte3;
    byte3 = PS2_data & 0xFF;

    if (byte2 == (char)0xF0 && byte3 == (char)0x43) {  // 1
      printf("I pressed");
      freqInputEn = 1;
    }

    // only takes num input if key I pressed first
    if (freqInputEn) {
      if (byte2 == (char)0xF0 && byte3 == (char)0x45) {  // 1
        printf("0 pressed");
        frequencyInput += '0';
      }
      if (byte2 == (char)0xF0 && byte3 == (char)0x16) {  // 1
        printf("1 pressed");
        frequencyInput += '1';
      }
      if (byte2 == (char)0xF0 && byte3 == (char)0x1E) {  // 1
        printf("2 pressed");
        frequencyInput += '2';
      }
      if (byte2 == (char)0xF0 && byte3 == (char)0x26) {  // 1
        printf("3 pressed");
        frequencyInput += '3';
      }
      if (byte2 == (char)0xF0 && byte3 == (char)0x25) {  // 1
        printf("4 pressed");
        frequencyInput += '4';
      }
      if (byte2 == (char)0xF0 && byte3 == (char)0x2E) {  // 1
        printf("5 pressed");
        frequencyInput += '5';
      }
      if (byte2 == (char)0xF0 && byte3 == (char)0x36) {  // 1
        printf("6 pressed");
        frequencyInput += '6';
      }
      if (byte2 == (char)0xF0 && byte3 == (char)0x3D) {  // 1
        printf("7 pressed");
        frequencyInput += '7';
      }
      if (byte2 == (char)0xF0 && byte3 == (char)0x3E) {  // 1
        printf("8 pressed");
        frequencyInput += '8';
      }
      if (byte2 == (char)0xF0 && byte3 == (char)0x46) {  // 1
        printf("9 pressed");
        frequencyInput += '9';
      }
      if (byte2 == (char)0xF0 && byte3 == (char)0x66) {  // 1
        printf("Backspace pressed");
      }
    }
  }
  if (frequency < 9999) {  // check frequency within bounts
    if (byte2 == (char)0xF0 && byte3 == (char)0x2D) {  // 1
      printf("R pressed");
    }
    if (byte2 == (char)0xF0 && byte3 == (char)0x1B) {  // 1
      printf("S pressed");
    }
    if (byte2 == (char)0xF0 && byte3 == (char)0x15) {  // 1
      printf("Q pressed");
    }
    if (byte2 == (char)0xF0 && byte3 == (char)0x2C) {  // 1
      printf("T pressed");
    }
    if (byte2 == (char)0xF0 && byte3 == (char)0x1D) {  // 1
      printf("W pressed");
    }
  }

  // printf("another key pressed");

  if ((byte2 == (char)0xAA) && (byte3 == (char)0x00))
    // mouse inserted; initialize sending of data
    *(PS2_ptr) = 0xF4;

  return;
}

void interval_timer_ISR() {
  volatile int *interval_timer_ptr = (int *)TIMER_BASE;

  *(interval_timer_ptr) = 0;  // clear the interrupt
  timeout = 1;                // set global variable

  return;
}