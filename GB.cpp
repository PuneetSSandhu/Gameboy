#include <iostream>
#include "GB.hpp"
#include <string.h>

void GB_init(GB *gb)
{
    // Initialize the registers
    gb->AF.word = 0x01B0;
    gb->BC.word = 0x0013;
    gb->DE.word = 0x00D8;
    gb->HL.word = 0x014D;
    gb->sp.word = 0xFFFE;
    gb->pc = 0x0100;

    // Initialize the memory
    memset(gb->memory, 0, 0x10000);
    gb->memory[0xFF05] = 0x00;
    gb->memory[0xFF06] = 0x00;
    gb->memory[0xFF07] = 0x00;
    gb->memory[0xFF10] = 0x80;
    gb->memory[0xFF11] = 0xBF;
    gb->memory[0xFF12] = 0xF3;
    gb->memory[0xFF14] = 0xBF;
    gb->memory[0xFF16] = 0x3F;
    gb->memory[0xFF17] = 0x00;
    gb->memory[0xFF19] = 0xBF;
    gb->memory[0xFF1A] = 0x7F;
    gb->memory[0xFF1B] = 0xFF;
    gb->memory[0xFF1C] = 0x9F;
    gb->memory[0xFF1E] = 0xBF;
    gb->memory[0xFF20] = 0xFF;
    gb->memory[0xFF21] = 0x00;
    gb->memory[0xFF22] = 0x00;
    gb->memory[0xFF23] = 0xBF;
    gb->memory[0xFF24] = 0x77;
    gb->memory[0xFF25] = 0xF3;
    gb->memory[0xFF26] = 0xF1;
    gb->memory[0xFF40] = 0x91;
    gb->memory[0xFF42] = 0x00;
    gb->memory[0xFF43] = 0x00;
    gb->memory[0xFF45] = 0x00;
    gb->memory[0xFF47] = 0xFC;
    gb->memory[0xFF48] = 0xFF;
    gb->memory[0xFF49] = 0xFF;
    gb->memory[0xFF4A] = 0x00;
    gb->memory[0xFF4B] = 0x00;
    gb->memory[0xFFFF] = 0x00;

    // Initialize the game struct
    gb->game = new Game;
    gb->game->MBC1 = false;
    gb->game->MBC2 = false;
    gb->game->curRomBank = 1;
}

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
    if (address < 0x8000)
    {
        return 1;
    }
    // Writing to Echo RAM
    else if (address >= 0xE000 && address < 0xFE00)
    {
        gb->memory[address] = value;
        return (GB_write(gb, address - 0x2000, value));
    }
    // Writing to restricted memory
    else if (address >= 0xFEA0 && address < 0xFEFF)
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

int get_file_size(FILE *file)
{
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}

void GB_load(GB *gb, const char *filename)
{
    FILE *rom = fopen(filename, "rb");
    if (rom == NULL)
    {
        printf("Error: Could not open file %s", filename);
        exit(1);
    }

    fread(gb->memory + 0x100, 1, 0x3FFF, rom);
    fclose(rom);

    // Check for MBC
    switch (gb->memory[0x147])
    {
    case 0x01:
    case 0x02:
    case 0x03:
        gb->game->MBC1 = true;
        break;
    case 0x05:
    case 0x06:
        gb->game->MBC2 = true;
        break;
    }
}
