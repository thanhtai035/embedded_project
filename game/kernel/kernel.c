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

void intToString(int value, char *str) {
    // Handle the case of a negative number
    if (value < 0) {
        *str++ = '-';
        value = -value;
    }

    // Calculate the length of the string
    int length = 0;
    int temp = value;

    do {
        temp /= 10;
        length++;
    } while (temp != 0);

    // Null-terminate the string
    str[length] = '\0';

    // Fill the string in reverse order
    for (int i = length - 1; i >= 0; i--) {
        str[i] = '0' + (value % 10);
        value /= 10;
    }
}



// void showTime() {
//     printString("Time", 800, 50, 0, 0x00FF0000, 4);

//     int countTime = 60;

//     while(countTime != 0) {
//         char str[12];
//         intToString(countTime, str);
//         printString("  ", 800, 100, 0, 0x00FF0000, 4);
//         printString(&str[0], 800, 100, 0, 0x00FF0000, 4);

//         wait_msec(1000);
//         countTime--;
//     }
// }


int showTime(int timeInSeconds) {

    printString("Time", 800, 50, 0, 0x00FF0000, 4);

    // Calculate the position to display the time
    int x = 800;
    int y = 100;
    int fontSize = 4;

    // Convert the time to a string
    char timeStr[3]; // Assuming you only need two digits for seconds
    intToString(timeInSeconds, timeStr);

    // Clear the previous time display by drawing spaces
    printString("  ", x, y, 0, 0x00FF0000, fontSize);

    // Display the new time
    printString(timeStr, x, y, 0, 0x00FF0000, fontSize);

    return 0;
}


void startGame() {
    int xOffset = SCREEN_WIDTH / 2;
    int yOffset = SCREEN_HEIGHT /2;

    printString("Start", xOffset, yOffset, 0, 0x00FF0000, 4);
    while(1) {
        char c = uart_getc();

        if (c == '\n') {
            printString("     ", xOffset, yOffset, 0, 0x00FF0000, 4);

            break;
        }
    }
}

void clearScreen(unsigned int backgroundColor)
{
    // Define the screen dimensions
    int screenWidth = 1024; // Replace with your actual screen width
    int screenHeight = 768; // Replace with your actual screen height

    // Draw a filled rectangle covering the entire screen
    drawRectARGB32(0, 0, screenWidth - 1, screenHeight - 1, backgroundColor, 1);
}

void showPause() {
    clearScreen(0);

    printString("Pause", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0, 0x00FF0000, 4);

    while(1) {
        char c = uart_getc();
        if (c == '\n') {
            // showTime();
            break;
        }
    }
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

    startGame();


    int countTime = 60;

    renderCharacter(xOffset, yOffset);

    
    // echo everything back

    //showTime();

    while(1) {
        //read each char
        char c = getUart();

        showTime(countTime);
        
        set_wait_timer(1, 1000000);

        countTime--;

        set_wait_timer(0, 10);
        
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
        } else if (c == '\n') {
            showPause();
            renderCharacter(xOffset, yOffset);
        } else {

        }

        // Decrement the countdown timer
        
        
        // Disable the timer (wait for it to expire)
        
        

    }
}
