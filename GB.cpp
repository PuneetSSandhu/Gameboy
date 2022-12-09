#include <iostream>
#include <string.h>
#include "GB.hpp"

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
    gb->game->curRamBank = 0;
    gb->game->RAMEnable = false;
    memset(gb->game->ramBanks, 0, RAM_BANKS);
}

int GB_write(GB *gb, WORD address, BYTE value)
{
    address &= 0xFFFF;
    // External RAM Enable
    if (address < 0x1FFF)
    {   
        // check 4th bit for MBC2
        if(address & 0x0008){
          return 0;  
        }

        if (value & 0x0F == 0x0A)
        {
            gb->game->RAMEnable = true;
        }
        else
        {
            gb->game->RAMEnable = false;
        }
        return 0;
    }
    // ROM Bank Number
    else if (address >= 0x2000 && address < 0x3FFF)
    {
        return 0;
    }
    // Writing to ROM
    else if (address < 0x8000)
    {
        return 1;
    }
    // Writing to Echo RAM
    else if (gb->game->RAMEnable && address >= 0xE000 && address < 0xFE00)
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
    // Reading from ROM Banks
    if (add >= 0x4000 && add < 0x8000)
    {
        return gb->game->gameROM[(add - 0x4000) + (gb->game->curRomBank * ROM_BANK_SIZE)];
    }
    // Reading from RAM Banks
    else if (add >= 0xA000 && add < 0xC000)
    {
        return gb->game->ramBanks[(add - 0xA000) + (gb->game->curRamBank * RAM_BANK_SIZE)];
    }
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

    fclose(rom);

    // Load the game into memory
    int size = get_file_size(rom);
    gb->game->gameROM = (BYTE *)malloc(size);
    fread(gb->game->gameROM, 1, size, rom);

    // Copy the Game RAM into ramBanks

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
    default:
        break;
    }
}

void GB_close(GB *gb)
{
    // Free the memory if it was allocated
    if (gb->game->MBC1 || gb->game->MBC2)
    {
        free(gb->game);
    }
}