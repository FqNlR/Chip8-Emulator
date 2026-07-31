#include "chip8.hpp"
#include <iomanip>
#include <fstream>

const uint8_t FONT_SIZE = 80;
const uint8_t FONT_START_ADDRESS = 0x050;
const uint16_t ROM_START_ADDRESS = 0x200;

chip8::chip8(void) //constructor
: rnd_generator(std::random_device{}()),
  rnd_byte_dist(0, 255)
{
    pc = ROM_START_ADDRESS;

    uint8_t font[FONT_SIZE] = 
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
            
    for(int i = 0; i < FONT_SIZE; i++)
        memory[FONT_START_ADDRESS + i] = font[i];
}

bool chip8::Load_ROM(const char* filename) 
{
    std::fstream rom(filename, std::ios::binary | std::ios::in | std::ios::ate);

    if(!rom)
        return false;

    std::streampos file_pos = rom.tellg();

    if(file_pos <= 0)
        return false;
    
    const std::streamsize ROM_SIZE = static_cast<std::streamsize>(file_pos);
    const std::streamsize AVAILABLE_MEMORY = static_cast<std::streamsize>(MEMORY_SIZE - ROM_START_ADDRESS);

    if(ROM_SIZE > AVAILABLE_MEMORY)
        return false;

	rom.seekg(0, std::ios::beg);
	
    if(!rom.read(reinterpret_cast<char*>(&memory[ROM_START_ADDRESS]), ROM_SIZE))
        return false;

    return true;
}

bool chip8::Fetch(void)
{
    if(pc >= MEMORY_SIZE - 1)
        return false;

    opcode = static_cast<uint16_t>(memory[pc]) << 8 | static_cast<uint16_t>(memory[pc + 1]);
    pc += 2;

    return true;
}

void chip8::Execute(void)
{
    switch(opcode & 0xF000) //decoder
    {
        case 0x0000:
            if(opcode == 0x00E0)
                OP_00E0();
            else if(opcode == 0x00EE)
                OP_00EE();
            else
                Warning(0);
            break;

        case 0x1000:
            OP_1NNN();
            break;

        case 0x2000:
            OP_2NNN();
            break;

        case 0x3000:
            OP_3XNN();
            break;

        case 0x4000:
            OP_4XNN();
            break;

        case 0x5000:
            if((opcode & 0x000F) == 0)
                OP_5XY0();
            else
                Warning(0);
            break;

        case 0x6000:
            OP_6XNN();
            break;

        case 0x7000:
            OP_7XNN();
            break;
        
        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x0000:
                    OP_8XY0();
                    break;
                case 0x0001:
                    OP_8XY1();
                    break;
                case 0x0002:
                    OP_8XY2();
                    break;
                case 0x0003:
                    OP_8XY3();
                    break;
                case 0x0004:
                    OP_8XY4();
                    break;
                case 0x0005:
                    OP_8XY5();
                    break;
                case 0x0006:
                    OP_8XY6();
                    break;
                case 0x0007:
                    OP_8XY7();
                    break;
                case 0x000E:
                    OP_8XYE();
                    break;
                default:
                    Warning(0);
            }
            break;

        case 0x9000:
            if((opcode & 0x000F) == 0)
                OP_9XY0();
            else
                Warning(0);
            break;

        case 0xA000:
            OP_ANNN();
            break;

        case 0xB000:
            OP_BNNN();
            break;

        case 0xC000:
            OP_CXNN();
            break;
        
        case 0xD000:
            OP_DXYN();
            break;

        case 0xF000:
            switch (opcode & 0x00FF)
            {
                case 0x0007:
                    OP_FX07();
                    break;
            
                case 0x0015:
                    OP_FX15();
                    break;
                
                case 0x0018:
                    OP_FX18();
                    break;

                case 0x000A:
                    OP_FX0A();
                    break;

                default:
                    Warning(0);
                    break;
            }
            break;

        case 0xE000:
            if((opcode & 0x00FF) == 0x009E)
                OP_EX9E();
            else if((opcode & 0x00FF) == 0x00A1)
                OP_EXA1();
            else
                Warning(0);
            break;

        default:
            Warning(0);
            break;
    }
}

bool chip8::Cycle(void)
{
    if(stop_execution_flag)
        return false;

    if(!Fetch())
        return false;
    
    Execute();

    if(stop_execution_flag)
        return false;
    
    return true;
}

void chip8::Update_Timers(void)
{
    if(delay_timer > 0)
        delay_timer--;
    if(sound_timer > 0)
        sound_timer--;
}

void chip8::Print_Registers(void) //prints all registers, pc and index
{
    for(int i = 0; i < 16; i++)
        std::cout << "register v" << i << ": " << std::hex << static_cast<int>(registers[i]) << std::endl;

    std::cout << "index: " << static_cast<int>(index) <<
    std::endl << "pc: " << static_cast<int>(pc) << 
    std::endl << "delay timer: " << static_cast<int>(delay_timer) <<
    std::endl << "sound timer: " << static_cast<int>(sound_timer) << std::endl;
}

void chip8::Print_Memory(const int start, const int end) //prints memory from start address to end adress
{
    if(start < 0 || start >= MEMORY_SIZE || end <= start || end > MEMORY_SIZE)
    {
        Warning(2);
        return;
    }

    for(int i = start; i < end; i++)
    {
        if((i - start) % 16 == 0)
            std::cout << std::endl << "0x" << std::hex << std::setw(3) << std::setfill('0') << i << ": "; 

        std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(memory[i]) << ' ';
    }
    std::cout << std::endl;
}

void chip8::Warning(const int error_type) //prints error message and stops execution
{
    switch (error_type)
    {
        case 0:
            std::cout << "\n\nUNKNOWN INSTRUCTION: 0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(opcode);
            break;
        case 1:
            std::cout << "\n\nSTACK ERROR\n\n";
            break;
        case 2:
            std::cout << "\n\nINVALID START OR END POINTS FOR MEMORY PRINT\n\n";
            return;
        case 3:
            std::cout << "\n\nINVALID JUMP INSTRUCTION\n\n";
            break;
        case 4:
            std::cout << "\n\nINVALID INDEX IN DRAW FUNCTION\n\n";
            break;
    }

    stop_execution_flag = true;
}

void chip8::Set_Vy_Shift(bool set)
{
    vy_shift = set;
}

void chip8::Set_Vx_Jump(bool set)
{
    vx_jump = set;
}

void chip8::Set_KeyPad(const int key, bool state)
{
    if(key >= 0 && key < 16)
        keypad[key] = state;
}