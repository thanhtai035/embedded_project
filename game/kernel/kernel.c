#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "../image/img1.c"

#define PAN_STEP 10
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

struct PixelData {
    unsigned long value;
    int status;
};

struct PixelData screen[SCREEN_WIDTH][SCREEN_HEIGHT];


void updateCharacter(int xOffset, int yOffset) {
    int image_width = 217;
    int image_height = 232;
    for (int y = 0; y < image_height; y++) {
        for (int x = 0; x < image_width; x++) {
            int screen_x = x + xOffset;
            int screen_y = y + yOffset;
            // Check if current position + offset is inside the screen bounds
            if (screen_x >= 0 && screen_x < SCREEN_WIDTH && screen_y >= 0 && screen_y < SCREEN_HEIGHT) {
                unsigned int attr = character_img[y * image_width + x]; // Use the correct index for character_img
                screen[screen_x][screen_y].value = attr;
                screen[screen_x][screen_y].status = 1;
            }
        }
    }
}


void renderCharacter(int xOffset, int yOffset) {
    updateCharacter(xOffset, yOffset);
     for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_HEIGHT; x++) {
            drawPixelARGB32(x, y, screen[x][y].value);
        }
    }
}


/* Function to wait for some msec: the program will stop there */
void wait_msec(unsigned int n)
{
    volatile uint64_t f, t, r, expiredTime;

    // Get the current counter frequency (Hz)
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));

    // Read the current counter value
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));

    // Calculate expiration time for the counter
    expiredTime = t + ((f * n) / 1000); // Convert milliseconds to microseconds

    do {
        // Read the current counter value
        asm volatile ("mrs %0, cntpct_el0" : "=r"(r));
    } while (r < expiredTime);
}
void main()
{
int xOffset = SCREEN_WIDTH / 2;
int yOffset = SCREEN_HEIGHT /3;



// Define your step size for each arrow key press
// set up serial console
uart_init();
// say hello
uart_puts("\n\nHello World!\n");
// Initialize frame buffer
framebf_init();
// Draw something on the screen
// displayImage(epd_bitmap_download__3__removebg_preview__1_, 217, 232, xOffset, yOffset);
renderCharacter(xOffset, yOffset);
// echo everything back
while(1) {
//read each char
char c = uart_getc();
//send back
    if (c == UP) {
        if (yOffset - PAN_STEP >= 0)
            yOffset -= PAN_STEP;
        renderCharacter(xOffset, yOffset);
    } else if (c == DOWN) {
           if (yOffset + PAN_STEP <= SCREEN_HEIGHT)
            yOffset += PAN_STEP;        
        renderCharacter(xOffset, yOffset);
    } else if (c == LEFT) {
        if (xOffset - PAN_STEP >= 0)
            xOffset -= PAN_STEP;
        renderCharacter(xOffset, yOffset);
    } else if (c == RIGHT) {
        if (xOffset + PAN_STEP <= SCREEN_WIDTH)
            xOffset += PAN_STEP;
        renderCharacter(xOffset, yOffset);
    } else {

    }

}
}
