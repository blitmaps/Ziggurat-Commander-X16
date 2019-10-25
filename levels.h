/**
 *      Level and Screen Specific Routines for Ziggurat
 *      Written by Blitmaps (c) 2019
 * */
#ifndef __LEVELS__
#define __LEVELS__

#include <stdint.h>
#include "assets.h"
#include "sprites.h" // sprite functions to load into hardware
#include "text.h"
#include "properties.h"
#include "commanderx16.h"
#include "logic.h"

void draw_checkerboard() { //const char index (64)
    uint16_t cc;
    char jj;
    char cl;
    char sw = 0;
    char color1 = 84;
    // Blank-Screen.
    // set new interrupt function
    // needs different local stack from 0xa000 to 0xa7ff, because the stack functions are not reentrant
    *(char *)VH = START_FRAME_MARKER;
    *(char *)VM = 0;
    *(char *)VL = 0;
    for (jj = 0; jj < (char)SCREEN_HEIGHT; ++jj)
    {
        (sw < 5) ? cl = 0 : cl = 5;
        (sw < 9) ? ++sw : sw = 0;
        if (jj % 5 == 0)
            color1++;
        for (cc = 0; cc < SCREEN_WIDTH; ++cc) 
        {
            if ((jj >= PROPS_GAMEFIELD_Y_S) && (jj < PROPS_GAMEFIELD_Y_E) &&
               (cc >= PROPS_GAMEFIELD_X_S) && (cc < PROPS_GAMEFIELD_X_E)) 
               {
                    if ((jj >= PROPS_GAMEFIELD_Y_S) && (jj < PROPS_GAMEFIELD_Y_S+10))
                    {
                        (cl < 5) ? *(char *)VERA_DATA_REG = DARK_BARK_GREEN : *(char *)VERA_DATA_REG = DANGER_RED;
                        (cl < 9) ? ++cl : cl = 0;
                    }
                    else
                    {
                        (cl < 5) ? *(char *)VERA_DATA_REG = DARK_BARK_GREEN : *(char *)VERA_DATA_REG = DARK_TURQOISE;
                        (cl < 9) ? ++cl : cl = 0;
                    }

               }
            else {
                (cl < 5) ? *(char *)VERA_DATA_REG = BLACK1: *(char *)VERA_DATA_REG = BLACK2;
                (cl < 9) ? ++cl : cl = 0;
            }
        }
    }
}


void display_lv_text(char lv){
    int a;
    char shorttxt[10] = "level     ";
    if (lv < 10)
    {        
        shorttxt[6] = '0';
        shorttxt[7] = lv+'0';
    }
    else
    {
        a = lv/10;
        shorttxt[6] = (a)+'0';
        lv-=(a*10);
        shorttxt[7] = (lv)+'0';

    }
    print_string(0, shorttxt, 128, 118);
}

void level(sprite *block_ptr, char lv) {
    
    // counters
    char f = 0;
    char sc1 = 0; 
    char sc2 = 0;
    display_lv_text(lv);
    draw_checkerboard();
    clear_level_progress();
    // f is used here to generate the game objects
    for (f = 0; f < PROPS_GAMEFIELD_T_SIZE; f++) {

        // Genrate our objects
        generate_sprite(block_ptr, f, 0, (sc1 * 16) + 110, (sc2 * 16) + 40);
        
        // allow some number to be set to active. 
        //set_sprite_enabled(block_ptr, (rand() % 5 > 1));
        set_sprite_enabled(block_ptr, ((sc2 * 16) > 110)); //110
        
        // random block type
        set_sprite_index(block_ptr, (rand() % 3) );

        // which col are we in
        block_ptr->col = sc1;

        block_ptr++;
        sc1++;

        if (sc1 == PROPS_GAMEFIELD_X_SIZE)
        {
            sc2++;
            sc1 = 0;
        }
    }

    print_string(0, "", 128, 118);
}

void set_level_properties()
{
    DIFFICULTY  = 1+(LVL/10);
    if (DIFFICULTY > 5)
        DIFFICULTY = 5;
    BLOCK_GEN_CHANCE = 97 - (LVL/10);
    SPEED = 100 - (LVL*3);
    if (SPEED < 1)
        SPEED = 1;
    return;
}

