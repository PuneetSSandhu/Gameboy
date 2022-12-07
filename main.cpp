#include "GB.cpp"

int main(int argc, char** argv)
{
    if(argc < 1)
    {
        std::cout << "Usage: " << argv[0] << " <rom file>" << std::endl;
        return 1;
    }
    char * rom_file = argv[1];
    GB gb;
    GB_init(&gb);
    GB_load(&gb, rom_file);
    return 0;
}