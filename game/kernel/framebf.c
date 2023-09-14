// ----------------------------------- framebf.c -------------------------------------
#include "mbox.h"
#include "../uart/uart1.h"
#include "framebf.h"

//Use RGBA32 (32 bits for each pixel)
#define COLOR_DEPTH 32
//Pixel Order: BGR in memory order (little endian --> RGB in byte order)
#define PIXEL_ORDER 0
//Screen info
unsigned int width, height, pitch;
/* Frame buffer address
* (declare as pointer of unsigned char to access each byte) */
unsigned char *fb;



/**
* Set screen resolution to 1024x768
*/
void framebf_init()
{
mBuf[0] = 35*4; // Length of message in bytes
mBuf[1] = MBOX_REQUEST;
mBuf[2] = MBOX_TAG_SETPHYWH; //Set physical width-height
mBuf[3] = 8; // Value size in bytes
mBuf[4] = 0; // REQUEST CODE = 0
mBuf[5] = 1024; // Value(width)
mBuf[6] = 768; // Value(height)
mBuf[7] = MBOX_TAG_SETVIRTWH; //Set virtual width-height
mBuf[8] = 8;
mBuf[9] = 0;
mBuf[10] = 1024;
mBuf[11] = 768;
mBuf[12] = MBOX_TAG_SETVIRTOFF; //Set virtual offset
mBuf[13] = 8;
mBuf[14] = 0;
mBuf[15] = 0; // x offset
mBuf[16] = 0; // y offset
mBuf[17] = MBOX_TAG_SETDEPTH; //Set color depth
mBuf[18] = 4;
mBuf[19] = 0;
mBuf[20] = COLOR_DEPTH; //Bits per pixel
mBuf[21] = MBOX_TAG_SETPXLORDR; //Set pixel order
mBuf[22] = 4;
mBuf[23] = 0;
mBuf[24] = PIXEL_ORDER;
mBuf[25] = MBOX_TAG_GETFB; //Get frame buffer
mBuf[26] = 8;
mBuf[27] = 0;
mBuf[28] = 16; //alignment in 16 bytes
mBuf[29] = 0; //will return Frame Buffer size in bytes
mBuf[30] = MBOX_TAG_GETPITCH; //Get pitch
mBuf[31] = 4;
mBuf[32] = 0;
mBuf[33] = 0; //Will get pitch value here
mBuf[34] = MBOX_TAG_LAST;
// Call Mailbox
if (mbox_call(ADDR(mBuf), MBOX_CH_PROP) //mailbox call is successful ?
&& mBuf[20] == COLOR_DEPTH //got correct color depth ?
&& mBuf[24] == PIXEL_ORDER //got correct pixel order ?
&& mBuf[28] != 0 //got a valid address for frame buffer ?
) {
    
/* Convert GPU address to ARM address (clear higher address bits)
* Frame Buffer is located in RAM memory, which VideoCore MMU
* maps it to bus address space starting at 0xC0000000.
* Software accessing RAM directly use physical addresses
* (based at 0x00000000)
*/
mBuf[28] &= 0x3FFFFFFF;
// Access frame buffer as 1 byte per each address
fb = (unsigned char *)((unsigned long)mBuf[28]);
uart_puts("Got allocated Frame Buffer at RAM physical address: ");
uart_hex(mBuf[28]);
uart_puts("\n");
uart_puts("Frame Buffer Size (bytes): ");
uart_dec(mBuf[29]);
uart_puts("\n");
width = mBuf[5]; // Actual physical width
height = mBuf[6]; // Actual physical height
pitch = mBuf[33]; // Number of bytes per line
} else {
uart_puts("Unable to get a frame buffer with provided setting\n");
}
}
void drawPixelARGB32(int x, int y, unsigned int attr)
{
int offs = (y * pitch) + (COLOR_DEPTH/8 * x);
/* //Access and assign each byte
*(fb + offs ) = (attr >> 0 ) & 0xFF; //BLUE
*(fb + offs + 1) = (attr >> 8 ) & 0xFF; //GREEN
*(fb + offs + 2) = (attr >> 16) & 0xFF; //RED
*(fb + offs + 3) = (attr >> 24) & 0xFF; //ALPHA
*/
//Access 32-bit together
*((unsigned int*)(fb + offs)) = attr;
}
void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill)
{
for (int y = y1; y <= y2; y++ )
for (int x = x1; x <= x2; x++) {
if ((x == x1 || x == x2) || (y == y1 || y == y2))
drawPixelARGB32(x, y, attr);
else if (fill)
drawPixelARGB32(x, y, attr);
}
}


