#include "chip8.hpp"
#include <cstring>

int main(int argc, char* argv[])
{
    if(argc < 3 || argc > 5)
        return -1;

    chip8 chip;

    if(!chip.Load_ROM(argv[1]))
        return -1;
    
    if(strcmp(argv[3], "--vy_shift") == 0 || strcmp(argv[4], "--vy_shift") == 0)
        chip.Set_Vy_Shift(true);

    if(strcmp(argv[3], "--vx_jump") == 0 || strcmp(argv[4], "--vx_jump") == 0)
        chip.Set_Vx_Jump(true);

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