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

typedef struct GameRom{
    bool MBC1;
    bool MBC2;
    BYTE curRomBank;
    BYTE romBanks;
}Game;

typedef struct GameBoy{
    BYTE screen [HEIGHT][WIDTH][3];
    Register AF;
    Register BC;
    Register DE;
    Register HL;

    WORD pc; // init to 0x100
    Register sp; // init to 0xFFFE
    BYTE memory[MEMORY];
    Game * game;
}GB;


void GB_init(GB *gb);
BYTE GB_read(GB *gb, WORD address);
int GB_write(GB *gb, WORD address, BYTE value);
void GB_load(GB *gb, const char *filename);