void printFont(unsigned char *fontData, int x, int y, unsigned int color0, unsigned int color1, int pixelSize)
{
    // Iterate through each row of the font data
    for (int row = 0; row < 8; row++)
    {
        // Get the binary pattern for the current row
        unsigned char rowData = fontData[row];
        
        // Iterate through each column of the font data
        for (int col = 0; col < 8; col++)
        {
            // Extract the bit for the current column
            unsigned char pixel = (rowData >> (7 - col)) & 0x01;

            // Calculate the position in the framebuffer
            int pixelX = x + (col * pixelSize);
            int pixelY = y + (row * pixelSize);

            // Draw the pixel based on the font data
            for (int i = 0; i < pixelSize; i++)
            {
                for (int j = 0; j < pixelSize; j++)
                {
                    // Calculate the offset for the current pixel
                    int pixelOffset = (pixelY + i) * pitch + (pixelX + j) * (COLOR_DEPTH / 8);

                    // Set the pixel color based on the font data
                    unsigned int pixelColor = (pixel == 1) ? color1 : color0;

                    // Set the pixel color in the framebuffer
                    *((unsigned int*)(fb + pixelOffset)) = pixelColor;
                }
            }
        }
    }
}




void printString(const char *str, int x, int y, unsigned int color0, unsigned int color1, int pixelSize)
{
    int currentX = x;

    // Iterate through each character in the input string
    while (*str)
    {
        // Get the character from the string
        char character = *str;

        // Check if the character is a valid printable character
        if (character >= ' ' && character <= '~')
        {
            // Get the corresponding font data for the character
            // You can create a lookup table or switch statement to map characters to font data
            unsigned char *fontData = getFontDataForCharacter(character);

            // Call the printFont2 function to print the character
            printFont(fontData, currentX, y, color0, color1, pixelSize);

            // Move the currentX position to the next character's position
            currentX += (8 * pixelSize); // Assuming each character is 8 pixels wide

            // Add some spacing between characters (adjust as needed)
            currentX += pixelSize; // Adjust spacing as needed
        }

        // Move to the next character in the input string
        str++;
    }
}



