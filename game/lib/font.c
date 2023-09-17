#include "framebf.h"
#include "render.h"
#include "util.h"

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
    // printString("  ", x, y, 0x0, 0x00FF0000, fontSize);

    // Display the new time
    printString(timeStr, x, y, 0x0, 0x00FF0000, fontSize);

    return 0;
}

void startGame() {
    clearScreen();
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
    printString("Paused", SCREEN_WIDTH / 2 - 3*8*5, 50, 0, 0x00FF0000, 5);
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

    // Display the new level
    printString(levelStr, x, y, 0x0, 0x00FF0000, fontSize);
}
