// Define a macro to place data in read-only memory
#define PROGMEM __attribute__((section(".rodata")))
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define DROP_RATE 50 // Adjust the drop rate as needed
#define PAN_STEP 50

extern int xOffset;
extern int yOffset; 
extern int lastX;
extern int jump; 
extern int timeCount;
extern int count;
extern int isLose; 
extern int stage;
extern int gameLevel;
extern int bomX; // Initial X position of the bom
extern int bomY;                     // Initial Y position of the bom
extern int bomX1; // Initial X position of the bom
extern int bomY1;    
extern unsigned long next;
extern int previous;

extern const unsigned long bom_img[] PROGMEM;
extern const unsigned long background [] PROGMEM;
extern const unsigned long character [] PROGMEM;
struct PixelData {
    unsigned long value;
    int status;
};
struct Bomb
{
    int x;
    int y;
};

extern struct PixelData screen[SCREEN_WIDTH][SCREEN_HEIGHT];    
extern struct Bomb bombs[5];
