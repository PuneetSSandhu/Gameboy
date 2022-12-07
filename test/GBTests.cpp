#include <gtest/gtest.h>
#include "../GB.cpp"

struct WriteTests : public ::testing::Test {
    GB gb;
    WORD address;
    BYTE value;
    virtual void SetUp() override {
        GB_init(&gb);
    }
    virtual void TearDown() override {
        // Nothing to do here
    }
};

struct ReadTests : public ::testing::Test {
    GB gb;
    WORD address;
    virtual void SetUp() override {
        GB_init(&gb);
    }
    virtual void TearDown() override {
        // Nothing to do here
    }
};

TEST_F(WriteTests, GB_write) {
    address = 0x1000; // ROM Address
    value = 0x10;
    EXPECT_EQ(GB_write(&gb, address, value), 1);

    address = 0x0000; // ROM Address
    value = 0x10;
    EXPECT_EQ(GB_write(&gb, address, value), 1);

    address = 0x8000; // VRAM Address
    value = 0x10;
    EXPECT_EQ(GB_write(&gb, address, value), 0);

    address = 0xFFFF; // Flag register
    value = 0xFF;
    EXPECT_EQ(GB_write(&gb, address, value), 0);
    EXPECT_EQ(gb.memory[address], value);

    address = 0xFEA1; // Not useable
    value = 0x10;
    EXPECT_EQ(GB_write(&gb, address, value), 1);

    address = 0xFFFFF; // Out of bounds should get caught and wrapped
    value = 0x10;
    EXPECT_EQ(GB_write(&gb, address, value), 0);

    address = -0x0001; // Out of bounds should get caught and wrapped 
    value = 0x10;
    EXPECT_EQ(GB_write(&gb, address, value), 0);
    EXPECT_EQ(gb.memory[address & 0xFFFF], value);
}

TEST_F(ReadTests, GB_read) {
    address = 0xFFFF; // Flag register
    EXPECT_EQ(GB_read(&gb, address), 0);

    GB_write(&gb, address, 0x10);
    EXPECT_EQ(GB_read(&gb, address), 0x10);

    address = 0xFFFFF; // Out of bounds should get caught and wrapped
    EXPECT_EQ(GB_read(&gb, address), 0x10);

    address = -0x0001; // Out of bounds should get caught and wrapped
    EXPECT_EQ(GB_read(&gb, address), 0x10);
}

