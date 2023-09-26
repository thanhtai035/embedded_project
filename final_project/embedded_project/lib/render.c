#include "framebf.h"
#include "util.h"
#include "font.h"
#include "render.h" 
#include "../uart/uart1.h"

// Function to clear the screen
void clearScreen() {
    // Loop through every pixel of the screen
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            // Replace the pixel with black 0x0 color
            drawPixelARGB32(x, y, 0);
        }
    }
}

// Method to retrieve the abs value of the number
int custom_abs(int num)
{
    if (num < 0)
    {
        return -num;
    }
    return num;
}


// Method to get the custom x value to drop the rocket
int custom_rand()
{
    int min_distance = 120;
    int range = SCREEN_WIDTH; // The desired range of random numbers 

    while (1)
    {
        // Generate a random x value 
        next = next * 1103515245 + 12345;
        int rand_num = (unsigned int)(next / 65536) % range;

        // Check if the generated random number is at least 64 units away from the previous number
        if (custom_abs(rand_num - previous) >= min_distance)
        {
            previous = rand_num;
            return rand_num;
        }
    }
}

// This function used to draw character
void updateCharacter()
{
    int image_width = CHARACTER_IMAGE_WIDTH;
    int image_height = CHARACTER_IMAGE_HEIGHT;

    // Loop through previous x and y to delete the character
    if (jump == 0) // Case not jump, moving x position only
    {
        // Loop through every pixel of the previous character image position
        for (int y = yOffset; y < yOffset + image_height; y++)
        {
            for (int x = xOffset; x < xOffset + image_width; x++)
            {
                if (screen[x][y].status == CHARACTER_TAG) // The status == 1 means the pixel belong to the char image
                {
                    unsigned int attr = background[y * SCREEN_WIDTH + x]; // Replace the pixel with the background image
                    screen[x][y].value = attr; 
                    drawPixelARGB32(x, y, attr); 
                    screen[x][y].status = 0; // Clear the character status
                }
            }
        }
    }
    else
    {
        // Jump animation
        if (jump >= 4) // Case the character is jumping up
        {
            // Loop through previous x and y to delete the character
            for (int y = yOffset; y < yOffset + image_height + JUMP_STEP; y++)
            {
                for (int x = xOffset; x < xOffset + image_width; x++) // The status == 1 means the pixel belong to the char image
                {
                    unsigned int attr = background[y * SCREEN_WIDTH + x];  // Replace the pixel with the background image
                    screen[x][y].value = attr;                          
                    drawPixelARGB32(x, y, attr);
                    screen[x][y].status = 0; // Clear the character status
                }
            }
        }
        else // Case the character is failing down
        {
            for (int y = yOffset - JUMP_STEP; y < yOffset + image_height + JUMP_STEP; y++)
            {
                for (int x = xOffset; x < xOffset + image_width; x++)
                {
                    if (screen[x][y].status == CHARACTER_TAG) // The status == 1 means the pixel belong to the char image
                    {
                        unsigned int attr = background[y * SCREEN_WIDTH + x]; // Replace the pixel with the background image
                        screen[x][y].value = attr;                           
                        drawPixelARGB32(x, y, attr);
                        screen[x][y].status = 0; // Clear the character status
                    }
                }
            }
        }
    }

    // Draw character in new x and y
    // Loop through the new character in the new position
    for (int y = 0; y < image_height; y++)
    {
        for (int x = 0; x < image_width; x++)
        {
            // Calculate new x, y position
            int screen_x = x + xOffset; 
            int screen_y = y + yOffset;
            // Check if current position + offset is inside the screen bounds
            if (screen_x >= 0 && screen_x < SCREEN_WIDTH && screen_y >= 0 && screen_y < SCREEN_HEIGHT)
            {
                unsigned int attr = character[y * image_width + x]; // Get the corresponding image in the char image
                if (attr != 0x0) // Not the background with is 0x0 color but the real image of char
                {
                    if (screen[screen_x][screen_y].status == BOMB_TAG) // Check if new position of char met the rocket
                    {
                        isLose = 1; // Set the lose tag
                    }
                    screen[screen_x][screen_y].value = attr; 
                    screen[screen_x][screen_y].status = CHARACTER_TAG; // Set the status of the pixel in the screen
                    drawPixelARGB32(screen_x, screen_y, attr);
                }
            }
        }
    }
    // Store the new x off set
    lastX = xOffset;
}

// Draw the background intially
void updateBackground()
{
    // Loop through the screen to draw the background
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            if (screen[x][y].value == 0x0 || screen[x][y].status == 0) // Only draw in pixel without any text or images
            {
                unsigned int attr = background[y * SCREEN_WIDTH + x];
                drawPixelARGB32(x, y, attr);
            }
        }
    }
}

