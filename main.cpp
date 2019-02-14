#include "mbed.h"

DigitalOut led1(PA_15);
DigitalIn user_button(PA_0); 

Serial pc(PB_6, PB_7);

// main() runs in its own thread in the OS
int main() {
    pc.printf("test\n");

    while (true) {
        led1 = !led1;
        pc.printf("Hello World\n"); 
        wait_ms(500); 
    }
}

