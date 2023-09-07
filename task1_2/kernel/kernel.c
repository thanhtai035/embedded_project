#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"


void main()
{
    // set up serial console
    uart_init();
    // say hello
    // uart_puts("\n\nHello World!\n");
    // Initialize frame buffer
    framebf_init();

    printString("Jaeheon Jeong", 50, 50, 0, 0x00AA0000, 4);
    printString("Sanghwa Jung", 50, 100, 0, 0x0000BB00, 4);
    printString("Le Thanh Tai", 50, 150, 0x000000CC, 1, 4);
    printString("Vo Hoang Tuan", 50, 200, 0x000000CC, 0x00FFFF00, 4);

    // echo everything back
    while(1) {
        //read each char
        char c = uart_getc();
        //send back
        uart_sendc(c);
    }
}