// Method to initialize screen value 
void initScreen()
{
    for (int i = 0; i < 5; i++)
    {
        bombs[i].x = custom_rand(); // Random X position within screen width
        bombs[i].y = 0;             // Start at the top
    }

    hBomb.x = 0;
    hBomb.y = HORIZONTAL_BOMB_Y_OFFSET;

    updateCharacter();
    updateBackground();
}

// Resume screen after the paused. Update the background again
void resumeScreen()
{
    updateBackground();
}

// Draw the bom
void updateBom(int *bomX, int *bomY)
{
    // Get width and height of bomb image
    int image_width = BOMB_IMAGE_WIDTH;
    int image_height = BOMB_IMAGE_HEIGHT;

    // Clear the previous position of the bom
    // Loop through the previous bomb image 
    for (int y = *bomY; y < *bomY + image_height; y++)
    {
        for (int x = *bomX; x < *bomX + image_width; x++)
        {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) // The bomb is still in the screen
            {
                if (screen[x][y].status != CHARACTER_TAG) // We dont want to collapse with the char image
                {
                    unsigned int attr = background[y * SCREEN_WIDTH + x]; // Replace the pixel with the background image
                    screen[x][y].value = attr;                           
                    drawPixelARGB32(x, y, attr);
                    screen[x][y].status = 0; // Clear status
                }
            }
        }
    }

    // Update the position of the bom one step down
    *bomY += DROP_RATE;

    // Draw the bom at the new position
    // Loop through the new bomb image 
    for (int y = *bomY; y < *bomY + image_height; y++)
    {
        for (int x = *bomX; x < *bomX + image_width; x++)
        {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
            {
                unsigned int attr = bom_img[(y - *bomY) * image_width + (x - *bomX)]; // Get the pixel of the corresponding x and y in the image
                if (attr != 0)
                {
                    if (screen[x][y].status != CHARACTER_TAG) // Not hit the target
                    {
                        screen[x][y].value = attr;
                        screen[x][y].status = BOMB_TAG;
                        drawPixelARGB32(x, y, screen[x][y].value);
                    }
                    else
                    { 
                        isLose = 1; // Set lose tag if hit
                    }
                }
            }
        }
    }

    // Incase the rocket go passed the text, draw the background again and put the text after that
    for (int y = TIME_TEXT_Y_OFFSET; y < TIME_TEXT_Y_OFFSET + TEXT_SIZE * PIXEL_WIDTH_PER_CHARACTER; y++)
    {
        for (int x = TIME_TEXT_X_OFFSET; x < TIME_TEXT_X_OFFSET + TEXT_SIZE * PIXEL_WIDTH_PER_CHARACTER * 3; x++) // the value for the time only has 3 chars at maximum
        {
            drawPixelARGB32(x, y, background[y * SCREEN_WIDTH + x]);
        }
    }
    for (int y = LEVEL_VALUE_Y_OFFSET; y < LEVEL_VALUE_Y_OFFSET + TEXT_SIZE * PIXEL_WIDTH_PER_CHARACTER; y++)
    {
        for (int x = LEVEL_VALUE_X_OFFSET; x < LEVEL_VALUE_X_OFFSET + TEXT_SIZE * PIXEL_WIDTH_PER_CHARACTER * 2; x++) // the value for the leve only has 2 chars at maximum
        {
            drawPixelARGB32(x, y, background[y * SCREEN_WIDTH + x]);
        }
    }
    // Display text in case the rocket go passed the text
    displayLevel(gameLevel);
    showTime(timeCount);
}

// Draw the bom horizontally
void updateBomHorizontal(int *bomX, int *bomY)
{
    int image_width = BOMB_IMAGE_WIDTH;
    int image_height = BOMB_IMAGE_HEIGHT;

    // Clear the previous position of the bom
    // Loop through the previous bomb image 
    for (int y = *bomY; y < *bomY + image_height; y++)
    {
        for (int x = *bomX; x < *bomX + image_width; x++)
        {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
            {
                if (screen[x][y].status != CHARACTER_TAG)
                {
                    unsigned int attr = background[y * SCREEN_WIDTH + x]; // Replace the pixel with the background image
                    screen[x][y].value = attr;                            // Clear the pixel
                    drawPixelARGB32(x, y, attr);
                    screen[x][y].status = 0;
                }
            }
        }
    }

    // Update the position of the bom one step to the right
    *bomX += TRAVEL_RATE;

    // Draw the bom at the new position
    // Loop through the new bomb image 

    for (int y = *bomY; y < *bomY + image_height; y++)
    {
        for (int x = *bomX; x < *bomX + image_width; x++)
        {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
            {
                unsigned int attr = bom_horizon_img[(y - *bomY) * image_width + (x - *bomX)];
                if (attr != 0)
                {
                    if (screen[x][y].status != CHARACTER_TAG) // not hit the character
                    {
                        screen[x][y].value = attr;
                        screen[x][y].status = BOMB_TAG;
                        drawPixelARGB32(x, y, screen[x][y].value);
                    }
                    else // set lose tag 
                    {
                        isLose = 1;
                    }
                }
            }
        }
    }
}