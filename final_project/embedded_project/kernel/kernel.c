#include "../uart/uart1.h"
#include "mbox.h"
#include "../lib/framebf.h"
#include "../lib/font.h"
#include "../lib/render.h"
#include "../lib/util.h"
#include "../image/task1_image.c"
#include "../image/frames.c"

#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'
#define TAB '\t'
#define ENTER '\n'

#define MENU 0
#define TASK1_1A 1
#define TASK1_1B 2
#define TASK1_1C 3
#define TASK1_2 4
#define GAME 5

#define TASK1_1A_HEIGHT 1329
#define TASK1_1A_WIDTH 2362
#define VIDEO_FRAME_WIDTH 320
#define VIDEO_FRAME_HEIGHT 180

int task = MENU;
int countTime = 10;

void showMenu();
void task1_1a_operation();
void task1_1b_operation();
void task1_1c_operation();
void task1_2_operation();
void gameOperation();
void updateDifficulty();
void set_wait_timer(int set, unsigned int msVal);

void main()
{

    uart_init();
    framebf_init();


    while (1)
    {
        if (task == MENU) { // Show menu
            showMenu();
        }
        else if (task == TASK1_1A) { // Run the scroll image task
            task1_1a_operation();
            task = MENU;
        } else if (task == TASK1_1B) { // Run the slideshow task
            task1_1b_operation();
            task = MENU;
        } else if (task == TASK1_1C) { // Run the video task
            task1_1c_operation();
            task = MENU;
        } else if (task == TASK1_2) { // Run the font display task
            task1_2_operation();
            task = MENU;
        } 
        else {
            gameOperation(); // Run the game
        }
    }
}

// Method to display the menu to navigate to the task
void showMenu() {
    clearScreen();
    resetVariable(); // Reset variable of the game when get back to main menu


    uart_puts("\n\nEmbedded Systems: Operating Systems & Interfacing - Group 1\n");
    uart_puts("1. Task 1a, part i - Scroll for big image\n");
    uart_puts("2. Task 1a, part ii - Image slideshow\n");
    uart_puts("3. Task 1a, part iii - Video\n");
    uart_puts("4. Task 1b - Display font\n");
    uart_puts("5. Play the game!\n");
    uart_puts("\nPress TAB to go back to main menu\n");
    while(1) {
        char c = uart_getc();
        if (c == '1') {
            task = TASK1_1A;
            break;
        }
        else if (c == '2') {
            task = TASK1_1B;
            break;
        }
        else if (c == '3') {
            task = TASK1_1C;
            break;
        } 
        else if (c == '4') {
            task = TASK1_2;
            break;
        }
        else if (c == '5') {
            task = GAME;
            break;
        } else {
            uart_puts("Invalid input");
        }
    }
}

// Method for task 1 - 1a, scroll image
void task1_1a_operation() {
    // Initialize x, y offset
    int y = 0; 
    int x = 0;
    // Display image at the inital stage
    scrollImage(task1_1a_image, TASK1_1A_WIDTH, TASK1_1A_HEIGHT, x, y);

    while(1) {
    char c = uart_getc();
        if (c == UP) {  // w button click
            y -= PAN_STEP; // Update y offset
            if (y < 0) 
                y = 0;  // prevent going beyond top edge
            scrollImage(task1_1a_image, TASK1_1A_WIDTH, TASK1_1A_HEIGHT, x, y); // Display image with new y off set
        } else if (c == DOWN) { // s button click
            y += PAN_STEP; // Update y offset
            if (y > (TASK1_1A_HEIGHT - SCREEN_HEIGHT)) y = TASK1_1A_HEIGHT - SCREEN_HEIGHT;  // prevent going beyond bottom edge
            scrollImage(task1_1a_image, TASK1_1A_WIDTH, TASK1_1A_HEIGHT, x, y);  // Display image with new y off set

        } else if (c == LEFT) { // a button click
            x -= PAN_STEP;  // Update x offset
            if (x < 0) x = 0;  // prevent going beyond left edge
            scrollImage(task1_1a_image, TASK1_1A_WIDTH, TASK1_1A_HEIGHT, x, y); // Display image with new x off set

        } else if (c == RIGHT) {  // d button click
            x += PAN_STEP; // Update x offset
            if (x > (TASK1_1A_WIDTH - SCREEN_WIDTH)) x = TASK1_1A_WIDTH - SCREEN_WIDTH;  // prevent going beyond right edge
            scrollImage(task1_1a_image, TASK1_1A_WIDTH, TASK1_1A_HEIGHT, x, y); // Display image with new x off set
        } else if (c == TAB){
            break;
        }

    }
}

