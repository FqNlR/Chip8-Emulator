#pragma once

#include "raylib.h"
#include "chip8.hpp"
#include <cstring>
#include <cmath>

constexpr int SCALE = 15;
constexpr int WINDOW_HEIGHT = VIDEO_HEIGHT * SCALE;
constexpr int WINDOW_WIDTH = VIDEO_WIDTH * SCALE;
constexpr int FPS = 60;

constexpr int AUDIO_SAMPLE_RATE = 44100;
constexpr int AUDIO_BUFFER_SIZE = 512;

constexpr float BEEP_FREQUENCY = 440.0f;
constexpr float BEEP_AMPLITUDE = 0.20f;

constexpr int KEYMAP[16] =
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

void Update_Beeper(AudioStream, bool);