void title_screen(sprite *block_ptr)
{
    // vars
    uint16_t cc;
    uint16_t x;

    char co;
    char jj;
    sprite *p; 
    p = block_ptr;

    // turn off all the blocks
    for (co = 0; co < PROPS_GAMEFIELD_T_SIZE; ++co) 
    {
        set_sprite_enabled(p,0x0);
        ++p;
    }

    // Turn off all the text
    for (co = 0; co < 5; ++co)
        print_string(co, "", 120, 126);

    // Blank-Screen.
    // set new interrupt function
    // needs different local stack from 0xa000 to 0xa7ff, because the stack functions are not reentrant
    x = 0;
    *(char *)VH = START_FRAME_MARKER;
    *(char *)VM = 0;
    *(char *)VL = 0;
    for (cc = 0; cc < SCREEN_WIDTH; ++cc)      //320
        for (jj = 0; jj < (char)65; ++jj) //240
            {
                *(char *)VERA_DATA_REG = 0;
            }
    for (jj = 0; jj < (char)46; ++jj) //240
    {

        for (cc = 0; cc < 52; ++cc)      //320
            *(char *)VERA_DATA_REG = 0;
        for (cc = 0; cc < 215; ++cc)
        {
            *(char *)VERA_DATA_REG = background_0[x];
            ++x;
        }
        for (cc = 0; cc < 53; ++cc)      //320
            *(char *)VERA_DATA_REG = 0;
    }
    for (cc = 0; cc < SCREEN_WIDTH; ++cc)      //320
        for (jj = 111; jj < (char)SCREEN_HEIGHT; ++jj) //240
            {
                *(char *)VERA_DATA_REG = 0;
            }

    print_string(0, "enter(sta", 82-17, 156);
    print_string(1, "rt) > sta", 138, 156);
    print_string(2, "rt", 210, 156);

    print_string(3, "space(sele", 82+5, 176);
    print_string(4, "ct) > quit", 168, 176);


    print_string(5, "by blitmap", 100+80, 220);
    print_string(6, "s 2019", 180+80, 220);

    // Wait for a key to continue
    for (;;)
    {
        check_keyboard();
        if (KEY_UP[Enter])
        {
            CURRENT_SCREEN = GAME_SCREEN;
            break;
        }
        else if (KEY_UP[Space])
        {
            CURRENT_SCREEN = QUIT;
            break;
        }
    }

    // cleanup text
    for (co = 0; co < 7; ++co)
        print_string(co, "", 120, 126);

    // reset the score
    SCORE = 0;

}


void play_level(sprite *block_ptr, char start_level)
{
    sprite *beg = block_ptr;
    level(block_ptr, start_level);
    /*
        Main game loop
    */
    for (;;)
    {
        block_ptr = beg;
        /**
         * 
         * START OF MAIN LOOPING ROUTINE -
         * 1. Check Keyboard for keys
         * 2. Check global timer
         * 3. Adavance if timer has been hit
         **/
         
        // Pool the controller etc here.
        check_keyboard();

        // Toggle sprites in level
        if (KEY_UP[Ctrl] || KEY_UP[Space]) {
            toggle_sprites(block_ptr);
        }

        // pause if needed.
        if (KEY_UP[Enter]) {
            int c = 0;
            int a = 0;
            print_string(0, " paused", 125, 126);
            print_string(1, "to quit", 130, 136);
            print_string(2, "(ctrl+alt)", 117, 146);

            for (;;)
            {
                check_keyboard();
                if (KEY_UP[Enter]) {
                    break;
                }
                if (KEY_UP[Ctrl])
                    c = 1;
                if (KEY_UP[Alt])
                    a = 1;
                if (c & a)
                {
                    CURRENT_SCREEN = TITLE_SCREEN;
                    return;
                }
            }
            print_string(0, "", 120, 126);
            print_string(1, "", 120, 126);
            print_string(2, "", 120, 126);

        }

        // Tick up global timer from 0 -> speed, so if speed is higher then 
        // game will run slower.
        (GLOBAL_TIMER < SPEED) ? ++GLOBAL_TIMER : GLOBAL_TIMER=0;

        // Guard to move the blocks
        if (GLOBAL_TIMER)
            continue;

        // How many blocks
        (rand() % 100 > BLOCK_GEN_CHANCE) ? NUMBER_TO_GENERATE=NUMBER_TO_GENERATE : NUMBER_TO_GENERATE=0;

        // Perform the iteration and let us know what happened
        R = blocks_iteration(block_ptr, NUMBER_TO_GENERATE);

        // Reset difficulty.
        NUMBER_TO_GENERATE = DIFFICULTY;

        // draw score
        draw_score(1);

        // decision on level
        if (R == DEAD)
        {
            print_string(0, "game over", 120, 126);
            for (;;)
            {
                check_keyboard();
                if (KEY_UP[Enter] | KEY_UP[Space] | KEY_UP[Ctrl]) {
                    break;
                }
            }
            CURRENT_SCREEN = TITLE_SCREEN;
            break;
            //continue;
        }
        else if (R == COMPLETE)
        {
            print_string(0, "complete!!", 120, 126);
            ++LVL;
            block_ptr = beg;   
            level(block_ptr, LVL);
            /*
                Level parameters
            */
            set_level_properties();
        }
    }
}