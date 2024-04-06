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
volatile char byte1, byte2, byte3;
void PS2Poll(void);
char *frequencyInput;
int freqInputEn = 0;
int frequency = 0;

int main(void) {
  /* Declare volatile pointers to I/O registers (volatile means that IO load
     and store instructions will be used to access these pointer locations,
     instead of regular memory loads and stores) */
  volatile int *PS2_ptr = (int *)PS2_BASE;  // PS/2 port address

  /* initialize some variables */
  byte1 = 0;
  byte2 = 0;
  byte3 = 0;  // used to hold PS/2 data

  *(PS2_ptr) = 0xFF; /* reset */
  *(PS2_ptr + 1) =
      0x1; /* write to the PS/2 Control register to enable interrupts */

  while (1) {
    PS2Poll();
  }
}

void PS2Poll(void) {
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
  if (frequency < 9999 && frequency > 0) {  // check frequency within bounts
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
    // ps2 device inserted
    *(PS2_ptr) = 0xF4;

  return;
}
