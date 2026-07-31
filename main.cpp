#include "chip8.hpp"
#include "front.hpp"

int main(int argc, char* argv[])
{
    if(argc < 3 || argc > 8)
    {
        std::cout << "Usage: chip8.exe [ROM_NAME] [CHIP-8 CYCLES PER FRAME] " << 
        "[--vy_shift --vx_jump --legacy_indexing --fx1e_sets_ov --logic_resets_vf]" << std::endl;
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
            
            if(strcmp(argv[i], "--fx1e_sets_ov") == 0)
                chip.Set_FX1E_Sets_OV(true);

            if(strcmp(argv[i], "--logic_resets_vf") == 0)
                chip.Set_Logic_Resets_VF(true);
        }
    }

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "chip8");
    SetTargetFPS(FPS);
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(AUDIO_BUFFER_SIZE);
    AudioStream beep_stream = LoadAudioStream(AUDIO_SAMPLE_RATE, 32, 1);
    PlayAudioStream(beep_stream);

    bool emulation_running = true;
    while(!WindowShouldClose())
    {
        for(int key = 0; key < 16; key++)
            chip.Set_KeyPad(key, IsKeyDown(KEYMAP[key]));
        
        if(emulation_running)
        {
            for(int i = 0; i < CYCLES_PER_FRAME; i++)
            {
                if(!chip.Cycle())
                {
                    emulation_running = false;
                    break;
                }
            }    
        }

        Update_Beeper(beep_stream, chip.Is_Sound_Active());

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

    StopAudioStream(beep_stream);
    UnloadAudioStream(beep_stream);
    CloseAudioDevice();
    CloseWindow();

    chip.Print_Registers();

    return 0;
}

void Update_Beeper(AudioStream stream, bool sound_active)
{
    static float phase = 0.0f;

    if(!IsAudioStreamProcessed(stream))
        return;

    std::array<float, AUDIO_BUFFER_SIZE> samples{};

    const float phase_step =
        2.0f * PI * BEEP_FREQUENCY / AUDIO_SAMPLE_RATE;

    for(float& sample : samples)
    {
        if(sound_active)
        {
            sample = std::sin(phase) * BEEP_AMPLITUDE;

            phase += phase_step;

            if(phase >= 2.0f * PI)
                phase -= 2.0f * PI;
        }
        else
            sample = 0.0f;
    }

    UpdateAudioStream(stream, samples.data(), AUDIO_BUFFER_SIZE);
}