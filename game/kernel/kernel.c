#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "../image/img1.c"
#include "../image/img2.c"
#include "../image/bom.c"
#define DROP_RATE 50 // Adjust the drop rate as needed

#define PAN_STEP 50
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768


int xOffset = SCREEN_WIDTH / 2;
int yOffset = 460;
int lastX = SCREEN_WIDTH / 2;
int jump = 0;
int count = 0;
int isLose = 0; // flag for dodge object which means lose
int stage = 2;
int gameLevel = 1;
struct PixelData {
    unsigned long value;
    int status;
};

struct PixelData screen[SCREEN_WIDTH][SCREEN_HEIGHT];


// This function used to draw character
void updateCharacter() {
    int image_width = 150;
    int image_height = 150;

    // Loop through previous x and y to delete the character
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
        // uart_dec(yOffset);
        // uart_puts("\n");
        // if (jump >= 3) {
        //     for (int y = yOffset; y < yOffset + image_height + PAN_STEP; y++)
        //         {
        //         for (int x = xOffset; x < xOffset + image_width; x++)
        //         {
        //             if (screen[x][y].status == 1)
        //             {
        //                 unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
        //                 screen[x][y].value = attr; // Clear the pixel
        //                 drawPixelARGB32(x, y, attr);
        //                 screen[x][y].status = 0;
        //             }
        //         }
        //     }
        // } else {
        //     for (int y = yOffset; y < yOffset + image_height - PAN_STEP; y++)
        //         {
        //         for (int x = xOffset; x < xOffset + image_width; x++)
        //         {
        //             if (screen[x][y].status == 1)
        //             {
        //                 unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
        //                 screen[x][y].value = attr; // Clear the pixel
        //                 drawPixelARGB32(x, y, attr);
        //                 screen[x][y].status = 0;
        //             }
        //         }
        //     }
        // }
    }

    // Draw character in new x and y
    for (int y = 0; y < image_height; y++) {
        for (int x = 0; x < image_width; x++) {
            int screen_x = x + xOffset; 
            int screen_y = y + yOffset;
            // Check if current position + offset is inside the screen bounds
            if (screen_x >= 0 && screen_x < SCREEN_WIDTH && screen_y >= 0 && screen_y < SCREEN_HEIGHT) {
                unsigned int attr = character[y * image_width + x]; 
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


// Draw the background intially
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

// Draw the bom
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

int showTime(int timeInSeconds) {

    printString("Time: ", 10, 20, 0xFFFFFFFF, 0x00FF0000, 3);

    // Calculate the position to display the time
    int x = 150;
    int y = 20;
    int fontSize = 3;

    // Convert the time to a string
    char timeStr[3]; // Assuming you only need two digits for seconds
    intToString(timeInSeconds, timeStr);

    // Clear the previous time display by drawing spaces
    printString("  ", x, y, 0x0, 0x00FF0000, fontSize);

    // Display the new time
    printString(timeStr, x, y, 0x0, 0x00FF0000, fontSize);

    return 0;
}

void startGame() {
    int xOffset = SCREEN_WIDTH / 2 - 100;
    int yOffset = SCREEN_HEIGHT /2 - 40;

    printString("Start", xOffset, yOffset, 0, 0x00FF0000, 4);
    //printString("__________", xOffset, yOffset + 30, 0, 0x00FF0000, 2);


    while(1) {
        char c = uart_getc();

        if (c == '\n') {
            printString("     ", xOffset, yOffset, 0, 0x00FF0000, 4);
            //printString("      ", xOffset, yOffset+30, 0, 0x00FF0000, 4);

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

    printString("Paused", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40, 0, 0x00FF0000, 4);

    while(1) {
        char c = uart_getc();
        if (c == '\n') {
            // showTime();
            break;
        }
    }
}

void winGame() {
    clearScreen(0);

    printString("Win!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40, 0, 0x00FF0000, 4);
    printString("Press enter for next level", SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 + 10, 0, 0x00FF0000, 2);

    //press enter to move next
    while(1) {
        char c = uart_getc();
        if (c == '\n') {
            // showTime();
            break;
        }
    }
}

void displayLevel(int level) {
    printString("Lv. ", 904, 20, 0x0, 0x00FF0000, 3);

    // Convert the time to a string
    char levelStr[1]; // Assuming you only need one digits for seconds
    intToString(level, levelStr);

    // Calculate the position to display
    int x = 980;
    int y = 16;
    int fontSize = 3;

    // Clear the previous level display by drawing spaces
    printString("  ", x, y, 0x0, 0x00FF0000, fontSize);

    // Display the new level
    printString(levelStr, x, y, 0x0, 0x00FF0000, fontSize);
}

void main()
{

    int gameLevel = 1;


    // Define your step size for each arrow key press
    // set up serial console
    uart_init();
    // say hello
    // uart_puts("\n\nHello World!\n");
    // Initialize frame buffer
    framebf_init();
    // Draw something on the screen
    // displayImage(epd_bitmap_download__3__removebg_preview__1_, 217, 232, xOffset, yOffset);

    // startGame();

    int bomX = SCREEN_WIDTH / 2 - 32; // Initial X position of the bom
    int bomY = 0;                     // Initial Y position of the bom

    // set the timer
    int countTime = 10;
    // int countTimefor2 = 30;
    // int countTimefor3 = 60;

    unsigned char c;
    updateBom(&bomX, &bomY);
    updateCharacter();
    updateBackground();
    displayLevel(1);

    // echo everything back
    // showTime();
    while(1) {

        //read each char
        set_wait_timer(1, 10);

        c = getUart();

        if (c == UP) {
            if (jump ==0) {
                jump = 4;
            }
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
            countTime--;
            if (jump > 0) {
                if (jump >= 3) {
                    yOffset -= PAN_STEP;
                } else {
                    yOffset += PAN_STEP;
                }
                jump--;
                updateCharacter();
            }

            updateBom(&bomX, &bomY);

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
