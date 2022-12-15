#include "GB.hpp"
#include <iostream>
#include <SDL2/SDL.h>

void update(GB *gb)
{
    int MAX_CYCLES = 69905;
    int cycles = 0;
    while (cycles < MAX_CYCLES)
    {
        // run the next opcode
        cycles += GB_step(gb);
        // update the timers
        GB_updateTimers(gb, cycles);
        // update the graphics
        GB_updateGraphics(gb, cycles);
        // // update the input
        // GB_updateInput(gb);
        // update interrupts
        // GB_updateInterrupts(gb);
    }
    // GB_render(gb);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <rom file>" << std::endl;
        return 1;
    }

    GB gb;
    char *rom_file = argv[1];
    GB_init(&gb);

    // If the rom supplied is "default" then load the default rom
    if (strcmp(rom_file, "default") == 0)
    {
        GB_load(&gb, "roms/Tetris.gb");
    }
    else
    {
        GB_load(&gb, rom_file);
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create a window
    SDL_Window *win = SDL_CreateWindow("Gameboy Emulator", 100, 100, WIDTH * 2, HEIGHT * 2, SDL_WINDOW_SHOWN);
    if (win == nullptr)
    {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr)
    {
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create a texture
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (tex == nullptr)
    {
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Main loop
    bool quit = false;
    SDL_Event e;

    while (!quit)
    {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            // User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        // Clear screen
        SDL_RenderClear(ren);

        // Update the texture
        SDL_UpdateTexture(tex, NULL, gb.screen, WIDTH * sizeof(Uint32));

        // Render the texture
        SDL_RenderCopy(ren, tex, NULL, NULL);

        // Update the screen
        SDL_RenderPresent(ren);

        // Update the gameboy
        update(&gb);
    }

    // Destroy the texture
    SDL_DestroyTexture(tex);

    // Destroy the renderer
    SDL_DestroyRenderer(ren);

    // Destroy the window
    SDL_DestroyWindow(win);

    // Quit SDL subsystems
    SDL_Quit();

    // Quit the gameboy
    GB_close(&gb);

    return 0;
}