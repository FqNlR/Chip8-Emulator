#include "chip8.hpp"

int main(int argc, char* argv[])
{
    if(argc != 2)
        return -1;

    chip8 chip;

    if(!chip.Load_ROM(argv[1]))
        return -1;
        
    /*
    uint8_t current_instruction = 0;
    bool quit = false;
    while (!quit)
    {
        chip.Cycle();
    }
    */

    chip.Print_Memory(0);

    return 0;
}