
/**
 *      Ziggurat Main Program
 *      Written by Blitmaps (c) 2019
 * */
#include <stdio.h>
#include "properties.h"
#include "assets.h"
#include "logic.h"
#include "levels.h"

void loop()
{   

    // Generate the memory for our blocks, all our props are stored in the
    // sprite struct type
    sprite *block_ptr = malloc(PROPS_GAMEFIELD_T_SIZE*sizeof(*block_ptr));
    sprite *beg = block_ptr;
    
    for (;;)
    {
        // Reset to the start of our game objects
        block_ptr = beg;

        if (CURRENT_SCREEN == TITLE_SCREEN)
        {
            title_screen(block_ptr);
            continue;
        }
        if (CURRENT_SCREEN == GAME_SCREEN)
        {
            play_level(block_ptr, 1);
            continue;
        }
        if (CURRENT_SCREEN == QUIT)
        {
            break;
        }
        // Generate level-1 here, while deving engine.
    }
    //free
    free(block_ptr);
}

int main(void)
{
    // Setup the hardware
    setup_hardware();

    // Setup the game
    setup();

    //Main loop
    loop();

    // reset the system
    cleanup();

    // Return
	return 0;
}