// Method for task 1 - 1b, slide show
void task1_1b_operation() {
    // The images used in this tag have the width and height match the screen
    displayImage(task1_1b_image1, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Variable to track the current image
    int currentImg = 1;
    
    while(1) {
        char c = uart_getc();

        if (c == LEFT) { // Go to previous image 
            currentImg--;
            if(currentImg == 0) // The minimum image index is 1
                currentImg = 1;
        } else if (c == RIGHT) { // Go to next image
            currentImg++;
            if(currentImg == 4) // The maximum image index is 1
                currentImg = 3;
        } else if (c == TAB){
            break;
        }
        clearScreen();
        // Display image according to the current index
        if (currentImg == 1)
            displayImage(task1_1b_image1, SCREEN_WIDTH, SCREEN_HEIGHT);
        else if (currentImg == 2)
            displayImage(task1_1b_image2, SCREEN_WIDTH, SCREEN_HEIGHT);
        else if (currentImg == 3)
                displayImage(task1_1b_image3, 1024, 768);
        }
}

// Method for task 1 - 1c, video
void task1_1c_operation() {
    int currentFrame = 0;
    displayImage(epd_bitmap_allArray[currentFrame], VIDEO_FRAME_WIDTH, VIDEO_FRAME_HEIGHT);

    while(1) {
        set_wait_timer(1, 100); // Update the new frame every 0.1 seconds (10fps)
        char c = getUart();
        if (c == TAB) {
            break;
        }
        currentFrame++;
        if(currentFrame == 51) // The video has only 50 frames (50 seconds)
            currentFrame = 0;
        set_wait_timer(0, 100);
        displayImage(epd_bitmap_allArray[currentFrame], VIDEO_FRAME_WIDTH, VIDEO_FRAME_HEIGHT); // Display the current frame
    }
}

// Method for task 1 - 2, display font
void task1_2_operation() {
    // Display the name of the students
    // The implementation of the function is in the framebf library
    printString("Jaeheon Jeong", 50, 50, 0, 0x00AA0000, 4);
    printString("Sanghwa Jung", 50, 100, 0, 0x0000BB00, 4);
    printString("Le Thanh Tai", 50, 150, 0, 0x000000CC, 4);
    printString("Vo Hoang Tuan", 50, 200, 0x000000CC, 0x00FFFF00, 4);

    while(1) {
        char c = uart_getc();
        if (c == TAB)
            break;
    }
}

// Method to implement game logic
void gameOperation() {
    while(1) {
        if (stage == 1) { // Start game stage
            startGame(); // Display message
            while(1) {
                char c = uart_getc();
                if (c == '\n') { // Get enter to move to stage 2
                    printString("     ", xOffset, yOffset, 0, TEXT_COLOR, 4);
                    break;
                } else if (c == TAB)
                    break;
            }
            stage = 2;
            initScreen(); // Init the character, bomb and background
        } else if (stage == 2) { // Gaming stage
            set_wait_timer(1, 10); // Set timer for 10ms
            char c = getUart();

            // Update x, y offset of character based on the input
            if (c == UP) {
                if (jump == 0) // Check if the character is jumping or not. If not, set tag for the user to jump
                    jump = 6;
            } else if (c == LEFT) {
                if (xOffset - PAN_STEP >= 0)
                    xOffset -= PAN_STEP;
                updateCharacter();
            } else if (c == RIGHT) {
                if (xOffset + PAN_STEP <= SCREEN_WIDTH)
                    xOffset += PAN_STEP;
                updateCharacter();
            } else if (c == ENTER) { // Press enter to move to stage 3 (pause the game)
                stage = 3;
                continue;
            } else if (c == TAB)
                break;

            set_wait_timer(0, 10);
            count++; // Count increase every 10ms

            if (count % 10 == 0) { // 100 ms pass
                if (jump > 0) {
                    if (jump >= 4) // For the first 3 steps, jump on
                        yOffset -= JUMP_STEP;
                    else // For the last 3 steps, fall down to original y off set
                        yOffset += JUMP_STEP;
                    updateCharacter();
                    jump--;
                }
            
                // Update bom position accordingly
                updateBom(&bombs[bomIndex].x, &bombs[bomIndex].y); //
                updateDifficulty();

                // Check if the bom has reached the bottom of the screen
                for (int i = 0; i < 5; i++) {
                    if (bombs[i].y >= SCREEN_HEIGHT) {
                        bombs[i].x = custom_rand(); // Random X position within screen width
                        bombs[i].y = 0;             // Start at the top
                    }
                }

                // Reset horizontal of the bomb if go out of the screen
                if (hBomb.x >= SCREEN_WIDTH) {
                    hBomb.x = 0;
                    hBomb.y = HORIZONTAL_BOMB_Y_OFFSET;
                }

                // 1 seconds reach
                if (count == 100) {
                    timeCount--; // Count the timer
                    // Reset timer, move to new next level if count to 0
                    if (timeCount == 0) {
                        timeCount = SECONDS_PER_STAGE;
                        gameLevel++;

                        updateBackground();
                        if (gameLevel == 11) { // Win game condition
                            stage = 5; // Move to stage 5 (win game stage)
                            continue;
                        }
                    }
                    count = 0; // Reset count variable
                }
            }
            // Check if the game is lost or not. If the lose tag is on
            if (isLose == 1)
                stage = 4;
        }
        else if (stage == 3) { // Pause game stage
            showPause(); // Show message
            char c = uart_getc();
            if (c == ENTER) {
                // Remove the pause message, display the text again.
                displayLevel(gameLevel);
                showTime(timeCount);
                updateBackground();
                stage = 2;
            } else if (c == TAB)
                break;
        }
        else if (stage == 4) { // Lose game stage
            loseGame(); // Display message
            char c = uart_getc();
            if (c == ENTER)
                resetVariable(); // Reset variable to start a new game
            else if (c == TAB)
                    break;
        } else {
            winGame(); // Display message
            char c = uart_getc();
            if (c == ENTER)
                resetVariable(); // Reset variable to start a new game
            else if (c == TAB)
                    break;
        }
    }
}

// Method to update the difficulty based on the current level
void updateDifficulty() {
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
