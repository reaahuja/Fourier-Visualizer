// Code for testing VGA output (with double buffering)

/*
Input: An array from fft.c
Output: A visualization of the input array
Purpose: This code will recieve an input array from fft.c, and display it on the
VGA by making a scale for a graph to be drawn on, and then correspondingly
drawing all of the lines
*/

/*
Steps:
1. Retrieve the array, and determine the max value for the x (frequency) and y
axis (weight)
2. Draw a line for the x axis and y axis, and ticks for scale
    a. For x axis:
        - Retrieve max value
        - Retrieve min value
        - Make a tick every 1Hz, 10Hz, 100Hz, 1KHz, or 10KHz
            - Inform user about the scale through Hex Display/LEDs/Axis
colour/Audio output b. For y axis:
        - Retrieve max value
        - Retrieve min value
        - Make a tick every 1, 10, 100, or 1000 units
            - Inform user about the scale through Hex Display/LEDs/Axis
colour/Audio output
3. Draw lines for each of the values in the array in the corresponding position
with respect to the y axis and x axis scales
*/

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define n 8         // stores size of fftArray
#define nColours 6  // stores size of colours array (# of colours)
#define baseXY \
  40  // stores how far the axis will be from the edge of the screen
#define totalTicks 10
#define PINK 0xFC18
#define ORANGE 0xFC00
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define WHITE 0xFFFF
#define Y_RESOLUTION 240
#define X_RESOLUTION 320
#define VGA_X 80
#define VGA_Y 60
#define MAX_WIDTH = 8
#define VGA_ADDR 0x08000000
#define CHAR_ADDR 0x09000000

float fftArray[n] = {4.3457, 3.35325, 5.5235, 6.431,
                     5.53,   7.6354,  1.0134, 9.413};
int colours[nColours] = {GREEN, BLUE, RED, PINK, ORANGE, WHITE};
int mode[nColours] = {0.1, 1, 10, 100, 1000, 10000};

float maxX, minX, maxY, minY;
int xAxisColour, yAxisColour;
int currentMode;
int xTickLocations[totalTicks + 1];
int yTickLocations[totalTicks + 1];

volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
int pixel_buffer_start;
int numHarmonics = sizeof(fftArray) /
                   sizeof(float);  // get length of array (i.e. num elements)

void initalSetUp();
void setColour(int *axisColour, float *maxValue);

// draw axis and its helper functions
void drawAxis(int x0, int y0, int x1, int y1, int colour);
void swap(int *var1, int *var2);
void plot_pixel(int x, int y, short int line_color);

void drawTicks();
void drawWeights();
void drawAxisLabels();
void clear_screen();
void write_char(int x, int y, char c);
void write_string(int x, int y, char *arr);
void clear_x_labels();
void clear_characters();
void clear_x_title();

int main() {
  clear_screen();
  // fills max and mins values, and sets colours
  initalSetUp();
  drawAxis(baseXY, Y_RESOLUTION - 1 - baseXY, X_RESOLUTION - 1 - baseXY,
           Y_RESOLUTION - 1 - baseXY, xAxisColour);  // x axis drawing
  drawAxis(baseXY, baseXY, baseXY, Y_RESOLUTION - 1 - baseXY,
           yAxisColour);  // y axis draing
  drawAxisLabels();
  drawTicks();
  // draw weighted amplitudes
  drawWeights();
}

void drawAxisLabels() {
  char *yLabel = "Magnitude of Signal";
  int y = VGA_Y / 3;
  while (*yLabel != 0) {
    write_char(2, y, *yLabel);
    y++;
    yLabel++;
  }

  char *xLabel = "Frequency (Hz)";
  int x = (VGA_X - 10) / 2;
  write_string(x, 55, xLabel);
}

void write_char(int x, int y, char c) {
  volatile char *character_buffer = (char *)(CHAR_ADDR + (y << 7) + x);
  *character_buffer = c;
}

void clear_chars() {
  for (int x = 0; x < VGA_X; x++) {
    for (int y = 0; y < VGA_Y; y++) {
      volatile char *character_buffer = (char *)(CHAR_ADDR + (y << 7) + x);
      *character_buffer = 0;
    }
  }
}

