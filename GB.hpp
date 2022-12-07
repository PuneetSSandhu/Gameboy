// A gameboy emulator written in C++.

#define MEMORY 0x10000
#define HEIGHT 144
#define WIDTH 160
#define MAX_SPRITES 40
#define MAX_SPRITES_PER_LINE 10
#define CLOCK_SPEED 4194304
#define REFRESH_RATE 60
#define CYCLES_PER_FRAME (CLOCK_SPEED / REFRESH_RATE)
#define CYCLES_PER_LINE (CYCLES_PER_FRAME / HEIGHT)
#define CYCLES_PER_PIXEL (CYCLES_PER_LINE / WIDTH)

#define WORD unsigned int
#define BYTE unsigned char

// Flag values
#define FLAG_Z 7
#define FLAG_N 6
#define FLAG_H 5
#define FLAG_C 4

typedef union Register{
    struct{
        BYTE low;
        BYTE high;
    };
    WORD word;
}Register;

typedef struct GameBoy{
    BYTE screen [HEIGHT][WIDTH][3];
    Register AF;
    Register BC;
    Register DE;
    Register HL;

    WORD pc; // init to 0x100
    Register sp; // init to 0xFFFE
    BYTE memory[MEMORY];
}GB;

void GB_init(GB *gb);

// TODO: Implement region locking
//  0000-3FFF 16KB ROM Bank 00 (in cartridge, fixed at bank 00)
//  4000-7FFF 16KB ROM Bank 01..NN (in cartridge, switchable bank number)
//  8000-9FFF 8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
//  A000-BFFF 8KB External RAM (in cartridge, switchable bank, if any)
//  C000-CFFF 4KB Work RAM Bank 0 (WRAM)
//  D000-DFFF 4KB Work RAM Bank 1 (WRAM) (switchable bank 1-7 in CGB Mode)
//  E000-FDFF Same as C000-DDFF (ECHO) (typically not used)
//  FE00-FE9F Sprite Attribute Table (OAM)
//  FEA0-FEFF Not Usable
//  FF00-FF7F I/O Ports
//  FF80-FFFE High RAM (HRAM)
//  FFFF Interrupt Enable Register
int GB_write(GB *gb, WORD address, BYTE value)
{
    address &= 0xFFFF;
    // Writing to RAM
    if(address < 0x8000)
    {
        return 1;
    }
    // Writing to Echo RAM
    else if(address >= 0xE000 && address < 0xFE00)
    {
        gb->memory[address] = value;
        return(GB_write(gb, address - 0x2000, value));
    }
    // Writing to restricted memory
    else if(address >= 0xFEA0 && address < 0xFEFF)
    {
        return 1;
    }
    gb->memory[address] = value;
    return 0;
}

BYTE GB_read(GB *gb, WORD add)
{
    add &= 0xFFFF;
    return gb->memory[add];
}