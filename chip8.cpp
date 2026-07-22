#include "chip8.hpp"

const uint8_t FONT_SIZE = 80;
const uint8_t FONT_START_ADDRESS = 0x050;
const uint16_t ROM_START_ADDRESS = 0x200;

chip8::chip8(void)
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

    const uint16_t ROM_SIZE = rom.tellg();
    
    if(ROM_SIZE > MEMORY_SIZE - ROM_START_ADDRESS || ROM_SIZE <= 0)
    {
        rom.close();
        return false;
    }
    
    char* buffer = new char[ROM_SIZE];
	rom.seekg(0, std::ios::beg);
	rom.read(buffer, static_cast<std::streamsize>(ROM_SIZE));
	rom.close();

	for(long i = 0; i < ROM_SIZE; i++)
		memory[ROM_START_ADDRESS + i] = buffer[i];

	delete[] buffer;

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

bool chip8::Cycle(void)
{
    if(!Fetch())
        return false;
    Execute();
}

void chip8::OP_00E0(void) //clear screen
{
    for(int i = 0; i < VIDEO_HEIGHT * VIDEO_WIDTH; i++)
        video[i] = 0;
}

void chip8::OP_1NNN(void) //jump to NNN
{
    uint16_t jump_adress = opcode & 0x0FFF; // mask for 12 least significant bits

    pc = jump_adress;
}

void chip8::Print_Memory(int start)
{
    int aux = 0;

    for(int i = start; i < MEMORY_SIZE; i++)
    {
        if(aux % 16 == 0)
            std::cout << std::endl << "0x" << std::hex << std::setw(3) << std::setfill('0') << i << ": "; 

        std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(memory[i]) << ' ';
        aux++;
    }
}