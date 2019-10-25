/**
 *      Game Logic for Ziggurat, independent of levels
 *      Written by Blitmaps (c) 2019
 * */
#ifndef __LOGIC__
#define __LOGIC__
#include "sprites.h"
#include "properties.h"
#include "commanderx16.h"
#include "assets.h"
#include "text.h"

// Setup game
void setup() {
    
    initialise_text_engine();

    // Load block 0 bitmap into the first vblock of memory
    print_string(0, "loading", 128, 118);

    // Load the sprites into index 0-2
    load_sprite_to_index(16,0,BLOCK[0]);
    load_sprite_to_index(16,1,BLOCK[1]);
    load_sprite_to_index(16,2,BLOCK[2]);
    
    // Load the text
    print_string(0, "", 128, 118);

}

char BIG_SCORE = 0;
uint16_t SCORE;
char SCORE_STRING[10] = {0x53,0x43,0x52,0x20,0x30,0x30,0x30,0x30,0x30,0x30};

void draw_score(int index)
{
    int a;
    uint16_t sc = SCORE;
    SCORE_STRING[0] = 0x53;
    SCORE_STRING[1] = 0x43;
    SCORE_STRING[2] = 0x52;
    SCORE_STRING[3] = 0x20;
    SCORE_STRING[4] = 0x30+BIG_SCORE;

    a = (sc / 10000);
    SCORE_STRING[5] = 0x30+a;
    sc -= (a*10000);

    a = (sc / 1000);
    SCORE_STRING[6] = 0x30+a;
    sc -= (a*1000);

    a = (sc / 100);
    SCORE_STRING[7] = 0x30+a;
    sc -= (a*100);

    a = (sc / 10);
    SCORE_STRING[8] = 0x30+a;

    print_string(index, SCORE_STRING, 230, 220);
}

void increase_score(char score) {
    if (SCORE > ((long)65535L-(long)score)) {
        BIG_SCORE +=1;
    }
    SCORE +=score;

}

char LEVEL_CLEARED[6] = {0,0,0,0,0,0};

void clear_level_progress() {
    LEVEL_CLEARED[0] = 0;
    LEVEL_CLEARED[1] = 0;
    LEVEL_CLEARED[2] = 0;
    LEVEL_CLEARED[3] = 0;
    LEVEL_CLEARED[4] = 0;
    LEVEL_CLEARED[5] = 0;
};


