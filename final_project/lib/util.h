// Define a macro to place data in read-only memory
#define PROGMEM __attribute__((section(".rodata")))
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define DROP_RATE 50   
#define TRAVEL_RATE 25
#define PAN_STEP 50
#define JUMP_STEP 70
#define SECONDS_PER_STAGE 15
#define BACKGROUND_Y_OFFSET 460

#define CHARACTER_TAG 1
#define BOMB_TAG 2

// Shared variables used in the game
extern int xOffset;
extern int yOffset;
extern int lastX;
extern int jump;
extern int timeCount;
extern int count;
extern int isLose;
extern int stage;
extern int gameLevel;
extern int bomX; 
extern int bomY;  
extern int bomX1; 
extern int bomY1;
extern unsigned long next;
extern int previous;
extern int bomIndex;
extern int bomIndex;

// Images values
extern const unsigned long bom_img[] PROGMEM;
extern const unsigned long background[] PROGMEM;
extern const unsigned long character[] PROGMEM;
extern const unsigned long bom_horizon_img[] PROGMEM;

// Pixel struct to store the data of the each pixel in the screen
struct PixelData
{
    unsigned long value; // color
    int status; // 
};
// Bomb struct to store x, v value of a bomb
struct Bomb
{
    int x;
    int y;
};

extern struct PixelData screen[SCREEN_WIDTH][SCREEN_HEIGHT];
extern struct Bomb bombs[5];
extern struct Bomb hBomb;

void resetVariable();
