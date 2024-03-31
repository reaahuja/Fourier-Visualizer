/*
Task: 
1. Takes input audio from microphone for 1 second once KEY 1 is pressed
2. Store the input audio into an array 
3. Pass the input audio array to the fft algorithim 
4. Retrieve the manipulated array and change convert it to dB for the y-axis
5. Make an array for the x-axis that is in Hz instead of k
*/

//THIS CODE GAVE HIGHEST FREQUENCY AT 250HZ, seemed to work TEST IN LAB (march 31st)
// code: https://replit.com/@ReaAhuja/ECE243AudioWORKINGCOPY#main.c (in case I delete stuff somehow)

//NOTE: TEST THIS IN LAB
#include <string.h>
#include <math.h>
#include <stdio.h>
#define audioLength 5
#define timerValue 50000000
//#define audioSamples (audioLength *8000 + 128*2)
#define audioSamples 65536
#define maxFrequency 1000
#define HEX_BASE 0xFF200020 
int samplesObtained = 0; 

void microphoneRecording();
void microphoneOutput(); 
void fftSetUp(); 
void fftAlteration(int);
void displayHexDigit(int, int);
void configureTimer(volatile int* timingPTR, int timeVal);
void fft(float data_re[], float data_im[], const unsigned int N);
void rearrange(float data_re[], float data_im[],
               const unsigned int N);  // Sort array
void compute(float data_re[], float data_im[],
             const unsigned int N);  // Compute DTFT w FFT algorithm

struct audio_t{
    volatile unsigned int control; 
    volatile unsigned char  rarc; 
    volatile unsigned char  ralc; 
    volatile unsigned char  wsrc; 
    volatile unsigned char  wslc; 
    volatile unsigned int  left; 
    volatile unsigned int  right;  
};

unsigned char hex_map[] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71  // F
};


struct audio_t* const audioptr = ((struct audio_t*) 0xFF203040);
volatile int* switches = (int*) (0xFF200040); 
volatile int* keys = (int*)(0xFF200050);

int audioLeft[audioSamples] = {0};
int audioRight[audioSamples] = {0};
float inputAudio[audioSamples] = {0};
float fftRealAudio[audioSamples] = {0};
float fftImgAudio[audioSamples] = {0};
float fftAudioMag[audioSamples] = {0}; //array for y axis 
float frequencyX[maxFrequency] = {0}; //array for x axis


int main(void){
    *(keys + 3) = 0xf;
    while (!(*(keys + 3) & 0x2));
    microphoneRecording();
    microphoneOutput();
    printf("Entering fftsetup \n");
    fftSetUp(); 

    // if (*(switches) > 0){
    //     fftAlteration(*(switches));
    // }
}

// void fftSetUp(){
//     memcpy(fftRealAudio, inputAudio, sizeof(audioSamples));
//     fft(fftRealAudio, fftImgAudio, audioSamples);
//     //Retrieve 20*log_base10(||weight||) for y-axis
//     for(int i = 0; i < audioSamples; i++){
//         int magnitude = sqrt(pow(fftRealAudio[i], 2) + pow(fftImgAudio[i], 2));
//         fftAudioMag[i] = 20*log10(magnitude);
//     }
//     for (int i  = 0; i < maxFrequency; i++){
//         frequencyX[i] = i*(8000/audioSamples);
//     }
// }

void fftSetUp(){
  for (int i = 0; i < audioSamples; i++){
    fftRealAudio[i] = 1.0 * inputAudio[i];
  }
    //memcpy(fftRealAudio, inputAudio, sizeof(inputAudio));

    printf("Beginning fft computation \n");
    fft(fftRealAudio, fftImgAudio, audioSamples);
    printf("Ending fft computation \n");
    //Retrieve 20*log_base10(||weight||) for y-axis
    int largestFrequency = 0; 
    float largestFrequencyMag = 0; 
    int abslargestFrequency = 0; 
    float abslargestFrequencMag = 0; 

    for(int i = 0; i < audioSamples/2; i++){
        float magnitude = sqrt(pow(fftRealAudio[i], 2) + pow(fftImgAudio[i], 2));
        if (magnitude == 0){
          fftAudioMag[i] = 0;
        }else{
          fftAudioMag[i] = 20*log10(magnitude);
        }
        

        if (magnitude > largestFrequencyMag){
            largestFrequencyMag = magnitude; 
            largestFrequency = i * 8000/audioSamples; 
        }
        if (abs(magnitude) > abslargestFrequencMag){
            abslargestFrequencMag = magnitude; 
            abslargestFrequency = i * 8000/audioSamples; 
        }
    }

    printf("Largest frequency index %d, largest frequency value %f", largestFrequency, largestFrequencyMag);
    printf("Largest abs frequency index %d, largest abs frequency value %f", abslargestFrequency, abslargestFrequencMag);

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
        float magnitude = sqrt(pow(fftRealAudio[i], 2) + pow(fftImgAudio[i], 2));
        if(magnitude > 0){
           fftAudioMagAltered[i] = 20*log10(magnitude);
        }else{
          fftAudioMagAltered[i] = magnitude;
        }
    }


}


