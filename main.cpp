#include "chip8.hpp"

int main(int argc, char* argv[])
{
    if(argc != 3)
        return -1;

    chip8 chip;

    if(!chip.Load_ROM(argv[1]))
        return -1;
        
    /*for(int i = 0; i < atoi(argv[2]); i++)
    {
        chip.Cycle();
        std::system("cls");
        chip.Print_Registers();
        chip.Print_Memory(0x0200, 0x0220);
        std::system("pause");
    }*/

    return 0;
}