void clear_x_labels() {
  for (int x = 0; x < VGA_X; x++) {
    volatile char *character_buffer =
        (char *)(CHAR_ADDR + ((VGA_Y + 1 - baseXY / 8 * 2) << 7) + x);
    *character_buffer = 0;
  }
}

void clear_x_title() {
  for (int x = 0; x < VGA_X; x++) {
    volatile char *character_buffer = (char *)(CHAR_ADDR + (55 << 7) + x);
    *character_buffer = 0;
  }
}

void write_string(int x, int y, char *arr) {
  while (*arr) {
    write_char(x, y, *arr);  //
    x++;
    arr++;
  }
}

void drawWeights() {
  /*
  1. Iterate through samples in array
  2. Draw line for weight based on arrangement of ticks
  */
  for (int i = 0; i < n; i++) {
    int decimalPart = (fftArray[i] - floor(fftArray[i])) * 10;
    int accuracy = abs(yTickLocations[0] - yTickLocations[1]) / 10;
    int yLocation =
        yTickLocations[(int)floor(fftArray[i])] + accuracy * decimalPart;
    drawAxis(xTickLocations[i + 1], 239 - baseXY, xTickLocations[i + 1],
             yLocation, ORANGE);
  }
}

void drawTicks() {  // Note: can toggle max X to change scale bounds
  int pixelsPerXTick =
      (X_RESOLUTION - 2 * baseXY) / totalTicks;  // length of x axis on screen
  int pixelsPerYTick =
      (Y_RESOLUTION - 2 * baseXY) / totalTicks;  // length of y axis on screen
  double valPerXTick = ceil((maxX - minX) / totalTicks);
  double valPerYTick = ceil((maxY - minY) / totalTicks);

  int x = baseXY;  // starting position for resolution
  int xVGA = 10;   // starting position for VGA char buffer
  clear_x_labels();
  clear_x_title();

  for (int i = 0; i <= totalTicks; i++) {
    xTickLocations[i] = x;  // where the ith tick is
    drawAxis(x, Y_RESOLUTION - baseXY + 2, x, Y_RESOLUTION - baseXY - 4, PINK);
    int num = i * valPerXTick;
    char value[100];
    if (valPerXTick >= 1000)
      sprintf(value, "%d", num / 1000);  // if 4 or 5 digits, change to kHz
    else
      sprintf(value, "%d", num);
    // if 1 digit
    if ((int)(num / 10) == 0) {
      write_string(xVGA, VGA_Y + 1 - baseXY / 8 * 2, value);
      write_string((VGA_X - 10) / 2, 55, "Frequency (Hz)");
    }

    // if 2 digits
    else if ((int)((num - 1) / 100) == 0) {
      write_string(xVGA - 1, VGA_Y + 1 - baseXY / 8 * 2, value);
      write_string((VGA_X - 10) / 2, 55, "Frequency (Hz)");
    }

    // if 3 digits
    else if ((int)(num / 1000) == 0 && maxX != 1100) {
      write_string(xVGA - 2, VGA_Y + 1 - baseXY / 8 * 2, value);
      write_string((VGA_X - 10) / 2, 55, "Frequency (Hz)");
    }

    // if 4 digits - change scale to kHz and use 1 digit shift
    else if ((int)(num / 10000) == 0 || (num / 10000) == 1) {
      write_string(xVGA, VGA_Y + 1 - baseXY / 8 * 2, value);
      write_string((VGA_X - 10) / 2, 55, "Frequency (kHz)");
    }

    // if 5 digits - change scale to kHz and use 2 digit shift
    else if ((int)(num / 100000) == 0 || (num / 100000) == 1) {
      write_string(xVGA - 1, VGA_Y + 1 - baseXY / 8 * 2, value);
      write_string((VGA_X - 10) / 2, 55, "Frequency (kHz)");
    }

    x += pixelsPerXTick;
    xVGA += pixelsPerXTick / 4;
  }

  int y = Y_RESOLUTION - 1 - baseXY;
  int yVGA = 49;
  for (int j = 0; j <= totalTicks; j++) {
    yTickLocations[j] = y;  // where the ith tick is
    drawAxis(baseXY - 2, y, baseXY + 2, y, PINK);
    int num = j * valPerYTick;
    char value[100];  // max 16 digit nums accepted
    if (num > 999) {
      int power = (int)log10(fabs(num));
      float radix = num / pow(10, power);
      gcvt(radix, 2, value);
      char tmp[6];
      char exponent[2] = "e";
      strcat(value, exponent);
      sprintf(tmp, "%d", power);
      strcat(value, tmp);
      if ((ceil(num / pow(10, power)) - floor(num / pow(10, power))) < 0.1) {
        write_string(VGA_X - baseXY * 2 + (8 - (log10(power) + 1)), yVGA,
                     value);
      } else
        write_string(VGA_X - baseXY * 2 + (7 - (log10(power) + 1)), yVGA,
                     value);  // shift over
    } else {
      sprintf(value, "%d", num);
      // if 1 digit
      if ((int)(num / 10) == 0)
        write_string(VGA_X - baseXY * 2 + 8, yVGA, value);
      // if 2 digits
      else if ((int)(num / 100) == 0)
        write_string(VGA_X - baseXY * 2 + 7, yVGA, value);
      // if 3 digits
      else
        write_string(VGA_X - baseXY * 2 + 6, yVGA, value);
    }
    y -= pixelsPerYTick;
    yVGA -= pixelsPerYTick / 4;
  }
}

