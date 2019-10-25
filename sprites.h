/**
 *      Sprite Objects for Ziggurat
 *      Written by Blitmaps (c) 2019
 * */
#ifndef _SPRITES_
#define _SPRITES_

#include "commanderx16.h"
#include <stdbool.h>

#define START_NON_TEXT_SPRITES 30 

/*
    size of sprite,
    bank of memory,
    16-bit address from 0-x
    the data.
*/
void load_sprite(uint8_t size, uint8_t bank, uint16_t addr, char *d)
{
    int ac = 0;
    char cc;
    char jj;
    for (cc = 0; cc < size; ++cc)
        for (jj = 0; jj < size; ++jj)
        {
            *(char *)VH = bank;
            *(char *)VM = addr >> 8;
            *(char *)VL = addr & 0xff;
            *(char *)VERA_DATA_REG = d[ac];
            ac++;
            addr++;
        }
}

void load_sprite_to_index(uint8_t size, uint16_t index, char *d)
{
    load_sprite(size, 1, 256*(index+SPRITE_OFFSET), d);
}

typedef struct sprite {
    char number;
    char sprite_index;
    uint16_t x;
    uint16_t y;
    uint16_t adr;
    bool enabled;
    bool landed;
    char speed;
    char col;
} sprite;

void generate_sprite(sprite *S, int number, int sprite_index, uint16_t x, uint16_t y)
{
    uint8_t B;

    S->number = number;
    S->landed = false;

    S->adr = ((S->number+START_NON_TEXT_SPRITES) * 8);
    // address 12:5
    //vpoke(0xf, 0x5000 + adr, 0);

    VERA_ADDRESS_16(0xf, 0x5000 + S->adr);
    B = ((SPRITE_OFFSET + sprite_index) * 256) >> 5 & 0xFF; 
    VERA_DATA(B);

    // address 16:13 (starting at 0x10000) and 8 bpp mode
    //vpoke(0xf, 0x5001 + adr, 0x88);
    VERA_ADDRESS_16(0xf, 0x5001 + S->adr);
    VERA_DATA((1 << 7) | (0x010000 >> 13) | ((SPRITE_OFFSET +sprite_index) * 256) >> 13);

    // set x
    S->x = x;
    S->y = y;
    // x coordinate 7:0
    //vpoke(0xf, 0x5002 + adr, (x & 0xff));
    VERA_ADDRESS_16(0xf, 0x5002 + S->adr);
    VERA_DATA(S->x & 0xff);

    // x coordinate 9:8
    VERA_ADDRESS_16(0xf, 0x5003 + S->adr);
    VERA_DATA(S->x >> 8);
    //vpoke(0xf, 0x5003 + adr, x >> 8);

    // y coordinate 7:0
    //vpoke(0xf, 0x5004 + adr, 0);
    VERA_ADDRESS_16(0xf, 0x5004 + S->adr);
    VERA_DATA(S->y & 0xff);

    // y coordinate 9:8
    VERA_ADDRESS_16(0xf, 0x5005 + S->adr);
    VERA_DATA(S->y >> 8);

    // z-depth: in front of layer 2
    VERA_ADDRESS_16(0xf, 0x5006 + S->adr);
    S->enabled = 0xFF;
    //VERA_DATA((rand() % 5 > 1) ? 0xcc : 0xc0);
    VERA_DATA(0xC); //0xcc
    //vpoke(0xf, 0x5006 + adr, 0x0c);

    VERA_ADDRESS_16(0xf, 0x5007 + S->adr);
    //VERA_DATA(0xf0);
    VERA_DATA(0x50);

    // Set default speed to 0;
    S->speed = 0;
}

void set_sprite_x(sprite *S, uint16_t x) {
    uint16_t adr = 0x5002+S->adr;
    S->x = x;
    // x coordinate 7:0
    //vpoke(0xf, 0x5002 + adr, (x & 0xff));
    *(char *)VH = 0x1F;
    *(char *)VM = adr >> 8;
    *(char *)VL = adr & 0xff;
    *(char *)VERA_DATA_REG = S->x & 0xff;
    *(char *)VERA_DATA_REG = S->x >> 8;
}

void set_sprite_y(sprite *S, uint16_t y) {
    uint16_t adr = 0x5004+S->adr;
    S->y = y;
    // x coordinate 7:0
    //vpoke(0xf, 0x5002 + adr, (x & 0xff));
    *(char *)VH = 0x1F;
    *(char *)VM = adr >> 8;
    *(char *)VL = adr & 0xff;
    *(char *)VERA_DATA_REG = S->y & 0xff;
    *(char *)VERA_DATA_REG = S->y >> 8;
}

void set_sprite_enabled(sprite *S, bool enabled) {
    S->enabled = enabled;
    VERA_ADDRESS_16(0xf, 0x5006 + S->adr);
    (S->enabled) ? *(char *)VERA_DATA_REG = 0xC : *(char *)VERA_DATA_REG = 0x0;
}
/*
    The second byte is currently broken!!
*/
void set_sprite_index(sprite *S, char sprite_index) {

    uint8_t B;

    S->sprite_index = sprite_index;
    VERA_ADDRESS_16(0xf, 0x5000 + S->adr);
    B = ((SPRITE_OFFSET + sprite_index) * 256) >> 5 & 0xFF; 
    VERA_DATA(B);

    // address 16:13 (starting at 0x10000) and 8 bpp mode
    //vpoke(0xf, 0x5001 + adr, 0x88);
    VERA_ADDRESS_16(0xf, 0x5001 + S->adr);
    VERA_DATA((1 << 7) | (0x010000 >> 13) | ((SPRITE_OFFSET +sprite_index) * 256) >> 13);
}

void set_sprite_size(sprite *S, char size) {

    VERA_ADDRESS_16(0xf, 0x5007 + S->adr);
    switch(size)
    {
        case 16: VERA_DATA(0x50); break;
        case 8: VERA_DATA(0x0); break;
    }
}
#endif