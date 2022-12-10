#include "GB.hpp"
#include <iostream>

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
        // update the input
        GB_updateInput(gb);
    }
    GB_render(gb);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <rom file>" << std::endl;
        return 1;
    }
    char *rom_file = argv[1];
    GB gb;
    GB_init(&gb);
    GB_load(&gb, rom_file);

    update(&gb);

    GB_close(&gb);
    return 0;
}