#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "../image/img1.c"
#define PAN_STEP 10
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

#define LCD_WIDTH 1024
#define LCD_HEIGHT 768

void main()
{
int xOffset = 0;
int yOffset = 0;

// Define your step size for each arrow key press
// set up serial console
uart_init();
// say hello
uart_puts("\n\nHello World!\n");
// Initialize frame buffer
framebf_init();
// Draw something on the screen
displayImage(epd_bitmap_ad1, 2362, 1329, xOffset, yOffset);
// echo everything back
while(1) {
//read each char
char c = uart_getc();
//send back
uart_puts(c);
    if (c == UP) {
        yOffset -= PAN_STEP;
        if (yOffset < 0) 
            yOffset = 0;  // prevent going beyond top edge
        displayImage(epd_bitmap_ad1, 2362, 1329, xOffset, yOffset);
    } else if (c == DOWN) {
        yOffset += PAN_STEP;
        if (yOffset > (1329 - LCD_HEIGHT)) yOffset = 1329 - LCD_HEIGHT;  // prevent going beyond bottom edge
        displayImage(epd_bitmap_ad1, 2362, 1329, xOffset, yOffset);

    } else if (c == LEFT) {
        xOffset -= PAN_STEP;
        if (xOffset < 0) xOffset = 0;  // prevent going beyond left edge
        displayImage(epd_bitmap_ad1, 2362, 1329, xOffset, yOffset);

    } else if (c == RIGHT) {
        xOffset += PAN_STEP;
        if (xOffset > (2362 - LCD_WIDTH)) xOffset = 2362 - LCD_WIDTH;  // prevent going beyond right edge
        displayImage(epd_bitmap_ad1, 2362, 1329, xOffset, yOffset);

    } else {

    }

}
}
