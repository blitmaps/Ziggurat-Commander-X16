/**
 *      Game Properties for Ziggurat
 *      Written by Blitmaps (c) 2019
 * */
#ifndef __PROPS__
#define __PROPS__
#include <stdint.h>

#define PROPS_GAMEFIELD_X_SIZE 6
#define PROPS_GAMEFIELD_Y_SIZE 11
#define PROPS_GAMEFIELD_T_SIZE 66 // We do this by hand, to save the multiplication in loops

#define GAME_FIELD_CEILING 35
#define GAME_FIELD_FLOOR 200
#define GENERATE_BLOCK_ZONE 51

#define PROPS_GAMEFIELD_Y_S 35
#define PROPS_GAMEFIELD_X_S 110

#define PROPS_GAMEFIELD_Y_E 216
#define PROPS_GAMEFIELD_X_E 206
#define ALL_GOOD 0
#define DEAD 1
#define COMPLETE 2

// Colors 
#define DARK_RED 46 //40
#define DARK_YELLOW 84
#define DARK_TURQOISE 152
#define DARK_PINK 34
#define DARK_BARK_GREEN 88

#define D65_GRAY 17
#define D65_LIGHT_GRAY 19
#define DANGER_RED 49

#define BLACK1 16
#define BLACK2 17

// Screen codes
#define TITLE_SCREEN 0
#define GAME_SCREEN 1
#define QUIT 99

// return code from block iteration
char R = 0;

// current level
char LVL = 1;
    
    // Our game timer 0-65535
uint16_t GLOBAL_TIMER = 0;
    
// Our rotary speed on the timer 0-speed;
uint16_t SPEED = 100; //150 //5000 = BABY SLOW, 2500 = Reasonable challenging.
    
// Number of blocks to generate, more = harder.
char DIFFICULTY  = 1;
char NUMBER_TO_GENERATE = 1;

// Chance of block 1-chance 
char BLOCK_GEN_CHANCE = 97;

char CURRENT_SCREEN = 0;

#endif
