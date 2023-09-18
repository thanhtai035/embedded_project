#include "../uart/uart1.h"
#include "mbox.h"
#include "../lib/framebf.h"
#include "../lib/font.h"
#include "../lib/render.h"
#include "../lib/util.h"

#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

void set_wait_timer(int set, unsigned int msVal);

void main()
{

    uart_init();
    framebf_init();

    // set the timer
    int countTime = 10;
    int bomIndex = 0;
    // int countTimefor2 = 30;
    // int countTimefor3 = 60;

    unsigned char c;

    while (1)
    {
        if (stage == 1)
        {
            startGame();
            initScreen();
            stage = 2;
        }
        else if (stage == 2)
        {
            // read each char
            set_wait_timer(1, 10);
            c = getUart();

            if (c == UP)
            {
                if (jump == 0)
                {
                    jump = 6;
                }
            }
            else if (c == LEFT)
            {
                if (xOffset - PAN_STEP >= 0)
                    xOffset -= PAN_STEP;
                updateCharacter();
            }
            else if (c == RIGHT)
            {
                if (xOffset + PAN_STEP <= SCREEN_WIDTH)
                    xOffset += PAN_STEP;
                updateCharacter();
            }
            else if (c == '\n')
            {
                stage = 3;
                continue;
            }
            set_wait_timer(0, 10);
            count++;

            if (count % 10 == 0)
            {
                if (jump > 0)
                {
                    if (jump >= 4)
                    {
                        yOffset -= JUMP_STEP;
                    }
                    else
                    {
                        yOffset += JUMP_STEP;
                    }
                    updateCharacter();
                    jump--;
                }
                // for (int i = 0; i < 5; i++)
                // {
                //     updateBom(&bombs[i].x, &bombs[i].y);
                // }

                // // Check if the bom has reached the bottom of the screen
                // if (bombs[0].y >= SCREEN_HEIGHT)
                // {

                //     for (int i = 0; i < 5; i++)
                //     {

                //         bombs[i].x = custom_rand(); // Random X position within screen width
                //         bombs[i].y = 0;             // Start at the top
                //     }
                // }

                updateBom(&bombs[bomIndex].x, &bombs[bomIndex].y);

                if (gameLevel == 2)
                {
                    updateBom(&bombs[0].x, &bombs[0].y);

                    if (count % 20)
                    {
                        updateBom(&bombs[1].x, &bombs[1].y);
                    }
                }

                if (gameLevel == 3)
                {
                    updateBom(&bombs[0].x, &bombs[0].y);
                    if (count % 20)
                    {
                        updateBom(&bombs[1].x, &bombs[1].y);
                    }

                    if (count % 30)
                    {
                        updateBom(&bombs[2].x, &bombs[2].y);
                    }
                }

                if (gameLevel == 4)
                {
                    updateBom(&bombs[0].x, &bombs[0].y);
                    if (count % 20)
                    {
                        updateBom(&bombs[1].x, &bombs[1].y);
                    }

                    if (count % 30)
                    {
                        updateBom(&bombs[2].x, &bombs[2].y);
                    }

                    if (count % 40)
                    {
                        updateBom(&bombs[3].x, &bombs[3].y);
                    }
                }

                if (gameLevel == 5)
                {
                    updateBom(&bombs[0].x, &bombs[0].y);
                    if (count % 20)
                    {
                        updateBom(&bombs[1].x, &bombs[1].y);
                    }

                    if (count % 30)
                    {
                        updateBom(&bombs[2].x, &bombs[2].y);
                    }

                    if (count % 40)
                    {
                        updateBom(&bombs[3].x, &bombs[3].y);
                    }

                    if (count % 50)
                    {
                        updateBom(&bombs[4].x, &bombs[4].y);
                    }
                }

                if (gameLevel == 6)
                {
                    updateBomHorizontal(&hBomb.x, &hBomb.y);
                    updateBom(&bombs[bomIndex].x, &bombs[bomIndex].y);
                }

                if (gameLevel == 7)
                {
                    updateBomHorizontal(&hBomb.x, &hBomb.y);
                    updateBom(&bombs[bomIndex].x, &bombs[bomIndex].y);

                    if (count % 20)
                    {
                        updateBom(&bombs[1].x, &bombs[1].y);
                    }
                }

                if (gameLevel == 8)
                {
                    updateBomHorizontal(&hBomb.x, &hBomb.y);

                    updateBom(&bombs[bomIndex].x, &bombs[bomIndex].y);

                    if (count % 20)
                    {
                        updateBom(&bombs[1].x, &bombs[1].y);
                    }
                    if (count % 30)
                    {
                        updateBom(&bombs[2].x, &bombs[2].y);
                    }
                }

                if (gameLevel == 9)
                {
                    updateBomHorizontal(&hBomb.x, &hBomb.y);
                    updateBom(&bombs[bomIndex].x, &bombs[bomIndex].y);

                    if (count % 20)
                    {
                        updateBom(&bombs[1].x, &bombs[1].y);
                    }

                    if (count % 30)
                    {
                        updateBom(&bombs[2].x, &bombs[2].y);
                    }

                    if (count % 40)
                    {
                        updateBom(&bombs[3].x, &bombs[3].y);
                    }
                }

                if (gameLevel == 10)
                {
                    updateBomHorizontal(&bombs[5].x, &bombs[5].y);
                    updateBom(&bombs[bomIndex].x, &bombs[bomIndex].y);

                    if (count % 20)
                    {
                        updateBom(&bombs[1].x, &bombs[1].y);
                    }

                    if (count % 30)
                    {
                        updateBom(&bombs[2].x, &bombs[2].y);
                    }

                    if (count % 40)
                    {
                        updateBom(&bombs[3].x, &bombs[3].y);
                    }

                    if (count % 50)
                    {
                        updateBom(&bombs[4].x, &bombs[4].y);
                    }
                }

                // Check if the bom has reached the bottom of the screen
                for (int i = 0; i < 5; i++)
                {
                    if (bombs[i].y >= SCREEN_HEIGHT)
                    {

                        bombs[i].x = custom_rand(); // Random X position within screen width
                        bombs[i].y = 0;             // Start at the top
                    }
                }

                if (hBomb.x >= SCREEN_WIDTH)
                {

                    hBomb.x = 0;
                    hBomb.y = 530;
                }

                if (count == 100)
                {
                    timeCount--;
                    if (timeCount == 0)
                    {
                        timeCount = 15;
                        gameLevel++;

                        updateBackground();
                        if (gameLevel == 10)
                        {
                            stage = 5;
                            continue;
                        }
                    }
                    count = 0;
                }
            }
            if (isLose == 1)
            {
                stage = 4;
                continue;
            }
        }
        else if (stage == 3)
        {
            showPause();
            char c = uart_getc();
            if (c == '\n')
            {
                displayLevel(gameLevel);
                showTime(timeCount);

                updateBackground();
                stage = 2;
                continue;
            }
        }
        else if (stage == 4)
        {
            printString("You lost!", SCREEN_WIDTH / 2 - 200, 200, 0, 0x00FF0000, 6);
            char c = uart_getc();
            if (c == '\n')
            {
                resetVariable();
            }
        }
        else
        {
            winGame();
            char c = uart_getc();
            if (c == '\n')
            {
                resetVariable();
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
