// Header library for font function

#define TEXT_COLOR 0x00FF0000
#define TEXT_SIZE 3
#define HEADING_SIZE 6
#define PIXEL_WIDTH_PER_CHARACTER 8 

#define TIME_TEXT_X_OFFSET 150
#define TIME_TEXT_Y_OFFSET 20
#define LEVEL_TEXT_X_OFFSET 904
#define LEVEL_TEXT_Y_OFFSET 20
#define LEVEL_VALUE_X_OFFSET 980
#define LEVEL_VALUE_Y_OFFSET 16
#define PAUSED_TEXT_Y_OFFSET 50
#define LOSE_TEXT_Y_OFFSET 20
#define WIN_TEXT_Y_OFFSET 20

void intToString(int value, char *str);
int showTime(int timeInSeconds);
void startGame();
void winGame();
void loseGame();
void displayLevel(int level);
void showPause();
