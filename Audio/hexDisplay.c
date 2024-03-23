#define HEX_BASE 0xFF200020 // Example base address, adjust according to actual
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


int main(void) {
    displayHexDigit(5, 0); 
    return 0;
}
