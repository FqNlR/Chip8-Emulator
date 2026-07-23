#include "chip8.hpp"

const uint8_t REGF_ID = 0x0F;

void chip8::OP_00E0(void) //clear screen
{
    for(int i = 0; i < VIDEO_HEIGHT * VIDEO_WIDTH; i++)
        video[i] = 0;
}

void chip8::OP_1NNN(void) //jump to NNN
{
    uint16_t jump_address = opcode & 0x0FFF;
    
    pc = jump_address;
}

void chip8::OP_2NNN(void) //calls subroutine at NNN
{
    uint16_t subroutine_address = opcode & 0x0FFF;
    
    stack.push(pc);
    pc = subroutine_address;
}

void chip8::OP_00EE(void) //return from subroutine
{
    if(stack.empty())
    {
        std::cout << "\nSTACK EMPTY!" << "\nERROR!\n";
        return;
    }
    
    uint16_t return_address = stack.top();
    stack.pop();
    
    pc = return_address;
}

void chip8::OP_3XNN(void) //skip one instruction if value in VX is equal to NN
{
    uint8_t reg_id = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;
    
    if(registers[reg_id] == value)
        pc += 2;
}

void chip8::OP_4XNN(void) //skip one instruction if value in VX is NOT equal to NN
{
    uint8_t reg_id = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;
    
    if(registers[reg_id] != value)
        pc += 2;
}

void chip8::OP_5XY0(void) //skip one instruction if value in VX is equal to value in VY
{
    uint8_t regy_id = (opcode & 0x00F0) >> 4;
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    
    if(registers[regx_id] == registers[regy_id])
        pc += 2;
}

void chip8::OP_9XY0(void) //skip one instruction if value in VX is NOT equal to value in VY
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;
    
    if(registers[regx_id] != registers[regy_id])
        pc += 2;
}

void chip8::OP_6XNN(void) //set register VX to value NN
{
    uint8_t reg_id = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;
    
    registers[reg_id] = value;
}

void chip8::OP_7XNN(void) //add value NN to register VX
{
    uint8_t reg_id = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;
    
    registers[reg_id] += value;
}

void chip8::OP_ANNN(void) //set index to value NNN
{
    uint16_t value = opcode & 0x0FFF;
    
    index = value;
}

void chip8::OP_8XY0(void) //VX is set to the value of VY
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;

    registers[regx_id] = registers[regy_id];
}

void chip8::OP_8XY1(void) //VX is set to result of bitwise OR of VX and VY
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;

    registers[regx_id] = registers[regx_id] | registers[regy_id];
}

void chip8::OP_8XY2(void) //VX is set to result of bitwise AND of VX and VY
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;

    registers[regx_id] = registers[regx_id] & registers[regy_id];
}   

void chip8::OP_8XY3(void) //VX is set to result of bitwise XOR of VX and VY
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;

    registers[regx_id] = registers[regx_id] ^ registers[regy_id];
}

void chip8::OP_8XY4(void) //VX is set to result of VX + VY. affects VF
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;

    uint16_t sum = registers[regx_id] + registers[regy_id];

    registers[regx_id] = static_cast<uint8_t>(sum);
    
    if(sum > 255)
        registers[REGF_ID] = 1;
    else
        registers[REGF_ID] = 0;
}

void chip8::OP_8XY5(void) //VX is set to result of VX - VY. affects VF
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;
    uint8_t value_x = registers[regx_id];
    uint8_t value_y = registers[regy_id];

    uint16_t difference = value_x - value_y;
    uint8_t borrow = 0;
    
    if(value_x >= value_y)
        borrow = 1;

    registers[regx_id] = static_cast<uint8_t>(difference);
    registers[REGF_ID] = borrow;
}

void chip8::OP_8XY7(void) //VX is set to result of VY - VX. affects VF
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;
    uint8_t value_x = registers[regx_id];
    uint8_t value_y = registers[regy_id];

    uint16_t difference = value_y - value_x;
    uint8_t borrow = 0;
    
    if(value_x >= value_y)
        borrow = 1;

    registers[regx_id] = static_cast<uint8_t>(difference);
    registers[REGF_ID] = borrow;
}

void chip8::OP_8XY6(void) //may set the value of VX to the value of VY (--legacy flag) before rest of the operation.
                          //shifts the value of VX one bit to the right. affects VF based on the bit that was shifted
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;

    if(legacy_flag == true)
        registers[regx_id] = registers[regy_id];
        
    if((registers[regx_id] & 0x01) == 1)
        registers[REGF_ID] = 1;
    else
        registers[REGF_ID] = 0;

    registers[regx_id] = registers[regx_id] >> 1;
}

void chip8::OP_8XYE(void) //may set the value of VX to the value of VY (--legacy flag) before rest of the operation.
                          //shifts the value of VX one bit to the left. affects VF based on the bit that was shifted
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;

    if(legacy_flag == true)
        registers[regx_id] = registers[regy_id];
        
    if((registers[regx_id] & 0x80) == 0x80)
        registers[REGF_ID] = 1;
    else
        registers[REGF_ID] = 0;

    registers[regx_id] = registers[regx_id] << 1;
}    