// Define font data for lowercase alphabet a to z and space
unsigned char fontData[63 * 8] = {
    //0
    0x0C, 0x1E, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C,

    //1
    0x0C, 0x1C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x0C,

    //2
    0x1E, 0x33, 0x03, 0x06, 0x0C, 0x18, 0x33, 0x3F,

    //3
    0x1E, 0x33, 0x03, 0x0E, 0x03, 0x33, 0x1E, 0x00,


    //4
    0x06, 0x0E, 0x1E, 0x36, 0x3F, 0x06, 0x06, 0x00,

    
    //5
    0x3F, 0x30, 0x30, 0x3E, 0x03, 0x33, 0x1E, 0x00,


    //6
    0x1E, 0x33, 0x30, 0x3E, 0x33, 0x33, 0x1E, 0x00,


    //7
    0x3F, 0x03, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x00,


    //8
    0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00,


    //9
    0x1E, 0x33, 0x33, 0x1F, 0x03, 0x33, 0x1E, 0x00,

    /* 'A' - ASCII 97 */
    0x0C, 0x1E, 0x33, 0x33,
    0x3F, 0x33, 0x33, 0x00,

    /* 'B' - ASCII 98 */
    0x3E, 0x33, 0x33, 0x3E,
    0x3B, 0x33, 0x33, 0x3E,

    //'C'
    0x1E, 0x33, 0x30, 0x30, 0x30, 0x33, 0x1E, 0x00,

    //'D'
    0x3C, 0x36, 0x33, 0x33, 0x33, 0x36, 0x3C, 0x00,

    //E
    0x3F, 0x30, 0x30, 0x3C, 0x30, 0x30, 0x3F, 0x00,

    //F
    0x3F, 0x30, 0x30, 0x3C, 0x30, 0x30, 0x30, 0x00,

    //G
    0x1E, 0x33, 0x30, 0x3B, 0x33, 0x33, 0x1F, 0x00,

    //H
    0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00,

    //I
    0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00,

    //J
    0x0F, 0x06, 0x06, 0x06, 0x36, 0x36, 0x1C, 0x00,


    //K
    0x33, 0x36, 0x3C, 0x38, 0x3C, 0x36, 0x33, 0x00,

    //L
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3F, 0x00,

    //M
    0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00,

    //N
    0x73, 0x73, 0x7B, 0x6B, 0x6F, 0x67, 0x67, 0x00,

    //O
    0x1E, 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x00,

    //P
    0x3E, 0x33, 0x33, 0x3E, 0x30, 0x30, 0x30, 0x00,

    //Q
    0x1E, 0x33, 0x33, 0x33, 0x3B, 0x36, 0x1D, 0x00,

    //R
    0x3E, 0x33, 0x33, 0x3E, 0x3C, 0x36, 0x33, 0x00,

    //S
    0x1F, 0x30, 0x30, 0x1E, 0x03, 0x03, 0x3E, 0x00,

    //T
    0x3F, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x00,


    //U
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x00,

    //V
    0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00,

    //W
    0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00,

    //X
    0x33, 0x33, 0x1E, 0x0C, 0x1E, 0x33, 0x33, 0x00,

    //Y
    0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x0C, 0x00,

    /* 'z' - ASCII 122 */
    0x3F, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x3F, 0x00,

    //a
    0x00, 0x00, 0x0E, 0x01, 0x1F, 0x32, 0x1F, 0x00,

    //b
    0x20, 0x20, 0x2E, 0x33, 0x33, 0x33, 0x2E, 0x00,

    //c
    0x00, 0x00, 0x1F, 0x30, 0x30, 0x33, 0x1F, 0x00,


    //d
    0x06, 0x06, 0x1E, 0x36, 0x36, 0x36, 0x1F, 0x00,

    //e
    0x00, 0x00, 0x1E, 0x33, 0x3E, 0x30, 0x1F, 0x00,

    //f
    0x0E, 0x19, 0x18, 0x3C, 0x18, 0x18, 0x18, 0x00,

    //g
    0x00, 0x1F, 0x36, 0x36, 0x36, 0x1E, 0x06, 0x1C,

    //h
    0x30, 0x30, 0x3C, 0x36, 0x36, 0x36, 0x36, 0x00,

    //i
    0x0C, 0x00, 0x1C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00,

    //j
    0x06, 0x00, 0x0E, 0x06, 0x06, 0x36, 0x1C, 0x00,

    //k
    0x30, 0x30, 0x36, 0x3C, 0x38, 0x3C, 0x36, 0x00,
    

    //l
    0x1C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00,

    //m
    0x00, 0x00, 0x6E, 0x7F, 0x6B, 0x6B, 0x6B, 0x00,
    

    //n
    0x00, 0x00, 0x3E, 0x33, 0x33, 0x33, 0x33, 0x00,

    //o
    0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00,

    //p
    0x00, 0x00, 0x2E, 0x33, 0x33, 0x3E, 0x30, 0x30,

    //q
    0x00, 0x00, 0x1E, 0x36, 0x36, 0x1F, 0x06, 0x06,

    //r
    0x00, 0x00, 0x3B, 0x36, 0x30, 0x30, 0x30, 0x00,

    //s
    0x00, 0x00, 0x1F, 0x30, 0x0E, 0x03, 0x3E, 0x00,

    //t
    0x18, 0x18, 0x3C, 0x18, 0x18, 0x19, 0x0E, 0x00,

    //u
    0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x00,

    //v
    0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00,

    //w
    0x00, 0x00, 0x63, 0x63, 0x6B, 0x3E, 0x36, 0x00,

    //x
    0x00, 0x00, 0x33, 0x1E, 0x0C, 0x1E, 0x33, 0x00,

    //y
    0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x38,

    //z
    0x00, 0x00, 0x3F, 0x06, 0x0C, 0x18, 0x3F, 0x00,

    /* ' ' (space) - ASCII 32 */
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};



// Function to get font data for a character
unsigned char *getFontDataForCharacter(char character)
{

    if (character >= '0' && character <= '9')
    {
        // Calculate the index in the fontData array based on ASCII value
        int index = character - '0';

        // Return a pointer to the font data for the digit
        return &fontData[index * 8]; // Each character is 8 bytes in size
    }
    // Check if the character is a lowercase letter or space
    else if (character >= 'A' && character <= 'Z')
    {
        // Calculate the index in the fontData array based on ASCII value
        int index = character - 'A' + 10;

        // Return a pointer to the font data for the character
        return &fontData[index * 8]; // Each character is 8 bytes in size
    }
    else if (character >= 'a' && character <= 'z')
    {
        // Calculate the index in the fontData array based on ASCII value
        int index = character - 'a' + 36;

        // Return a pointer to the font data for the character
        return &fontData[index * 8]; // Each character is 8 bytes in size
    }
    else if (character == ' ')
    {
        // Return a pointer to the font data for space
        return &fontData[62 * 8]; // Space character is at the end of the array
    }
    else
    {
        // Character not supported, return NULL or handle appropriately
        return (unsigned char*)0xFF;
    }
}