void initalSetUp() {
  /*
  - maxX will store the highest frequency
  - minX will store the smallest frequency
  - maxY will store the greatest amplitude
  - minY will store the smallest amplitude
  */
  maxX = 100;
  minX = 0;
  maxY = fftArray[0];
  minY = fftArray[0];

  for (int i = 1; i < n; i++) {
    if (fftArray[n] > maxY) {
      maxY = fftArray[n];
    }

    if (fftArray[n] < minY) {
      minY = fftArray[n];
    }
  }

  setColour(&xAxisColour, &maxX);
  setColour(&yAxisColour, &maxY);

  // for safety so axis is actually drawn
  if ((minY == maxY) && maxY != 0) {
    minY = (-1) * maxY;
  } else if (maxY == 0) {
    maxY = 10;
    minY = 0;
  }
}

void setColour(int *axisColour, float *maxValue) {
  // determines how big the x/y value is depending on how many digits it has
  int zerosCounter = -1;
  int tempMax = abs(*maxValue);
  while (tempMax > 0) {
    tempMax /= 10;
    zerosCounter++;
  }

  // sets a colour for the xAxis/yAxis depending on how big the max x/y value is
  if (zerosCounter < nColours) {
    *axisColour = colours[zerosCounter];
    currentMode = mode[zerosCounter];
  } else {
    *axisColour = WHITE;
    currentMode = 1;
  }
}

void clear_screen() {
  /*
  Task: clear screen by drawing black at every pixel in screen
  */
  for (int y = 0; y <= Y_RESOLUTION - 1; y++) {
    for (int x = 0; x <= X_RESOLUTION - 1; x++) {
      plot_pixel(x, y, 0x0000);
    }
  }
}

void drawAxis(int x0, int y0, int x1, int y1, int colour) {
  /*
  Task: Implement Bresenham’s algorithim
  */

  bool is_steep = abs(y1 - y0) > abs(x1 - x0);

  if (is_steep) {
    swap(&x0, &y0);
    swap(&x1, &y1);
  }
  if (x0 > x1) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }

  int deltax = x1 - x0, deltay = abs(y1 - y0);
  int error = -(deltax / 2);
  int y = y0, y_step = 0;

  if (y0 < y1) {
    y_step = 1;
  } else {
    y_step = -1;
  }

  for (int x = x0; x <= x1; x++) {
    if (is_steep) {
      plot_pixel(y, x, colour);
    } else {
      plot_pixel(x, y, colour);
    }
    error = error + deltay;
    if (error > 0) {
      y = y + y_step;
      error = error - deltax;
    }
  }
}

void swap(int *var1, int *var2) {
  int temp = *var1;
  *var1 = *var2;
  *var2 = temp;
}

void plot_pixel(int x, int y, short int line_color) {
  volatile short int *one_pixel_address;
  pixel_buffer_start = *(pixel_ctrl_ptr);
  one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
  *one_pixel_address = line_color;
}
