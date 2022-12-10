#include <gtest/gtest.h>
#include "../GB.cpp"

struct MBCTests : public ::testing::Test
{
    GB gb;
    WORD address;
    BYTE value;
    virtual void SetUp() override
    {
        GB_init(&gb);
    }
    virtual void TearDown() override
    {
        // Nothing to do here
    }
    void LoadNoMBC()
    {
        GB_load(&gb, "../roms/Tetris.gb");
    }
    void LoadMBC1()
    {
        GB_load(&gb, "../roms/supermarioland.gb");
    }
    void LoadMBC2()
    {
        GB_load(&gb, "../roms/Kirby'sPinballLand.gb");
    }
};

struct ReadTests : public ::testing::Test
{
    GB gb;
    WORD address;
    virtual void SetUp() override
    {
        GB_init(&gb);
    }
    virtual void TearDown() override
    {
        // Nothing to do here
    }
};

TEST_F(MBCTests, GB_write)
{
    LoadNoMBC();
    // Check MBC type
    EXPECT_EQ(gb.game->MBC, MBC_NONE);

    // Check RAM Enable
    address = 0x0000;
    value = 0x0A;
    GB_write(&gb, address, value);
    EXPECT_FALSE(gb.game->RAMEnable);

    // Check RAM Disable
    address = 0x0000;
    value = 0x00;
    GB_write(&gb, address, value);
    EXPECT_FALSE(gb.game->RAMEnable);

    // Check ROM Bank Change
    address = 0x2000;
    value = 0x02;
    GB_write(&gb, address, value);
    EXPECT_EQ(gb.game->curRomBank, 0x01);

    // Check RAM Bank Change
    address = 0x4000;
    value = 0x02;
    GB_write(&gb, address, value);
    EXPECT_EQ(gb.game->curRamBank, 0x0);

    LoadMBC1();
    EXPECT_EQ(gb.game->MBC, MBC1);

    // Check RAM Enable
    address = 0x0000;
    value = 0x0A;
    GB_write(&gb, address, value);
    EXPECT_TRUE(gb.game->RAMEnable);

    // Check RAM Disable
    address = 0x0000;
    value = 0x00;
    GB_write(&gb, address, value);
    EXPECT_FALSE(gb.game->RAMEnable);

    // Check ROM Bank Change
    address = 0x2000;
    value = 0x02;
    GB_write(&gb, address, value);
    EXPECT_EQ(gb.game->curRomBank, 0x02);

    // Check RAM Bank Change
    address = 0x4000;
    value = 0x02;
    GB_write(&gb, address, value);
    EXPECT_EQ(gb.game->curRamBank, 0x2);

    // Enable ROM Banking
    address = 0x6000;
    value = 0x00; // 0x00 = ROM Banking
    GB_write(&gb, address, value);
    EXPECT_TRUE(gb.game->ROMBanking);

    // Enable RAM Banking
    address = 0x6000;
    value = 0x01; // 0x01 = RAM Banking
    GB_write(&gb, address, value);
    EXPECT_FALSE(gb.game->ROMBanking);

    LoadMBC2();
    EXPECT_EQ(gb.game->MBC, MBC2);

    // Check RAM Enable
    address = 0x0000;
    value = 0x0A;
    GB_write(&gb, address, value);
    EXPECT_TRUE(gb.game->RAMEnable);

    // Check RAM Disable
    address = 0x0000;
    value = 0x00;
    GB_write(&gb, address, value);
    EXPECT_FALSE(gb.game->RAMEnable);

    // Check ROM Bank Change
    address = 0x2000;
    value = 0x02;
    GB_write(&gb, address, value);
    EXPECT_EQ(gb.game->curRomBank, 0x02);

    // Check RAM Bank Change
    address = 0x4000;
    value = 0x02;
    GB_write(&gb, address, value);
    EXPECT_EQ(gb.game->curRamBank, 0x0);

    // Enable ROM Banking
    address = 0x6000;
    value = 0x00; // 0x00 = ROM Banking
    GB_write(&gb, address, value);
    EXPECT_FALSE(gb.game->ROMBanking);

    // Enable RAM Banking
    address = 0x6000;
    value = 0x01; // 0x01 = RAM Banking
    GB_write(&gb, address, value);
    EXPECT_FALSE(gb.game->ROMBanking);

    GB_close(&gb);
}

TEST_F(ReadTests, GB_read)
{
    address = 0xFFFF; // Flag register
    EXPECT_EQ(GB_read(&gb, address), 0);

    GB_write(&gb, address, 0x10);
    EXPECT_EQ(GB_read(&gb, address), 0x10);

    address = 0xFFFFF; // Out of bounds should get caught and wrapped
    EXPECT_EQ(GB_read(&gb, address), 0x10);

    address = -0x0001; // Out of bounds should get caught and wrapped
    EXPECT_EQ(GB_read(&gb, address), 0x10);
}
