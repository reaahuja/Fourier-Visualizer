/*
Task: 
1. Takes input audio from microphone for 1 second once KEY 1 is pressed
2. Store the input audio into an array 
3. Pass the input audio array to the fft algorithim 
4. Retrieve the manipulated array and change convert it to dB for the y-axis
5. Make an array for the x-axis that is in Hz instead of k
*/

//NOTE: Need to figure out hex connection 
#include <string.h>
#include <math.h>
#define audioLength 5
#define timerValue 50000000;
#define audioSamples (audioLength *8000)
#define maxFrequency 10000

struct audio_t{
    volatile unsigned int control; 
    volatile unsigned char  rarc; 
    volatile unsigned char  ralc; 
    volatile unsigned char  wsrc; 
    volatile unsigned char  wslc; 
    volatile unsigned int  left; 
    volatile unsigned int  right;  
};

struct audio_t* const audioptr = ((struct audio_t*) 0xFF203040);
// float leftAudio[audioLength*8000] = {0};
// float rightAudio[audioLength*8000] = {0};
float inputAudio[audioSamples] = {0};
float fftRealAudio[audioSamples] = {0};
float fftImgAudio[audioSamples] = {0};
float fftAudioMag[audioSamples] = {0}; //array for y axis 
float frequencyX[maxFrequency] = {0}; //array for x axis

void microphoneRecording();
void microphoneOutput(); 
void fftSetUp(); 
void fftAlteration(int);

void fftSetUp(){
    memcpy(fftRealAudio, inputAudio, sizeof(audioSamples));
    fft(fftRealAudio, fftImgAudio,audioSamples);
    //Retrieve 20*log_base10(||weight||) for y-axis
    for(int i = 0; i < audioSamples; i++){
        int magnitude = sqrt(pow(fftRealAudio[i], 2) + pow(fftImgAudio[i], 2));
        fftAudioMag[i] = 20*log10(magnitude);
    }
    for (int i  = 0; i < maxFrequency; i++){
        frequencyX[i] = i*(8000/audioSamples);
    }
}

void fftAlteration(int switchValue){
    /*
    Task: If the user wants to alter the amount of the signal used, need to recompute FFT
    1. Determine the new range of the audio 
    2. Perform FFT on it to retrieve a revised copy of the audio 
    */

   //User can toggle switches to retrieve 1/2 second, so putting up all 10 keys would mean 5 seconds
   double alteredAudioLength = (log2(switchValue)+ 1)*(0.5);
   int audioSamplesAltered = ((int)alteredAudioLength)*8000; 
   if (audioSamplesAltered == 0){
        audioSamplesAltered = audioSamples;
   }

   float fftRealAltered[audioSamplesAltered];
   float fftImgAltered[audioSamplesAltered];
   float fftAudioMagAltered[audioSamplesAltered];

    fft(fftRealAltered, fftRealAltered, audioSamplesAltered);
    //Retrieve 20*log_base10(||weight||) for y-axis
    for(int i = 0; i < audioSamples; i++){
        int magnitude = sqrt(pow(fftRealAltered[i], 2) + pow(fftRealAltered[i], 2));
        fftAudioMagAltered[i] = 20*log10(magnitude);
    }


}


void microphoneRecording(struct audio_t* audioPtr, int *leftAudio, int *rightAudio){
    //LEDS will go high during recording and timer will count down from 5
    volatile int *ledPtr = (int *)(0xff200000);
    volatile int* timerPtr = (int*)(0xFF202000);
    volatile int* AUDIO = 0xFF203040; 
    configTimer();

    audioptr->ralc = 0; 
    audioptr->ralc = 0; 
    audioptr->wslc = 128; 
    audioptr->wsrc = 128; 
     
    *(timerPtr) = 0;
    *(timerPtr + 1) = 0x4;
    int timing = audioLength; 
    for (int i = 0; (timing != 0); i++){
        // leftAudio[i] =  audioptr->left; 
        // rightAudio[i] = audioptr->right; 
        int leftAudio =  audioptr->left; 
        int rightAudio = audioptr->right;

        inputAudio[i] = (leftAudio + rightAudio)/2;

        *(ledPtr) = 0x3ff;

        if ((*timerPtr & 0x1) == 0x1){
            *(timerPtr) = 0;
            *(timerPtr + 1) = 0x4;
            timing--; 
        }
        i = i % (audioSamples);
    }
    *(ledPtr) = 0x0;
}

void microphoneOutput(){
    for (int i = 0; i <audioSamples; i++){
        audioptr->left  = inputAudio[i];
        audioptr->right = inputAudio[i];
        // audioptr->left = leftAudio[i]; 
        // audioptr->right = rightAudio[i]; 
    }
}

void configTimer() {
  volatile int* timerPtr = (int*)(0xFF202000);
  // 100 Hz = 100 cycles / second, 1 cycle / 0.01 s
  // 100 MHz clock on board = 100000000 cycles / second, 1000000 cycles / 0.01 s
  int timerVal = timerValue;
  int lowBits = timerVal & 0x0000ffff;   // get low 16b of timer value
  int highBits = timerVal & 0xffff0000;  // get high 16b of timer value

  *(timerPtr + 1) = 0x8;  // write 1 to STOP bit to stop timer (1000)
  *(timerPtr) = 0;        // write 0 to TO bit to reset time out bit in status
                          // register
  *(timerPtr + 2) = lowBits;
  *(timerPtr + 3) = highBits;

  return;
}

