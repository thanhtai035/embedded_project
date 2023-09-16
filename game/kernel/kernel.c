#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "../image/img1.c"
#include "../image/img2.c"

#include "../image/bom.c"

#define PAN_STEP 50
#define JUMP_STEP 20
#define DROP_RATE 50 // Adjust the drop rate as needed
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define TIMER_RUNNING 0
#define TIMER_EXPIRED 1

int xOffset = SCREEN_WIDTH / 2;
int yOffset = 460;
int lastX = SCREEN_WIDTH / 2;
int jump = 0;

struct PixelData
{
    unsigned long value;
    int status;
};

struct PixelData screen[SCREEN_WIDTH][SCREEN_HEIGHT];

void updateCharacter() {
    int image_width = 150;
    int image_height = 150;

    if (jump == 0) {
        for (int y = yOffset; y < yOffset + image_height; y++)
        {
            for (int x = xOffset; x < xOffset + image_width; x++)
            {
                if (screen[x][y].status == 1)
                {
                    unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
                    screen[x][y].value = attr; // Clear the pixel
                    drawPixelARGB32(x, y, attr);
                    screen[x][y].status = 0;
                }
            }
        }
    }  else {
        uart_dec(yOffset);
        uart_puts("\n");
        if (jump >= 3) {
            for (int y = yOffset; y < yOffset + image_height + PAN_STEP; y++)
                {
                for (int x = xOffset; x < xOffset + image_width; x++)
                {
                    if (screen[x][y].status == 1)
                    {
                        unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
                        screen[x][y].value = attr; // Clear the pixel
                        drawPixelARGB32(x, y, attr);
                        screen[x][y].status = 0;
                    }
                }
            }
        } else {
            // for (int y = yOffset; y < yOffset + image_height - PAN_STEP; y++)
            //     {
            //     for (int x = xOffset; x < xOffset + image_width; x++)
            //     {
            //         if (screen[x][y].status == 1)
            //         {
            //             unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
            //             screen[x][y].value = attr; // Clear the pixel
            //             drawPixelARGB32(x, y, attr);
            //             screen[x][y].status = 0;
            //         }
            //     }
            // }
        }
    }

    for (int y = 0; y < image_height; y++) {
        for (int x = 0; x < image_width; x++) {
            int screen_x = x + xOffset; 
            int screen_y = y + yOffset;
            // Check if current position + offset is inside the screen bounds
            if (screen_x >= 0 && screen_x < SCREEN_WIDTH && screen_y >= 0 && screen_y < SCREEN_HEIGHT) {
                unsigned int attr = character[y * image_width + x]; // Use the correct index for character_img
                if (attr != 0x0) {
                    screen[screen_x][screen_y].value = attr;
                    screen[screen_x][screen_y].status = 1;
                    drawPixelARGB32(screen_x, screen_y, attr);
                }
            }
        }
    }
    lastX = xOffset;
}



void updateBackground() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (screen[x][y].value == 0x0) {
                unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
                drawPixelARGB32(x, y, attr);
            }
        }
    }
}


void updateBom(int *bomX, int *bomY)
{
    int image_width = 64;
    int image_height = 64;

    // Clear the previous position of the bom
    for (int y = *bomY; y < *bomY + image_height; y++)
    {
        for (int x = *bomX; x < *bomX + image_width; x++)
        {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
            {   
                if(screen[x][y].status != 1 ) {
                    unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
                    screen[x][y].value = attr; // Clear the pixel
                    drawPixelARGB32(x, y, attr);
                    screen[x][y].status = 0;
                }
            }
        }
    }

    // Update the position of the bom one step down
    *bomY += DROP_RATE;

    // Draw the bom at the new position
    for (int y = *bomY; y < *bomY + image_height; y++)
    {
        for (int x = *bomX; x < *bomX + image_width; x++)
        {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
            {
                unsigned int attr = bom_img[(y - *bomY) * image_width + (x - *bomX)];
                    if (attr != 0) {
                    screen[x][y].value = attr;
                    screen[x][y].status = 2;
                    drawPixelARGB32(x, y, screen[x][y].value);
                }
            }
        }
    }
}

void renderBom(int *bomX, int *bomY)
{
    updateBom(bomX, bomY);
    // for (int y = 0; y < SCREEN_HEIGHT; y++)
    // {
    //     for (int x = 0; x < SCREEN_HEIGHT; x++)
    //     {
    //         drawPixelARGB32(x, y, screen[x][y].value);
    //     }
    // }
}


/* Function to start a timer (set = 1) or wait for it to expire (set = 0) */
void set_wait_timer(int set, unsigned int msVal)
{
    static unsigned long expiredTime = 0; // declare static to keep value
    register unsigned long r, f, t;
    if (set)
    { /* SET TIMER */
        // Get the current counter frequency (Hz)
        asm volatile("mrs %0, cntfrq_el0"
                     : "=r"(f));
        // Read the current counter
        asm volatile("mrs %0, cntpct_el0"
                     : "=r"(t));
        // Calculate expired time:
        // expiredTime = t + ((f / 1000) * msVal) / 1000;
        expiredTime = t + ((f * msVal) / 1000);
    }
    else
    { /* WAIT FOR TIMER TO EXPIRE */
        do
        {
            asm volatile("mrs %0, cntpct_el0"
                         : "=r"(r));
        } while (r < expiredTime);
    }
}

void main()
{
    int bomX = SCREEN_WIDTH / 2 - 32; // Initial X position of the bom
    int bomY = 0;                     // Initial Y position of the bom
    // Define your step size for each arrow key press
    // set up serial console
    uart_init();
    // say hello
    uart_puts("\n\nHello World!\n");
    framebf_init();
    int count = 0;
    renderBom(&bomX, &bomY);
    unsigned char c;


    updateCharacter();
    updateBackground();

    // Initially, set the timer for, e.g., 2000 milliseconds (2 seconds).
    // echo everything back
    while (1)
    {
        set_wait_timer(1, 10);

        c = getUart();

        if (c == UP) {
            if (jump ==0) {
                jump = 4;
            }
            // updateCharacter();
        } else if (c == LEFT) {
            if (xOffset - PAN_STEP >= 0)
                xOffset -= PAN_STEP;
            updateCharacter();

        } else if (c == RIGHT) {
            if (xOffset + PAN_STEP <= SCREEN_WIDTH)
                xOffset += PAN_STEP;
            updateCharacter();

        } 
        set_wait_timer(0,10); 
        count++; 
        if (count == 10 ) {
            if (jump > 0) {
                if (jump >= 3) {
                    yOffset -= PAN_STEP;
                } else {
                    yOffset += PAN_STEP;
                }
                jump--;
                updateCharacter();
            }

            renderBom(&bomX, &bomY);

            // Check if the bom has reached the bottom of the screen
            if (bomY >= SCREEN_HEIGHT)
            {
                // Reset the bom to the top of the screen
                bomX = lastX; // Adjust the initial X position as needed
                bomY = 0;
            }
            count = 0;
        }

    }
}
      