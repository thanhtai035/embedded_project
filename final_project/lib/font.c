#include "framebf.h"
#include "../uart/uart1.h"
#include "render.h"
#include "util.h"
#include "font.h"

// Method to convert integer to string value
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

// Function to display time in the game
int showTime(int timeInSeconds) {

    printString("Time: ", 10, 20, 0xFFFFFFFF, TEXT_COLOR, 3);

    // Calculate the position to display the time
    int x = TIME_TEXT_X_OFFSET;
    int y = TIME_TEXT_Y_OFFSET;
    int fontSize = TEXT_SIZE;

    // Convert the time to a string
    char timeStr[3]; // We only needs maximum 2 digits
    intToString(timeInSeconds, timeStr);

    // Display the new time
    printString(timeStr, x, y, 0x0, TEXT_COLOR, fontSize);

    return 0;
}

// Method to print the sreen when start the game start the game
void startGame() {
    clearScreen();
    int xOffset = SCREEN_WIDTH / 2 - 100;
    int yOffset = SCREEN_HEIGHT /2 - 40;

    printString("Start", xOffset, yOffset, 0, TEXT_COLOR, 4);
}

// Method to display paused message
void showPause() {
    // The x offset is calculated getting the middle value width of the screen minus the width of the string
    printString("Paused", SCREEN_WIDTH / 2 - TEXT_SIZE*PIXEL_WIDTH_PER_CHARACTER*6, PAUSED_TEXT_Y_OFFSET, 0, TEXT_COLOR, 5);
}

// Method to display win game message
void winGame() {
    clearScreen();

    // String display
    // The x offset and y offset is measured so that the text is displayed in the middle of the screen
    printString("Win!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40, 0, TEXT_COLOR, HEADING_SIZE);
    printString("Press enter to restart", SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 + 10, 0, TEXT_COLOR, TEXT_SIZE);
}

// Method to display lose game message
void loseGame() {
    printString("You lost!", SCREEN_WIDTH / 2 - 200, LOSE_TEXT_Y_OFFSET, 0, TEXT_COLOR, HEADING_SIZE);
}

// Method to display the current level of the game
void displayLevel(int level) {
    printString("Lv. ", LEVEL_TEXT_X_OFFSET, LEVEL_TEXT_Y_OFFSET, 0x0, TEXT_COLOR, TEXT_SIZE);

    // Convert the time to a string
    char levelStr[2]; 
    intToString(level, levelStr);

    // Calculate the position to display
    int x = LEVEL_VALUE_X_OFFSET;
    int y = LEVEL_VALUE_Y_OFFSET;
    int fontSize = TEXT_SIZE;

    // Display the new level
    printString(levelStr, x, y, 0x0, TEXT_COLOR, fontSize);
}
