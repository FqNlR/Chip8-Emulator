#include "chip8.hpp"

constexpr uint8_t REGF_ID = 0x0F;

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
    
    if(sp >= stack.size())
    {
        Warning(1);
        return;
    }        

    stack[sp] = pc;
    sp++;
    pc = subroutine_address;
}

void chip8::OP_00EE(void) //return from subroutine
{
    if(sp == 0)
    {
        Warning(1);
        return;
    }
    
    sp--; 
    pc = stack[sp];
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

    if(logic_resets_vf)
        registers[REGF_ID] = 0;
}

void chip8::OP_8XY2(void) //VX is set to result of bitwise AND of VX and VY
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;

    registers[regx_id] = registers[regx_id] & registers[regy_id];

    if(logic_resets_vf)
        registers[REGF_ID] = 0;
}   

void chip8::OP_8XY3(void) //VX is set to result of bitwise XOR of VX and VY
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;

    registers[regx_id] = registers[regx_id] ^ registers[regy_id];

    if(logic_resets_vf)
        registers[REGF_ID] = 0;
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
    
    if(value_y >= value_x)
        borrow = 1;

    registers[regx_id] = static_cast<uint8_t>(difference);
    registers[REGF_ID] = borrow;
}

void chip8::OP_8XY6(void) //may set the value of VX to the value of VY (--legacy flag) before rest of the operation.
                          //shifts the value of VX one bit to the right. affects VF based on the bit that was shifted
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;
    uint8_t shifted_bit = 0;
    
    if(vy_shift)
    registers[regx_id] = registers[regy_id];
    
    uint8_t value_x = registers[regx_id];
    
    if((value_x & 0x01) == 1)
        shifted_bit = 1;

    registers[regx_id] = value_x >> 1;
    registers[REGF_ID] = shifted_bit;
}

void chip8::OP_8XYE(void) //may set the value of VX to the value of VY (--legacy flag) before rest of the operation.
                          //shifts the value of VX one bit to the left. affects VF based on the bit that was shifted
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;
    uint8_t shifted_bit = 0;
    
    if(vy_shift)
    registers[regx_id] = registers[regy_id];
    
    uint8_t value_x = registers[regx_id];
    
    if((value_x & 0x80) == 0x80)
        shifted_bit = 1;

    registers[regx_id] = value_x << 1;
    registers[REGF_ID] = shifted_bit;
}

void chip8::OP_ANNN(void) //set index to value NNN
{
    uint16_t value = opcode & 0x0FFF;
    
    index = value;
}

void chip8::OP_CXNN(void) //VX is set to bitwise AND between NN and a random number
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t mask = opcode & 0x00FF;

    uint8_t rnd = static_cast<uint8_t>(rnd_byte_dist(rnd_generator));

    registers[regx_id] = rnd & mask;
}

void chip8::OP_BNNN(void) //set pc to address NNN + V0. if --vx_jump is enabled this instruction works more like BXNN,
                          //where pc is set to address XNN + VX.
{
    uint16_t address = opcode & 0x0FFF;
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint16_t target = 0;

    if(vx_jump)
        target = address + registers[regx_id];
    else
        target = address + registers[0];

    if(target >= MEMORY_SIZE)
    {
        Warning(3);
        return;
    }

    pc = target;
}

