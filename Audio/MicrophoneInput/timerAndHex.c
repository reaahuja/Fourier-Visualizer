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

    displayHexDigit(5, 0); 
    return 0;
    
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

#define HEX_BASE 0xFF200020 
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

void displayHexDigit(int digit, int display) {
    volatile int *hexPtr = (int *)(HEX_BASE + (display * 4)); 
    if (digit >= 0 && digit <= 15) {
        *hexPtr = hex_map[digit];
    }
}


