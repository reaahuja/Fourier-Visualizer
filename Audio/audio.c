/*
Task: 
1. Takes input audio from microphone for 1 second once KEY 1 is pressed
2. Store the input audio into an array 
3. Pass the input audio array to the fft algorithim 
4. Retrieve the manipulated array and change convert it to dB for the y-axis
5. Make an array for the x-axis that is in Hz instead of k
*/


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

int main(void){
    volatile int* AUDIO = 0xFF203040; 

    
    audioptr->ralc = 0; 
    audioptr->ralc = 0; 
    audioptr->wslc = 128; 
    audioptr->wsrc = 128; 
     
    while(1){
        if(((audioptr->ralc) > 0) || ((audioptr->rarc) > 0)){
            int leftAudio = audioptr->left; 
            int rightAudio = audioptr->right; 
            audioptr->left = leftAudio; 
            audioptr->right = rightAudio; 
            /*
            audioptr->wsrc = audioptr->rarc; 
            audioptr->wslc = audioptr->ralc; 
            */
        }
    }
}