void microphoneRecording(){
    //LEDS will go high during recording and timer will count down from 5
    volatile int *ledPtr = (int *)(0xff200000);
    volatile int* timerPtr = (int*)(0xFF202000); 
    int timingLength = (5*(pow(10, 8)));
    configureTimer(timerPtr, timingLength);
     
    *(timerPtr + 1) = 0x4;
    int timing = 0; 
    int oneSecond = 0;
    int i = 0;
    for (;(((*timerPtr) & 1) != 1) ; i++){

        while(!(audioptr->ralc > 0 && audioptr->rarc > 0)); //wait when there is no input

        int leftAudio =  audioptr->left; 
        int rightAudio = audioptr->right;
        //printf("leftAudio: %d \n", leftAudio);
        //printf("rightAudio: %d \n", rightAudio);
        //audioptr->left = leftAudio; 
        //audioptr->right = rightAudio; 
        inputAudio[i] = (leftAudio + rightAudio)/2;
        audioLeft[i] = leftAudio; 
        audioRight[i] = rightAudio;

        *(ledPtr) = 0x3ff;
        //i = i % (audioSamples);

        if(oneSecond == 8000){
            displayHexDigit(timing, 0);
            oneSecond = 0; 
            timing++;
            printf("timing: %d \n", timing);
        }

        //wait until next sample has been loaded in fifospace
        //*(samplePtr + 1) = 0x4;
        //while(((*samplePtr) & 1) != 1);
       // configureTimer(samplePtr, 12500);
        oneSecond++;
        //printf("i: %d \n", i);
        //printf("oneSecond: %d \n", oneSecond);
    }
    displayHexDigit(timing + 1, 0);
    samplesObtained = i; 
    printf("samples: %d \n", i);
    *(ledPtr) = 0x0;
}

void microphoneOutput(){
     printf("audioSamples: %d \n",  audioSamples);
     audioptr->control = 0x8;
     audioptr->control = 0x0;
    for (int i = 0; i <audioSamples; i++){
        //audioptr->left  = inputAudio[i];
        //audioptr->right = inputAudio[i];
        while((audioptr->wsrc == 0 || audioptr->wslc == 0)); //wait when there is no space for ouput
        audioptr->left  = audioLeft[i];
        audioptr->right = audioRight[i];
    }
}

void configureTimer(volatile int* timingPTR, int timeVal){
    /*
    1. Stop timer
    2. Reset TO bit 
    3. Load low bits 
    4. Load high bits
    */
   *(timingPTR+1) = 0b1000; 
   *timingPTR = 0b00; 
   *(timingPTR + 2) = timeVal & 0xffff;
   *(timingPTR + 3) = (timeVal & 0xffff0000) >> 16;
}


//fft functions
void fft(float data_re[], float data_im[], const unsigned int N) {
  rearrange(data_re, data_im, N);
  compute(data_re, data_im, N);
}

void rearrange(float data_re[], float data_im[], const unsigned int N) {
  unsigned int target = 0;
  for (unsigned int position = 0; position < N; position++) {
    if (target > position) {
      const float temp_re = data_re[target];
      const float temp_im = data_im[target];
      data_re[target] = data_re[position];
      data_im[target] = data_im[position];
      data_re[position] = temp_re;
      data_im[position] = temp_im;
    }
    unsigned int mask = N;
    while (target & (mask >>= 1)) target &= ~mask;
    target |= mask;
  }
}

void compute(float data_re[], float data_im[], const unsigned int N) {
  const float pi = -3.14159265358979323846;

  for (unsigned int step = 1; step < N; step <<= 1) {
    const unsigned int jump = step << 1;
    const float step_d = (float)step;
    float twiddle_re = 1.0;
    float twiddle_im = 0.0;
    for (unsigned int group = 0; group < step; group++) {
      for (unsigned int pair = group; pair < N; pair += jump) {
        const unsigned int match = pair + step;
        const float product_re =
            twiddle_re * data_re[match] - twiddle_im * data_im[match];
        const float product_im =
            twiddle_im * data_re[match] + twiddle_re * data_im[match];
        data_re[match] = data_re[pair] - product_re;
        data_im[match] = data_im[pair] - product_im;
        data_re[pair] += product_re;
        data_im[pair] += product_im;
      }

      // we need the factors below for the next iteration
      // if we don't iterate then don't compute
      if (group + 1 == step) {
        continue;
      }

      float angle = pi * ((float)group + 1) / step_d;
      twiddle_re = cos(angle);
      twiddle_im = sin(angle);
    }
  }
}

void displayHexDigit(int digit, int display) {
    volatile int *hexPtr = (int *)(HEX_BASE + (display * 4)); 
    if (digit >= 0 && digit <= 15) {
        *hexPtr = hex_map[digit];
    }
}