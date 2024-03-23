#define TIMER_BASE 0xFF202000
#define LED_BASE 0xFF200000
volatile int* timerPTR = (int*) TIMER_BASE; 
volatile int* ledPTR = (int*) LED_BASE; 

int timerValue = 900000000; 

int main(void){
     
    configureTimer();
    *(timerPTR+1) = 0b0100; 
    *ledPTR = 0; 
    while(((*timerPTR) & 1) != 1); 
    *ledPTR = 0x3ff; 
    
}

void configureTimer(){
    /*
    1. Stop timer
    2. Reset TO bit 
    3. Load low bits 
    4. Load high bits
    */
   int timerValue = 500000000; 
   *(timerPTR+1) = 0b1000; 
   *timerPTR = 0b00; 
   *(timerPTR + 2) = timerValue & 0xffff;
   *(timerPTR + 3) = (timerValue & 0xffff0000) >> 16;
}