#include "chip8.hpp"
#include <cstring>

int main(int argc, char* argv[])
{
    if(argc != 3 && argc != 4)
        return -1;

    chip8 chip;

    if(!chip.Load_ROM(argv[1]))
        return -1;
    
    if(argc == 4)
    {
        if(strcmp(argv[3], "--legacy") == 0)
            chip.legacy_flag = 1;
    }

    for(int i = 0; i < atoi(argv[2]); i++)
    {
        std::system("cls");
        chip.Print_Registers();
        chip.Print_Memory(0x0200, 0x0220);
        std::system("pause");
        
        if(!chip.Cycle())
            break;
    }

    return 0;
}