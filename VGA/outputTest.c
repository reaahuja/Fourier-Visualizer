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