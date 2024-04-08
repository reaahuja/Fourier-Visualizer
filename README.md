# Discrete Fourier Series Visualizer

This program computes the discrete Fourier series of a given input using the Fast Fourier Transform (FFT) algorithm and displays its frequency spectrum (magnitude of the output signal plotted against frequency, in Hz) on the VGA. Input can be taken through an external microphone or can be chosen from a list of pre-selected waves (triangle, square, sine, or sawtooth) with frequency selected by the user using the keyboard. The FFT can be performed on a variety of samples without restarting the program: toggling switch 0 will prompt the user for a new kind of input, and the algorithm will restart and display the new graph.

## Instructions to Execute Program:

- **Press Key 0** to begin the program.
- **Press Key 1** to select wave input or **Press Key 2** to select microphone input.
  - If **Key 1** is pressed (for wave selection):
    - Enter the frequency of the wave that you would like to create by entering numbers on the PS2 Keyboard (the numbers should be between 50-500 Hz; numbers outside of this range will be rounded up to 50 or down to 500).
    - Backspace can also be used in the event of user input error.
    - Click enter on the PS2 Keyboard to confirm the frequency; once confirmed, this frequency will be displayed in binary on the LEDs.
    - Select the shape of the wave that you would like to create by clicking one of the following keys on the PS2 keyboard:
      - `s` for sine,
      - `w` for sawtooth,
      - `t` for triangle,
      - `q` for square.
  - If **Key 2** is pressed (for microphone input):
    - Provide input to the microphone for 5 seconds - the hex display will countdown to show how much time is left to provide the input.
    - Listen to the audio to hear the recorded audio.
    - Wait for the Fast Fourier Transform computation to complete and enjoy the music generated while waiting.
    - Once the computation is complete, the frequency spectrum will be displayed on the VGA monitor.
- Once rendering is complete, toggle switch 0 (move it up or move it down) to restart the program and return to the selection screen (where you can choose to input another wave or record audio on the microphone).
