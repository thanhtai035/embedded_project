// Header library for rendering the game functions

#define CHARACTER_IMAGE_WIDTH 150
#define CHARACTER_IMAGE_HEIGHT 150
#define BOMB_IMAGE_WIDTH 64
#define BOMB_IMAGE_HEIGHT 64

#define HORIZONTAL_BOMB_Y_OFFSET 530

void initScreen();
void resumeScreen();
void updateCharacter();
void updateBackground();
void updateBom(int *bomX, int *bomY);
void updateBomHorizontal(int *bomX, int *bomY);
int custom_abs(int num);
int custom_rand();
void clearScreen();
