#pragma once

#include <iostream>
#include <iomanip>
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
        
        chip8(void);
        bool Load_ROM(const char*);
        bool Cycle(void);
        void Print_Memory(const int, const int);
        void Print_Registers(void);
        
    private:
        uint16_t pc = 0;
        uint8_t delay_timer = 0;
        uint8_t sound_timer = 0;
        uint8_t registers[16] = {};
        uint8_t memory[MEMORY_SIZE] = {};
        uint16_t index = 0;
        uint16_t opcode = 0;
        std::stack<uint16_t> stack;

        bool Fetch(void);
        void Execute(void);
        void OP_00E0(void); //clear screen
        void OP_1NNN(void); //jump to NNN
        void OP_6XNN(void); //set register VX to value NN
        void OP_7XNN(void); //add value NN to register VX
        void OP_ANNN(void); //set index to value NNN
};