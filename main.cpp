#include "GB.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <rom file>" << std::endl;
        return 1;
    }
    char * rom_file = argv[1];
    GB gb;
    GB_init(&gb);
    GB_load(&gb, rom_file);
    GB_close(&gb);
    return 0;
}