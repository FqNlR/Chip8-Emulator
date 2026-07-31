#pragma once

#include "raylib.h"
#include "chip8.hpp"
#include <cstring>

const int SCALE = 15;
const int WINDOW_HEIGHT = VIDEO_HEIGHT * SCALE;
const int WINDOW_WIDTH = VIDEO_WIDTH * SCALE;
const int FPS = 60;

const int KEYMAP[16] =
{   
    //Keyboard --- CHIP-8 equivalent
    KEY_X,          //0
    KEY_ONE,        //1
    KEY_TWO,        //2
    KEY_THREE,      //3
    KEY_Q,          //4
    KEY_W,          //5
    KEY_E,          //6
    KEY_A,          //7
    KEY_S,          //8
    KEY_D,          //9
    KEY_Z,          //A
    KEY_C,          //B
    KEY_FOUR,       //C
    KEY_R,          //D
    KEY_F,          //E
    KEY_V           //F
};