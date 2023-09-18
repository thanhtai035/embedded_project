#include "framebf.h"
#include "util.h"
#include "font.h"
#include "../uart/uart1.h"

void initScreen()
{
    for (int i = 0; i < 5; i++)
    {
        bombs[i].x = custom_rand(); // Random X position within screen width
        bombs[i].y = 0;             // Start at the top
    }

    hBomb.x = 0;
    hBomb.y = 530;

    updateCharacter();
    updateBackground();
    // displayLevel(1);
    // showtime(timeCount);
}

void resumeScreen()
{
    updateBackground();
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
                if (screen[x][y].status != 1)
                {
                    unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
                    screen[x][y].value = attr;                            // Clear the pixel
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
                if (attr != 0)
                {
                    if (screen[x][y].status != 1)
                    {
                        screen[x][y].value = attr;
                        screen[x][y].status = 2;
                        drawPixelARGB32(x, y, screen[x][y].value);
                    }
                    else
                    {
                        isLose = 1;
                    }
                }
            }
        }
    }
    for (int y = 20; y < 20 + 3 * 8; y++)
    {
        for (int x = 150; x < 150 + 3 * 8 * 3; x++)
        {
            drawPixelARGB32(x, y, background[y * SCREEN_WIDTH + x]);
        }
    }
    for (int y = 16; y < 16 + 3 * 8; y++)
    {
        for (int x = 980; x < 980 + 3 * 8 * 2; x++)
        {
            drawPixelARGB32(x, y, background[y * SCREEN_WIDTH + x]);
        }
    }
    displayLevel(gameLevel);
    showTime(timeCount);
}

// This function used to draw character
void updateCharacter()
{
    int image_width = 150;
    int image_height = 150;

    // Loop through previous x and y to delete the character
    if (jump == 0)
    {
        for (int y = yOffset; y < yOffset + image_height; y++)
        {
            for (int x = xOffset; x < xOffset + image_width; x++)
            {
                if (screen[x][y].status == 1)
                {
                    unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
                    screen[x][y].value = attr;                            // Clear the pixel
                    drawPixelARGB32(x, y, attr);
                    screen[x][y].status = 0;
                }
            }
        }
    }
    else
    {
        if (jump >= 4)
        {
            for (int y = yOffset; y < yOffset + image_height + JUMP_STEP; y++)
            {
                for (int x = xOffset; x < xOffset + image_width; x++)
                {
                    unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
                    screen[x][y].value = attr;                            // Clear the pixel
                    drawPixelARGB32(x, y, attr);
                    screen[x][y].status = 0;
                }
            }
        }
        else
        {
            for (int y = yOffset - JUMP_STEP; y < yOffset + image_height + JUMP_STEP; y++)
            {
                for (int x = xOffset; x < xOffset + image_width; x++)
                {
                    if (screen[x][y].status == 1)
                    {
                        unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
                        screen[x][y].value = attr;                            // Clear the pixel
                        drawPixelARGB32(x, y, attr);
                        screen[x][y].status = 0;
                    }
                }
            }
        }
    }

    // Draw character in new x and y
    for (int y = 0; y < image_height; y++)
    {
        for (int x = 0; x < image_width; x++)
        {
            int screen_x = x + xOffset;
            int screen_y = y + yOffset;
            // Check if current position + offset is inside the screen bounds
            if (screen_x >= 0 && screen_x < SCREEN_WIDTH && screen_y >= 0 && screen_y < SCREEN_HEIGHT)
            {
                unsigned int attr = character[y * image_width + x];
                if (attr != 0x0)
                {
                    if (screen[screen_x][screen_y].status == 2)
                    {
                        isLose = 1;
                    }
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
void updateBackground()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            if (screen[x][y].value == 0x0 || screen[x][y].status == 0)
            {
                unsigned int attr = background[y * SCREEN_WIDTH + x];
                drawPixelARGB32(x, y, attr);
            }
        }
    }
}

int custom_abs(int num)
{
    if (num < 0)
    {
        return -num;
    }
    return num;
}

int custom_rand()
{
    int min_distance = 120;
    int range = 1024; // The desired range of random numbers (0 to 1023)

    while (1)
    {
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

// Draw the bom horizontally
void updateBomHorizontal(int *bomX, int *bomY)
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
                if (screen[x][y].status != 1)
                {
                    unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
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
    for (int y = *bomY; y < *bomY + image_height; y++)
    {
        for (int x = *bomX; x < *bomX + image_width; x++)
        {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
            {
                unsigned int attr = bom_horizon_img[(y - *bomY) * image_width + (x - *bomX)];
                if (attr != 0)
                {
                    if (screen[x][y].status != 1)
                    {
                        screen[x][y].value = attr;
                        screen[x][y].status = 2;
                        drawPixelARGB32(x, y, screen[x][y].value);
                    }
                    else
                    {
                        isLose = 1;
                    }
                }
            }
        }
    }
    // Clear the remaining areas to the right of the bom
    // for (int y = *bomY; y < *bomY + image_height; y++)
    // {
    //     for (int x = *bomX + image_width; x < *bomX + image_width + TRAVEL_RATE; x++)
    //     {
    //         if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
    //         {
    //             unsigned int attr = background[y * SCREEN_WIDTH + x]; // Use the correct index for character_img
    //             screen[x][y].value = attr;                            // Clear the pixel
    //             drawPixelARGB32(x, y, attr);
    //             screen[x][y].status = 0;
    //         }
    //     }
    // }
}