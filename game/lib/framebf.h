// ----------------------------------- framebf.h -------------------------------------
void framebf_init();
void drawPixelARGB32(int x, int y, unsigned int attr);
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void displayImage(const unsigned long* bitmap, int image_width, int image_height, int xOffset, int yOffset);

void printFont(unsigned char *fontData, int x, int y, unsigned int color0, unsigned int color1, int pixelSize);
void printString(const char *str, int x, int y, unsigned int color0, unsigned int color1, int pixelSize);
unsigned char *getFontDataForCharacter(char character);
