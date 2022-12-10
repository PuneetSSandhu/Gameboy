// A gameboy emulator written in C++.

#define MEMORY 0x10000
#define CART_SIZE 0x200000
#define RAM_BANKS 0x8000
#define ROM_BANK_SIZE 0x4000
#define RAM_BANK_SIZE 0x2000
#define HEIGHT 144
#define WIDTH 160
#define MAX_SPRITES 40
#define MAX_SPRITES_PER_LINE 10
#define CLOCK_SPEED 4194304
#define REFRESH_RATE 60

#define DIVIDER 0xFF04 // Timer Counter
#define TIMA 0xFF05 // Timer Counter
#define TMA 0xFF06 // Timer Modulator
#define TMC 0xFF07 // Timer Control

#define IF 0xFF0F // Interrupt Flag

typedef unsigned char BYTE;
typedef char SIGNED_BYTE;
typedef unsigned short WORD;
typedef signed short SIGNED_WORD;

// Flag values
#define FLAG_Z 7
#define FLAG_N 6
#define FLAG_H 5
#define FLAG_C 4

// MBCTypes
#define MBC_NONE 0
#define MBC1 1
#define MBC2 2

typedef union Register
{
    struct
    {
        BYTE low;
        BYTE high;
    };
    WORD word;
} Register;

typedef struct GameRom
{
    int MBC;
    bool RAMEnable;
    bool ROMBanking;
    BYTE curRomBank;
    BYTE ramBanks[RAM_BANKS]; // store all ram banks in game since there is a max of 4
    BYTE curRamBank;
    BYTE gameROM[CART_SIZE];
} Game;

typedef struct GameBoy
{
    BYTE screen[HEIGHT][WIDTH][3];
    Register AF;
    Register BC;
    Register DE;
    Register HL;
    WORD pc;     // init to 0x100
    Register sp; // init to 0xFFFE
    BYTE memory[MEMORY];
    Game *game;
} GB;

void GB_init(GB *gb);
BYTE GB_read(GB *gb, WORD address);
void GB_write(GB *gb, WORD address, BYTE value);
void GB_load(GB *gb, const char *filename);
void GB_close(GB *gb);
int GB_step(GB *gb);
void GB_updateTimers(GB *gb, int cycles);
void GB_updateGraphics(GB *gb, int cycles);
void GB_updateInput(GB *gb);
void GB_render(GB *gb);