#include <iostream>
#include <string.h>
#include "GB.hpp"

int timerCounter;
int divCounter;
int scanlineCounter;

void GB_init(GB *gb)
{
    // Initialize the registers
    gb->AF = 0x01B0;
    gb->BC = 0x0013;
    gb->DE = 0x00D8;
    gb->HL = 0x014D;
    gb->sp = 0xFFFE;
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
    gb->game->MBC = 0;
    gb->game->curRomBank = 1;
    gb->game->curRamBank = 0;
    gb->game->RAMEnable = false;
    memset(gb->game->ramBanks, 0, RAM_BANKS);

    // Set frequency to 1024Hz
    gb->memory[TMC] = 0x04;
    timerCounter = 1024;
    divCounter = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GB_write(GB *gb, WORD address, BYTE value)
{

    // External RAM Enable
    if (address < 0x1FFF)
    {
        if (gb->game->MBC)
        {
            // check 4th bit for MBC2
            if (gb->game->MBC == MBC2 && (address & 0x0008))
            {
                return;
            }

            if ((value & 0xF) == 0xA)
            {
                gb->game->RAMEnable = true;
            }
            else
            {
                gb->game->RAMEnable = false;
            }
        }
        return;
    }
    // ROM Bank Number
    else if (address >= 0x2000 && address < 0x4000)
    {
        // Do ROM Bank Change
        if (gb->game->MBC == MBC2)
        {
            gb->game->curRomBank = value & 0x0F;
            // If the rom bank is 0, set it to 1
            if (gb->game->curRomBank == 0)
            {
                gb->game->curRomBank++;
            }
            return;
        }
        else if (gb->game->MBC == MBC1)
        {
            // Do ROM Bank Change for MBC1
            BYTE lower5Bits = value & 0x1F;
            // keep the upper 3 bits of curRomBank
            gb->game->curRomBank &= 0xE0;
            // set the lower 5 bits of curRomBank
            gb->game->curRomBank |= lower5Bits;
            // If the rom bank is 0, set it to 1
            if (gb->game->curRomBank == 0)
            {
                gb->game->curRomBank++;
            }
        }
        // do nothing for no MBC
    }
    else if (address >= 0x4000 && address < 0x6000)
    {
        // Do RAM Bank Change for MBC1
        if (gb->game->MBC == MBC1)
        {
            if (gb->game->ROMBanking) // HiRomBanking
            {
                // Set the upper 3 bits off curRomBank
                gb->game->curRomBank &= 0x1F;

                // turn off the lower 5 bits of value
                value &= 0xE0;
                // set the upper 3 bits of curRomBank
                gb->game->curRomBank |= value;
                // If the rom bank is 0, set it to 1
                if (gb->game->curRomBank == 0)
                {
                    gb->game->curRomBank++;
                }
            }
            else // Ram bank change
            {
                gb->game->curRamBank = value & 0x03;
            }
        }
    }
    else if (address >= 6000 && address < 0x8000)
    {
        // Set ROM/RAM Banking Mode
        if (gb->game->MBC == MBC1)
        {
            if (value & 0x01)
            {
                gb->game->ROMBanking = false;
            }
            else
            {
                gb->game->ROMBanking = true;
                gb->game->curRamBank = 0;
            }
        }
    }
    // Writing to Echo RAM
    else if (gb->game->RAMEnable && address >= 0xE000 && address < 0xFE00)
    {
        gb->memory[address] = value;
        GB_write(gb, address - 0x2000, value);
        return;
    }
    // Writing to RAM
    else if (gb->game->RAMEnable && address >= 0xA000 && address < 0xC000)
    {
        gb->memory[address] = value;
        GB_write(gb, address + 0x2000, value);
        return;
    }
    // Writing to restricted memory
    else if (address >= 0xFEA0 && address < 0xFEFF)
    {
        return;
    }
    gb->memory[address] = value;
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GB_load(GB *gb, const char *filename)
{
    FILE *rom = fopen(filename, "rb");
    if (rom == NULL)
    {
        std::cout << "Error opening file " << filename << std::endl;
        exit(1);
    }

    fread(gb->game->gameROM, 1, CART_SIZE, rom);

    fclose(rom);

    std::cout << gb->game->gameROM[0x147] << std::endl;
    // Check for MBC
    switch (gb->game->gameROM[0x147])
    {
    case 0x01:
    case 0x02:
    case 0x03:
        gb->game->MBC = MBC1;
        break;
    case 0x05:
    case 0x06:
        gb->game->MBC = MBC2;
        break;
    default:
        gb->game->MBC = MBC_NONE;
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getFreq(GB *gb)
{
    int freq = GB_read(gb, TMC) & 0x03;
    switch (freq)
    {
    case 0:
        timerCounter = CLOCK_SPEED / 1024;
    case 1:
        timerCounter = CLOCK_SPEED / 16;
    case 2:
        timerCounter = CLOCK_SPEED / 64;
    case 3:
        timerCounter = CLOCK_SPEED / 256;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GB_updateTimers(GB *gb, int cycles)
{
    // update the divider counter
    divCounter += cycles;
    if (divCounter >= 256)
    {
        gb->memory[DIVIDER] += 1;
        divCounter = 0;
    }

    // update the timer counter
    timerCounter -= cycles;

    if (timerCounter <= 0)
    {
        getFreq(gb);
        if (GB_read(gb, TMC) & 0x04) // check if timer is enabled
        {
            if (GB_read(gb, TIMA) == 255)
            {
                GB_write(gb, TIMA, GB_read(gb, TMA));
                GB_write(gb, IF, GB_read(gb, IF) | 0x04); // Request interrupt for timer
            }
            else
            {
                GB_write(gb, TIMA, GB_read(gb, TIMA) + 1);
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 00: H-Blank
// 01: V-Blank
// 10: Searching Sprites Atts
// 11: Transferring Data to LCD Driver 
void LCD_status(GB * gb){
    // Get the current status
    BYTE status = GB_read(gb, 0xFF41);

    // Get LCD enable
    bool LCDenable = (GB_read(gb, 0xFF40) >> 7) & 0x01;

    // if the LCD is not enables 
    if(!LCDenable){
        
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GB_updateGraphics(GB * gb, int cycles){
    // Set the LCD status

    // Check if the LCD is enabled
    BYTE LCDEnable = GB_read(gb, 0xFF40);

    if((LCDEnable >> 7) & 0x01){
        scanlineCounter -= cycles;
    }
    else{
        return;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GB_updateInterrupt(GB *gb)
{
    // check IE
    BYTE Ienable = GB_read(gb, IE);
    // check if any interrupts are enabled
    if (Ienable == 0x00)
    {
        return;
    }
    // check IF
    BYTE Iflags = GB_read(gb, IF);

    // check if any interrupts are requested
    if (Iflags == 0x00)
    {
        return;
    }

    // check if any interrupts are enabled and requested
    if ((Ienable & Iflags) == 0x00)
    {
        return;
    }

    // VBlank Interrupt
    if ((Ienable & 0x01) & (Iflags & 0x01))
    {
        // disable the interrupt
        GB_write(gb, IE, Ienable & 0xFE);
        // disable the interrupt flag
        GB_write(gb, IF, Iflags & 0xFE);
        // push the current PC to the stack
        GB_write(gb, gb->sp - 1, gb->pc >> 8);
        GB_write(gb, gb->sp - 2, gb->pc & 0xFF);
        gb->sp -= 2;
        // set the PC to the VBlank interrupt vector
        gb->pc = 0x0040;
    }

    // STAT interrupt
    if ((Ienable & 0x02) & (Iflags & 0x02))
    {
        // disable the interrupt
        GB_write(gb, IE, Ienable & 0xFD);
        // disable the interrupt flag
        GB_write(gb, IF, Iflags & 0xFD);
        // push the current PC to the stack
        GB_write(gb, gb->sp - 1, gb->pc >> 8);
        GB_write(gb, gb->sp - 2, gb->pc & 0xFF);
        gb->sp -= 2;
        // set the PC to the STAT interrupt vector
        gb->pc = 0x0048;
    }

    // Timer interrupt
    if ((Ienable & 0x04) & (Iflags & 0x04))
    {
        // disable the interrupt
        GB_write(gb, IE, Ienable & 0xFB);
        // disable the interrupt flag
        GB_write(gb, IF, Iflags & 0xFB);
        // push the current PC to the stack
        GB_write(gb, gb->sp - 1, gb->pc >> 8);
        GB_write(gb, gb->sp - 2, gb->pc & 0xFF);
        gb->sp -= 2;
        // set the PC to the Timer interrupt vector
        gb->pc = 0x0050;
    }
    // Serial interrupt
    if ((Ienable & 0x08) & (Iflags & 0x08))
    {
        // disable the interrupt
        GB_write(gb, IE, Ienable & 0xF7);
        // disable the interrupt flag
        GB_write(gb, IF, Iflags & 0xF7);
        // push the current PC to the stack
        GB_write(gb, gb->sp - 1, gb->pc >> 8);
        GB_write(gb, gb->sp - 2, gb->pc & 0xFF);
        gb->sp -= 2;
        // set the PC to the Serial interrupt vector
        gb->pc = 0x0058;
    }

    // Joypad interrupt
    if ((Ienable & 0x10) & (Iflags & 0x10))
    {
        // disable the interrupt
        GB_write(gb, IE, Ienable & 0xEF);
        // disable the interrupt flag
        GB_write(gb, IF, Iflags & 0xEF);
        // push the current PC to the stack
        GB_write(gb, gb->sp - 1, gb->pc >> 8);
        GB_write(gb, gb->sp - 2, gb->pc & 0xFF);
        gb->sp -= 2;
        // set the PC to the Joypad interrupt vector
        gb->pc = 0x0060;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO: Implement OPCODES
int GB_step(GB *gb)
{
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GB_close(GB *gb)
{
    // Free the memory if it was allocated
    if (gb->game->MBC)
    {
        free(gb->game);
    }
}