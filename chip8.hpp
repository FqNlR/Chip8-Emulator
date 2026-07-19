#pragma once

#include <iostream>
#include <cstdint>
#include <fstream>
#include <stack>
#include <chrono>
#include <thread>

const uint8_t VIDEO_HEIGHT = 32;
const uint8_t VIDEO_WIDTH = 64;
const uint16_t MEMORY_SIZE = 4096;

class chip8
{
    public:
        uint8_t keypad[16] = {};    
        bool video[VIDEO_HEIGHT * VIDEO_WIDTH] = {};
        uint8_t memory[MEMORY_SIZE] = {};

        chip8(void);
        int Load_ROM(const char* filename);
        void Cycle(void);

    private:
        uint16_t pc = 0;
        uint8_t sp = 0;
        uint8_t delay_timer = 0;
        uint8_t sound_timer = 0;
        uint8_t registers[16] = {};
        uint16_t index = 0;
        uint16_t opcode = 0;
        std::stack<uint16_t> stk;

        void Fetch(void);
        void Decode(void);
        void Execute(void);
};