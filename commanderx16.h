/**
 *      Commander X16 Code for Zigurat
 *      VERA chip and Memory location code
 *      Written by Blitmaps (c) 2019
 * */
#ifndef _COMMANDER_
#define _COMMANDER_
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <6502.h>

// Consts
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define VERA_DATA_REG 0x9f23
#define VH 0x9f22
#define VM 0x9f21
#define VL 0x9f20
#define VERA_ISR 0x9f27

#define START_FRAME_MARKER 0x10

#define JOY1 0x02BC//0xEF

#define Ctrl 0
#define Alt	1
#define Space 2
#define Enter 3
#define Cursor_Up 4
#define Cursor_Down 5
#define Cursor_Left 6
#define Cursor_Right 7

char KEY_CODES[8] = {128,64,32,16,8,4,2,1};

// Screen RAM
#define SPRITE_OFFSET 44 //44


void VERA_ADDRESS(uint8_t h, uint8_t m, uint8_t l)
{
    *(char *)VH = h;
    *(char *)VM = m;
    *(char *)VL = l;
}
void VERA_ADDRESS_16(uint8_t h, uint16_t address)
{
    *(char *)VH = h;
    *(char *)VM = address >> 8;
    *(char *)VL = address & 0xff;
}

void VERA_DATA_BULK(uint8_t *d, uint8_t n)
{
    int cc;
    for (cc = 0; cc < n; cc++)
    {
        *(char *)VERA_DATA_REG = *d;
        d++;
    }
}

void VERA_DATA(uint8_t d)
{
    *(char *)VERA_DATA_REG = d;
}

void enable_sprites()
{
    // Set the sprites enable address
    VERA_ADDRESS_16(0xf, 0x4000);
    VERA_DATA(1);
}

void blank_screen(const char index)
{
    uint16_t cc;
    char jj;
    // Blank-Screen.
    // set new interrupt function
    // needs different local stack from 0xa000 to 0xa7ff, because the stack functions are not reentrant
    *(char *)VH = START_FRAME_MARKER;
    *(char *)VM = 0;
    *(char *)VL = 0;
    for (cc = 0; cc < SCREEN_WIDTH; ++cc)      //320
        for (jj = 0; jj < (char)SCREEN_HEIGHT; ++jj) //240
            *(char *)VERA_DATA_REG = index;
}

void setup_hardware()
{
    char vc;
    // switch back to uppercase character set
    asm("lda #$8e");
    asm("jsr BSOUT");
    
    // Vera control
    *((uint8_t *)0x9f25) = 0x0;
    
    // bad hack: redefine CC65 stack to $0xa800-0xafff, should be a proper x16 custom target
    *((uint16_t *)0x02) = 0xb000;

    // Vera control
    *((uint8_t *)0x9f25) = 0x0;

    // Scale and init
    VERA_ADDRESS(0x1F, 0x0, 0x0);
    VERA_DATA(1);
    VERA_DATA(64);
    VERA_DATA(64);

    // Layer for drawing
    
    VERA_ADDRESS(0x1F, 0x20, 0x0);
    VERA_DATA(0xE1);
    for (vc = 0; vc < 9; vc++)
       VERA_DATA(0);
    // layer 1
    VERA_ADDRESS(0x1F, 0x30, 0x0);
    VERA_DATA(0);
    for (vc = 0; vc < 9; vc++)
        VERA_DATA(0);
    
    
    // Fill will pallete x (this is v slow.)
    enable_sprites();
}

// Return to basic cleanly
void cleanup() {
    // switch back to uppercase character set
    asm("lda #$8e");
    asm("jsr BSOUT");
    // Vera control
    VERA_ADDRESS(0x1F, 0x0, 0x0);
    VERA_DATA(0x1);
    VERA_DATA(128);
    VERA_DATA(128);
    // CINT
    asm("jsr $FF81");
    printf("\n");
    printf("thanks for playing!");
    printf("\n");
}
// Keyboard handler
char KEYBOARD_LAST[8] = {0,0,0,0,0,0,0,0};
char KEYBOARD_THIS[8] = {0,0,0,0,0,0,0,0};
char KEY_UP[8] = {0,0,0,0,0,0,0,0};

void check_keyboard() {
    char i;

    // KERNAL SUBROUTINE GETJOY -> Stores answer in 0xEF
    asm("JSR $FF06"); 

    for (i = 0; i < 8; ++i)
    {
        KEY_UP[i] = 0;
        if ((*(char *)JOY1 & KEY_CODES[i])) {
            KEYBOARD_THIS[i] = 1;
            KEY_UP[i] = 0;
        }
        else if (KEYBOARD_LAST[i] == 1) {
            KEYBOARD_THIS[i] = 0;
            KEY_UP[i] = 1;
        }
    }
    
    for (i = 0; i < 8; ++i)
        KEYBOARD_LAST[i] = KEYBOARD_THIS[i];
}


#endif
