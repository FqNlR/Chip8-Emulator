#include "chip8.hpp"
#include "front.hpp"

int main(int argc, char* argv[])
{
    if(argc < 3 || argc > 6)
    {
        std::cout << "Usage: chip8.exe [ROM_NAME] [CHIP-8 CYCLES PER FRAME] [--vy_shift --vx_jump]" << std::endl;
        return -1;
    }
    
    const int CYCLES_PER_FRAME = atoi(argv[2]);
    if(CYCLES_PER_FRAME <= 0)
    {
        std::cout << "ERROR: Cycles per frame must be equal to or greater than 1" << std::endl;
        return -1;
    }   

    chip8 chip;

    if(!chip.Load_ROM(argv[1]))
    {
        std::cout << "ERROR: Could not load ROM into memory" << std::endl;
        return -1;
    }
    
    if(argc > 3)
    {
        for(int i = 3; i < argc; i++)
        {
            if(strcmp(argv[i], "--vy_shift") == 0)
                chip.Set_Vy_Shift(true);

            if(strcmp(argv[i], "--vx_jump") == 0)
                chip.Set_Vx_Jump(true);
            
            if(strcmp(argv[i], "--legacy_indexing") == 0)
                chip.Set_Legacy_Indexing(true);
        }
    }

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "chip8");
    SetTargetFPS(FPS);

    bool emulation_running = true;
    while(!WindowShouldClose())
    {
        if(emulation_running)
        {
            for(int i = 0; i < CYCLES_PER_FRAME; i++)
            {
                for(int key = 0; key < 16; key++)
                    chip.Set_KeyPad(key, IsKeyDown(KEYMAP[key]));

                if(!chip.Cycle())
                {
                    emulation_running = false;
                    break;
                }
            }    
        }

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

    chip.Print_Registers();
    //chip.Print_Memory(0x0200, 0x0220);

    return 0;
}