void chip8::OP_DXYN(void) //draws N pixels tall sprite from memory location in index, at the horizontal X coordinate
                          //represented by the value in VX and the Y coordinate in VY.
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t regy_id = (opcode & 0x00F0) >> 4;
    uint8_t value_x = registers[regx_id] & 63;
    uint8_t value_y = registers[regy_id] & 31;
    uint8_t N = opcode & 0x000F;
    bool stop_drawing = false;

    uint32_t sprite_end = static_cast<uint32_t>(index) + N;

    if(sprite_end > MEMORY_SIZE)
    {
        Warning(4);
        return;
    }
    
    registers[REGF_ID] = 0;
    
    for(int i = 0; i < N; i++)
    {
        uint8_t sprite = memory[index + i];

        for(int j = 0; j < 8; j++)
        {
            uint8_t screen_x = value_x + j;
            uint8_t screen_y = value_y + i;
            uint8_t bit = (sprite >> (7 - j)) & 0x0001;

            if(screen_x >= VIDEO_WIDTH)
                break;
                
            if(screen_y >= VIDEO_HEIGHT)
            {
                stop_drawing = true;
                break;
            }

            if(bit == 1 && video[screen_y * VIDEO_WIDTH + screen_x] == 1)
            {
                video[screen_y * VIDEO_WIDTH + screen_x] = 0;
                registers[REGF_ID] = 1;
            }
            else if(bit == 1 && video[screen_y * VIDEO_WIDTH + screen_x] == 0)
                video[screen_y * VIDEO_WIDTH + screen_x] = 1;

        }

        if(stop_drawing)
            break;
    }
}

void chip8::OP_FX07(void) //VX is set to value of the delay timer
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    
    registers[regx_id] = delay_timer;
}

void chip8::OP_FX15(void) //delay timer is set to value of VX
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;

    delay_timer = registers[regx_id];
} 

void chip8::OP_FX18(void) //sound timer is set to value of VX
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;

    sound_timer = registers[regx_id];
}

void chip8::OP_EX9E(void) //skips one instruction if the key corresponding to value of VX is pressed
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t key = 0;

    if(registers[regx_id] < 16)
        key = registers[regx_id];
    else
        return;

    if(keypad[key])
        pc += 2;
}

void chip8::OP_EXA1(void) //skips one instruction if the key corresponding to value of VX is not pressed
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t key = 0;

    if(registers[regx_id] < 16)
        key = registers[regx_id];
    else
        return;

    if(!keypad[key])
        pc += 2;
}

void chip8::OP_FX0A(void) //waits for keypad input and then sets VX to input value
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;

    if (!waiting_key_release)
    {
        for (int key = 0; key < 16; key++)
        {
            if (keypad[key])
            {
                waiting_key = static_cast<uint8_t>(key);
                waiting_key_release = true;

                pc -= 2;
                return;
            }
        }

        pc -= 2;
        return;
    }

    if (keypad[waiting_key])
    {
        pc -= 2;
        return;
    }

    registers[regx_id] = waiting_key;
    waiting_key_release = false;
}

void chip8::OP_FX1E(void) //adds value of VX to index. sets VF to 1 if value overflows
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t value_x = registers[regx_id];
    uint16_t result = index + value_x;

    index = result;

    if (fx1e_sets_ov)
        registers[REGF_ID] = result >= MEMORY_SIZE;
}

void chip8::OP_FX29(void) //sets index to address of hex character in VX
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t character = registers[regx_id] & 0x0F;
    
    index = FONT_START_ADDRESS + character * 5;
}

void chip8::OP_FX33(void) //converts byte in VX to three decimal digits and stores them at index, index+1 and index+2
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;
    uint8_t value = registers[regx_id];

    if(index + 2 >= MEMORY_SIZE)
    {
        Warning(4);
        return;
    }

    memory[index] = value / 100;
    memory[index + 1] = (value / 10) % 10;
    memory[index + 2] = value % 10;
}

void chip8::OP_FX55(void) //stores in successive memory addresses the values from V0 to VX
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;

    if(index + regx_id >= MEMORY_SIZE)
    {
        Warning(4);
        return;
    }

    for(int i = 0; i <= regx_id; i++)
        memory[index + i] = registers[i];

    if(legacy_indexing)
        index += regx_id + 1;
}

void chip8::OP_FX65(void) //stores successive memory addresses in the registers V0 to VX
{
    uint8_t regx_id = (opcode & 0x0F00) >> 8;

    if(index + regx_id >= MEMORY_SIZE)
    {
        Warning(4);
        return;
    }

    for(int i = 0; i <= regx_id; i++)
        registers[i] = memory[index + i];

    if(legacy_indexing)
        index += regx_id + 1;
}