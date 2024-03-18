// Code for testing VGA output (with double buffering)

/*
Input: An array from fft.c
Output: A visualization of the input array 
Purpose: This code will recieve an input array from fft.c, and display it on the VGA by 
making a scale for a graph to be drawn on, and then correspondingly drawing all of the lines
*/

/*
Steps: 
1. Retrieve the array, and determine the max value for the x (frequency) and y axis (weight)
2. Draw a line for the x axis and y axis, and ticks for scale
    a. For x axis: 
        - Retrieve max value 
        - Retrieve min value 
        - Make a tick every 1Hz, 10Hz, 100Hz, 1KHz, or 10KHz
            - Inform user about the scale through Hex Display/LEDs/Axis colour/Audio output
    b. For y axis: 
        - Retrieve max value 
        - Retrieve min value
        - Make a tick every 1, 10, 100, or 1000 units 
            - Inform user about the scale through Hex Display/LEDs/Axis colour/Audio output
3. Draw lines for each of the values in the array in the corresponding position with respect
to the y axis and x axis scales
*/

#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h> 

#define n 8 //stores size of fftArray 
#define nColours 6 //stores size of colours array (# of colours)
#define baseXY 5 //stores how far the axis will be from the edge of the screen
#define totalTicks 10
#define PINK 0xFC18
#define ORANGE 0xFC00
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define WHITE 0xFFFF


float fftArray[n] = {4.3457, 3.35325, 5.5235, 6.431, 5.53, 7.6354, 1.0134, 9.413};
int colours[nColours] = {GREEN, BLUE, RED, PINK, ORANGE, WHITE};
int mode[nColours] = {0.1, 1, 10, 100, 1000, 10000};
float maxX, minX, maxY, minY; 
int xAxisColour, yAxisColour; 
int currentMode; 
int xTickLocations[totalTicks];
int yTickLocations[totalTicks];

volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
int pixel_buffer_start;

void initalSetUp();
void setColour(int *axisColour, float *maxValue);

//draw axis and its helper functions
void drawAxis(int x0, int y0, int x1, int y1, int colour); 
    void swap(int* var1, int* var2);
    void plot_pixel(int x, int y, short int line_color);

void drawTicks(); 
//void drawWeights(); 
void clear_screen(); 


int main(){
    clear_screen(); 
    //fills max and mins values, and sets colours
    initalSetUp(); 
    //draw x axis 
    drawAxis(baseXY, 239 - baseXY, 319 - baseXY, 239 - baseXY, xAxisColour); 
    //draw y axis
    drawAxis(baseXY, baseXY, baseXY, 239 - baseXY, yAxisColour); 
    drawTicks();
    drawWeights();
}

void drawWeights(){
    /*
    1. Iterate through samples in array 
    2. Draw line for weight based on arrangement of ticks 
    */
   for (int i = 0; i < n; i++){
     int decimalPart = (fftArray[i] - floor(fftArray[i]))*10;
     int accuracy = abs(yTickLocations[0] - yTickLocations[1])/10; 
     int yLocation = yTickLocations[(int) floor(fftArray[i])] + accuracy*decimalPart;
     drawAxis(xTickLocations[i + 1], 239-baseXY, xTickLocations[i + 1], yLocation,WHITE); 
   }
}

void drawTicks(){
    //draw ticks along x axis 
    //int tickSpacingX = (319-baseXY)/maxX; 
    //int tickSpacingY = (239-baseXY)/maxY; 
    int tickSpacingX = (319-baseXY)/totalTicks; 
    int tickSpacingY = (239-baseXY)/totalTicks; 

    int x = baseXY;  
    for (int i = 0; i < 10; i++){
        xTickLocations[i] = x; 
        drawAxis(x, 239 - 2*baseXY, x, 239 - baseXY, WHITE); 
        x += tickSpacingX; 
    }

    int y = 239-baseXY;  
    for (int i = 0; i < 10; i++){
        yTickLocations[i] = y;
        drawAxis(baseXY, y, 2*baseXY, y, WHITE); 
        y -= tickSpacingY; 
    }
}

void initalSetUp(){
    /*
    - maxX will store the highest frequency 
    - minX will store the smallest frequency
    - maxY will store the greatest amplitude
    - minY will store the smallest amplitude 
    */
    maxX = n;
    minX = 0;
    maxY  = fftArray[0];
    minY = fftArray[0];

    for (int i = 1; i < n; i++){
        if (fftArray[n] > maxY){
            maxY = fftArray[n]; 
        }

        if (fftArray[n] < minY){
            minY = fftArray[n]; 
        }
    }
    
    setColour(&xAxisColour, &maxX);
    setColour(&yAxisColour, &maxY);
   
    //for safety so axis is actually drawn 
    if ((minY == maxY) && maxY != 0){
        minY = (-1)*maxY; 
    }else if (maxY == 0){
        maxY = 10; 
        minY = 0; 
    }
}

void setColour(int *axisColour, float *maxValue){
     //determines how big the x/y value is depending on how many digits it has 
    int zerosCounter = -1; 
    int tempMax =  abs(*maxValue); 
    while (tempMax > 0){
        tempMax /= 10; 
        zerosCounter++; 
    }

    //sets a colour for the xAxis/yAxis depending on how big the max x/y value is 
    if(zerosCounter < nColours){
        *axisColour = colours[zerosCounter];
        currentMode = mode[zerosCounter];
    }else{
        *axisColour = WHITE; 
        currentMode = 1;
    }

   
}


void clear_screen(){
    /*
    Task: clear screen by drawing black at every pixel in screen 
    */
    for (int y = 0; y <= 239; y++){
        for (int x = 0; x <= 319; x++){
            plot_pixel(x, y, 0x0000);
        }
    }
}

void drawAxis(int x0, int y0, int x1, int y1, int colour){
    /*
    Task: Implement Bresenham’s algorithim 
    */

   bool is_steep = abs(y1  - y0) > abs(x1 - x0);

   if(is_steep){
        swap(&x0, &y0);
        swap(&x1, &y1); 
   }
   if(x0 > x1){
        swap(&x0, &x1);
        swap(&y0, &y1);
   }

   int deltax = x1 - x0, deltay = abs(y1 - y0); 
   int error = -(deltax/2);
   int y = y0, y_step = 0;

   if (y0 < y1){
    y_step = 1;
   }else{
    y_step = -1;
   }

   for (int x = x0; x <= x1; x++){
    if(is_steep){
        plot_pixel(y, x, colour);
    }else{
        plot_pixel(x, y, colour);
    }
    error = error + deltay; 
    if(error > 0){
        y = y + y_step; 
        error = error - deltax; 
    }
   }

}

void swap(int* var1, int* var2){
    int temp = *var1; 
    *var1 = *var2; 
    *var2 = temp; 
}

void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;
    pixel_buffer_start = *(pixel_ctrl_ptr);
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);

    *one_pixel_address = line_color;
}