char blocks_iteration(sprite *sprites, char generate) {

    //counter
    char co, cl;
    // Should move?
    volatile char ok;
    // temp memory address var
    uint16_t adr;
    // Spare blocks vars
    char spare_block[16] = {99,99,99,99,99,99,99,99,
                            99,99,99,99,99,99,99,99};
    char generate_xzone[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    // Should we auto generate some new blocks
    char all_landed = 1;

    char sp = 0;
    char cx = 0;

    // sprite pointers
    sprite *p;
    sprite *beg;
    sprite *c;

    // remember the start of the sprites
    beg = sprites;
    p = beg;

    for (co = 0; co < PROPS_GAMEFIELD_T_SIZE; ++co) 
    {
        // if landed continue
        if (p->landed)                     {
            ++p; continue;                 }
        if (LEVEL_CLEARED[p->col])         
        {
            if (p->enabled){
                set_sprite_enabled(p,0x0);
                p->enabled = false;
            }
            
            ++p; 
            continue;                 
        }
        
        // if disabled, keep a log so we can use the block, and continue
        if (!p->enabled)
        {
            // Tick up
            ++p;
            // if spare-blocks count is 15 then continue
            if (sp == 15)
                continue;
            // otherwise add to the block count
            spare_block[sp]=co;
            ++sp;
            continue;
        }

        // Block has not landed, but is enabled, so we 
        // should turn off the auto-make blocks
        all_landed = 0;

        // Special check to see if we have touched the game-floor
        if (p->y >= GAME_FIELD_FLOOR)      {
            p->landed = true;
            ++p; continue;                 }
        
        // Check region is clear for generating blocks
        if (p->y < GENERATE_BLOCK_ZONE)     {
            generate_xzone[cx] = p->x;
            ++cx;
                                            }

        // set pointer c (comparison block) to the beginning of the blocks        
        c = beg;
        
        // Ok to move is set to 1. 
        ok = 1;
        
        // Check all the blocks
        for (cl = 0; cl < PROPS_GAMEFIELD_T_SIZE; ++cl) 
        {
            // if not enabled, then continue
            if (!c->enabled)    {
                ++c; continue;  }
            // if not the same x-cords, then they can't colide, so continue
            if (!(p->x == c->x)){
                ++c; continue;  }
            // If the same memory address, then same object, continue
            if (&c == &p)       {
                ++c; continue;  }
            

            /*
                Actually check if there will be a touch-collision
            */
            if ( p->y == (c->y-(char)16) )
            {
                if ((p->sprite_index == c->sprite_index) && c->landed) {
                    set_sprite_enabled(p,0x0);
                    set_sprite_enabled(c,0x0);
                    p->landed = false;
                    c->landed = false;
                    increase_score(20);
                    /*
                        check if our line is clear
                    */
                   if (c->y >= GAME_FIELD_FLOOR) {
                       LEVEL_CLEARED[c->col] = 1;
                   }
                }
                else 
                {
                    // if our y is greater than a threshold, then Game Over
                    if (p->y < GAME_FIELD_CEILING+16)
                    {
                        return DEAD;
                    }
                    else
                    {
                        p->landed = true;
                    }
                }

                ok=0; 
                break;
            }
            ++c;
        }

        if (ok) {
            //set_sprite_y(p,p->y+speed); // inline for optimisation
            adr = 0x5004+p->adr;
            p->y+=1;
            *(char *)VH = 0x1F;
            *(char *)VM = adr >> 8;
            *(char *)VL = adr & 0xff;
            *(char *)VERA_DATA_REG = p->y & 0xff;
            *(char *)VERA_DATA_REG = p->y >> 8;
        }

        // Finally ++
        ++p;
    }

    // autogenerate
    generate += all_landed;

    // Generate new blocks 
    if (!generate)
        return ALL_GOOD;
    
    // spare counter
    sp = 0;
    // hang check, if this tips over then we return
    cx = 0;
    // our counter
    cl = 0;
    while ((cl < generate) && cx < 25) {
        
        // Which free block shall we try and make fall
        sp = rand() % 16;

        // increase tried counter
        ++cx;
        // if blank continue
        if (spare_block[sp] == 99)
            continue;
        ++cl; // tick up generated
        
        p=beg; // reset our block counter
        for (co=0; co < spare_block[sp]; co++)
            *p++; // fast-forward through to find our block (I know +=x should work, but it didn't)
        
        // Skip if our column in finished.
        if (LEVEL_CLEARED[p->col])
            continue;

        // Check collisions before generating.
        if ((p->x == generate_xzone[0]) ||
            (p->x == generate_xzone[1]) ||
            (p->x == generate_xzone[2]) ||
            (p->x == generate_xzone[3]) ||
            (p->x == generate_xzone[4]) ||
            (p->x == generate_xzone[5]) ||
            (p->x == generate_xzone[6]) ||
            (p->x == generate_xzone[7]) ||
            (p->x == generate_xzone[8]) ||
            (p->x == generate_xzone[9]))
                continue;

        // Generate the block;
        set_sprite_y(p,(char)PROPS_GAMEFIELD_Y_S);
        set_sprite_enabled(p, (char)0xF);
        set_sprite_index(p, (char)(rand() % 3));

        // kill the block
        spare_block[sp] = 99;
    }

    if (LEVEL_CLEARED[0] && LEVEL_CLEARED[1] &&
        LEVEL_CLEARED[2] && LEVEL_CLEARED[3] &&
        LEVEL_CLEARED[4] && LEVEL_CLEARED[5])
        return COMPLETE;
    
    // otherwise...
    return ALL_GOOD;
}

void toggle_sprites(sprite *sprites) {
    
    char co;
    
    // sprite pointers
    sprite *p;
    sprite *beg;

    // remember the start of the sprites
    beg = sprites;
    p = beg;

    // For each sprite, if landed, change the sprite index
    // by one.
    for (co = 0; co < PROPS_GAMEFIELD_T_SIZE; ++co) {
        if (!p->landed)   {
            ++p; continue;}
        switch (p->sprite_index)
        {
            case 0: set_sprite_index(p,1); break;
            case 1: set_sprite_index(p,2); break;
            case 2: set_sprite_index(p,0); break;
        }
        ++p;
    }
}
#endif