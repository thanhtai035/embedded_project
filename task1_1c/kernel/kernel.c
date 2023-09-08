#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "../image/frames.c"


#define LCD_WIDTH 1024
#define LCD_HEIGHT 768
/* Function to start a timer (set = 1) or wait for it to expire (set = 0) */
void set_wait_timer(int set, unsigned int msVal) {
static unsigned long expiredTime = 0; //declare static to keep value
register unsigned long r, f, t;
if (set) { /* SET TIMER */
// Get the current counter frequency (Hz)
asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
// Read the current counter
asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
// Calculate expired time:
expiredTime = t + ( (f/1000)*msVal )/1000;
}
else { /* WAIT FOR TIMER TO EXPIRE */
do {
asm volatile ("mrs %0, cntpct_el0" : "=r"(r));
} while(r < expiredTime);
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


// Define your step size for each arrow key press
// set up serial console
uart_init();
// say hello
uart_puts("\n\nHello World!\n");
// Initialize frame buffer
framebf_init();
// Draw something on the screen
// echo everything back
int currentFrame = 0;
displayImage(epd_bitmap_allArray[currentFrame], 320, 180);

while(1) {
wait_msec(100);
currentFrame++;
if(currentFrame == 51)
    currentFrame = 0;
displayImage(epd_bitmap_allArray[currentFrame], 320, 180);


}
}