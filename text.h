/**
 *      Text Engine for Zigurat
 *      Blocks, Background
 *      Written by Blitmaps (c) 2019
 * */
#ifndef __TEXT__
#define __TEXT__
#include "font.h"
#include <stdint.h>
#include "sprites.h" // sprite functions to load into hardware


char STRING_1_CURRENT = 0;
char STRING_2_CURRENT = 0;
char STRING_3_CURRENT = 0;
char STRING_4_CURRENT = 0;
char STRING_5_CURRENT = 0;

// Which hardware sprite to start at, lower sprites in memory have 
// higher draw priority, so we shall start our text at 0.
#define STRING_SPRITE_START 0

// Which sprite index shall we start at.
#define SPRITE_BITMAP_OFFSET 3

void initialise_text_engine() {
    char counter = 0;
    // Load the fonts into index 3+
    for (counter = 0; counter < 83; ++counter) {
        load_sprite_to_index(8,SPRITE_BITMAP_OFFSET+counter,F[counter]);
    }
}

void print_string(char spring_bank, char *string, int16_t x, int16_t y) {
    
    int i = 0;
    uint16_t adr;
    int sprite_index;
    char c;
    char done = 0;
    
    while(i < 10) {
        
        c = ' ';
        if (!done)
        {
            (string[i] != '\0') ?  c = string[i] : done=1;
        }

        // calculate the hardware sprite 
        adr = (STRING_SPRITE_START + i + (spring_bank*10)) * 8;
        //calculate the character based on its ANSI code
        sprite_index = (c - 32);

        // ofset by the sprite bitmap location
        sprite_index += SPRITE_BITMAP_OFFSET;

        VERA_ADDRESS_16(0xf, 0x5000 + adr); 
        VERA_DATA(((SPRITE_OFFSET + sprite_index) * 256) >> 5 & 0xFF);

        // address 16:13 (starting at 0x10000) and 8 bpp mode
        //vpoke(0xf, 0x5001 + adr, 0x88);
        VERA_ADDRESS_16(0xf, 0x5001 + adr);
        VERA_DATA((1 << 7) | (0x010000 >> 13) | ((SPRITE_OFFSET +sprite_index) * 256) >> 13);

        // x coordinate 7:0
        //vpoke(0xf, 0x5002 + adr, (x & 0xff));
        VERA_ADDRESS_16(0xf, 0x5002 + adr);
        VERA_DATA((x + (8*i)) & 0xff);

        // x coordinate 9:8
        VERA_ADDRESS_16(0xf, 0x5003 + adr);
        VERA_DATA((x + (8*i)) >> 8);

        // y coordinate 7:0
        //vpoke(0xf, 0x5004 + adr, 0);
        VERA_ADDRESS_16(0xf, 0x5004 + adr);
        VERA_DATA(y & 0xff);

        // y coordinate 9:8
        VERA_ADDRESS_16(0xf, 0x5005 + adr);
        VERA_DATA(y >> 8);

        // z-depth: in front of layer 2
        VERA_ADDRESS_16(0xf, 0x5006 + adr);
        VERA_DATA(0xC); //0xcc

        VERA_ADDRESS_16(0xf, 0x5007 + adr);
        //VERA_DATA(0xf0);
        VERA_DATA(0);
        // Tick
        ++i;
    }
}

#endif