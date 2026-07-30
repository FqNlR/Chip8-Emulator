#include "chip8.hpp"
#include "include/raylib.h"
#include <cstring>

const int SCALE = 15;
const int WINDOW_HEIGHT = VIDEO_HEIGHT * SCALE;
const int WINDOW_WIDTH = VIDEO_WIDTH * SCALE;
const int FPS = 60;

int main(int argc, char* argv[])
{
    if(argc < 3 || argc > 5)
        return -1;

    chip8 chip;

    if(!chip.Load_ROM(argv[1]))
        return -1;
    
    if(argc > 3)
    {
        for(int i = 3; i < argc; i++)
        {
            if(strcmp(argv[i], "--vy_shift") == 0)
                chip.Set_Vy_Shift(true);

            if(strcmp(argv[i], "--vx_jump") == 0)
                chip.Set_Vx_Jump(true);
        }
    }

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "chip8");
    SetTargetFPS(FPS);

    const int CPS = atoi(argv[2]); //gets cycles per frame from command line input

    while(!WindowShouldClose())
    {
        for(int i = 0; i < CPS; i++)
            chip.Cycle();

        BeginDrawing();
        ClearBackground(BLACK);
        for(int i = 0; i < VIDEO_HEIGHT; i++)
        {
            for(int j = 0; j < VIDEO_WIDTH; j++)
            {
                if(chip.video[i * VIDEO_WIDTH + j] == 1)
                    DrawRectangle(j * SCALE, i * SCALE, SCALE, SCALE, WHITE);
            }
        }
        EndDrawing();

        chip.Update_Timers();
    }

    CloseWindow();

    std::system("cls");
    chip.Print_Registers();
    //chip.Print_Memory(0x0200, 0x0220);
    std::system("pause");

    return 0;
}