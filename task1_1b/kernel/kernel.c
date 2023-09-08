#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "../image/img1.c"
#include "../image/img2.c"
#include "../image/img3.c"

#define PAN_STEP 10
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

#define LCD_WIDTH 1024
#define LCD_HEIGHT 768

void main()
{


// Define your step size for each arrow key press
// set up serial console
uart_init();
// say hello
uart_puts("\n\nHello World!\n");
// Initialize frame buffer
framebf_init();
// Draw something on the screen
displayImage(img1, 1024, 768);
// echo everything back
int currentImg = 1;
while(1) {
//read each char
char c = uart_getc();

//send back
uart_puts(c);
    if (c == LEFT) {
        currentImg--;
        if(currentImg == 0)
            currentImg = 1;
        displayImage(img2, 1024, 768);
    } else if (c == RIGHT) {
        currentImg++;
        if(currentImg == 4)
            currentImg = 3;
        displayImage(img3, 1024, 768);
    } 
    if (currentImg == 1)
        displayImage(img1, 1024, 768);
    else if (currentImg == 2)
        displayImage(img2, 1024, 768);
    else if (currentImg == 3)
        displayImage(img3, 1024, 768);
}
}
