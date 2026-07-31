#pragma once

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <fstream>
#include <array>
#include <random>

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
        void Update_Timers(void);
        void Print_Memory(const int, const int);
        void Print_Registers(void);
        void Set_Vy_Shift(bool);
        void Set_Vx_Jump(bool);
        
    private:
        uint16_t pc = 0;
        uint8_t delay_timer = 0;
        uint8_t sound_timer = 0;
        uint8_t registers[16] = {};
        uint8_t memory[MEMORY_SIZE] = {};
        uint16_t index = 0;
        uint16_t opcode = 0;
        uint8_t sp = 0;
        std::array<uint16_t, 16> stack;
        std::mt19937 rnd_generator;
        std::uniform_int_distribution<int> rnd_byte_dist;
        bool stop_execution_flag = false;
        bool vy_shift = false;
        bool vx_jump = false;
        
        bool Fetch(void);
        void Execute(void);
        void Warning(const int); //prints error message and pauses execution
        void OP_00E0(void); //clears screen
        void OP_1NNN(void); //jumps to NNN
        void OP_2NNN(void); //calls subroutine at NNN
        void OP_00EE(void); //returns from subroutine
        void OP_3XNN(void); //skips one instruction if value in VX is equal to NN
        void OP_4XNN(void); //skips one instruction if value in VX is NOT equal to NN
        void OP_5XY0(void); //skips one instruction if value in VX is equal to value in VY
        void OP_9XY0(void); //skips one instruction if value in VX is NOT equal to value in VY
        void OP_6XNN(void); //sets register VX to value NN
        void OP_7XNN(void); //adds value NN to register VX
        void OP_8XY0(void); //VX is set to the value of VY
        void OP_8XY1(void); //VX is set to result of bitwise OR of VX and VY
        void OP_8XY2(void); //VX is set to result of bitwise AND of VX and VY
        void OP_8XY3(void); //VX is set to result of bitwise XOR of VX and VY
        void OP_8XY4(void); //VX is set to result of VX + VY. affects VF
        void OP_8XY5(void); //VX is set to result of VX - VY. affects VF
        void OP_8XY7(void); //VX is set to result of VY - VX. affects VF
        void OP_8XY6(void); //may set the value of VX to the value of VY (--vy_shift flag) before rest of the operation.
                            //shifts the value of VX one bit to the right. affects VF based on the bit that was shifted
        void OP_8XYE(void); //may set the value of VX to the value of VY (--vy_shift flag) before rest of the operation.
                            //shifts the value of VX one bit to the left. affects VF based on the bit that was shifted
        void OP_ANNN(void); //sets index to value NNN
        void OP_BNNN(void); //sets pc to address NNN + V0. if --vx_jump is enabled this instruction works more like BXNN,
                            //where pc is set to address XNN + VX.
        void OP_CXNN(void); //VX is set to bitwise AND between NN and a random number
        void OP_DXYN(void); //draws N pixels tall sprite from memory location in index, at the horizontal X coordinate
                            //represented by the value in VX and the Y coordinate in VY.
        void OP_FX07(void); //VX is set to value of the delay timer
        void OP_FX15(void); //delay timer is set to value of VX 
        void OP_FX18(void); //sound timer is set to value of VX
};