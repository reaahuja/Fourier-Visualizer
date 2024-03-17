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
        - If all frequencies are in Hz
            - If most frequencies < 1Hz, ticks will be placed at 0.1 Hz sequences, and 
            axis will be drawn in GREEN 
            - If most frequencies > 1Hz but < 100 Hz, ticks will be placed at 1Hz sequencies 
            and axis will be drawn in BLUE
            - If most frequencies > 100 Hz but < 1000Hz, ticks will be placed at 100Hz sequencies
            and axis will be drawn in YELLOW
        - If all or some frequencies are in KHz, then draw axis in red 
            - If most frequencies > 1KHz but < 10KHz, ticks will be placed at 1KHz sequencies
            and axis will be drawn in RED
            - If most frequencies > 10KHz but < 100KHz, ticks will be placed at 10KHz sequencies
            and axis will be drawn in PINK
    b. For y axis: 
        - Determine 
3. Draw lines for each of the values in the array in the corresponding position with respect
to the y axis and x axis scales
*/