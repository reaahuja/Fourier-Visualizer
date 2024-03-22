/*
Task: 
1. Takes input audio from microphone for 1 second once KEY 1 is pressed
2. Store the input audio into an array 
3. Pass the input audio array to the fft algorithim 
4. Retrieve the manipulated array and change convert it to dB for the y-axis
5. Make an array for the x-axis that is in Hz instead of k
*/

#define audioLength 5
#define timerValue 50000000;

void microphoneRecording();
void microphoneOutput(); 

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

int leftAudio[audioLength*8000] = {0};
int rightAudio[audioLength*8000] = {0};

void microphoneRecording(){
    //LEDS will go high during recording and timer will count down from 5
    volatile int *ledPtr = (int *)(0xff200000);
    volatile int* timerPtr = (int*)(0xFF202000);
    volatile int* AUDIO = 0xFF203040; 

    audioptr->ralc = 0; 
    audioptr->ralc = 0; 
    audioptr->wslc = 128; 
    audioptr->wsrc = 128; 
     
    *(timerPtr) = 0;
    *(timerPtr + 1) = 0x4;
    int timing = audioLength; 
    for (int i = 0; (i < (audioLength*8000)) && (timing != 0); i++){
        leftAudio[i] =  audioptr->left; 
        rightAudio[i] = audioptr->right; 
        *(ledPtr) = 0x3ff;

        if ((*timerPtr & 0x1) == 0x1){
            *(timerPtr) = 0;
            *(timerPtr + 1) = 0x4;
            timing--; 
        }
    }
    *(ledPtr) = 0x0;
}

void microphoneOutput(){
    for (int i = 0; i < audioLength*8000; i++){
        audioptr->left  = leftAudio[i];
        audioptr->right = rightAudio[i]; 
    }
}

void configTimer(int timerMaxVal) {
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

