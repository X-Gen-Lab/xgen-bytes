/**
 * \file            test_bytes.cpp
 * \brief           Integer byte order, alignment and boundary contracts
 * \author          X-Gen Lab
 */

#include <xgen/bytes/bytes.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <gtest/gtest.h>
#include <string>

namespace {

struct ByteCodec {
    const char* name;
    size_t width;
    uint64_t value;
    std::array<uint8_t, 8> encoded;
    void (*write)(uint8_t*, uint64_t);
    uint64_t (*read)(const uint8_t*);
};

const std::array<ByteCodec, 6> codecs = {{
    {"U16LittleEndian",
     2,
     UINT64_C(0x0102),
     {2, 1, 0, 0, 0, 0, 0, 0},
     [](uint8_t* buffer, uint64_t value) {
         xgb_serialize_u16_le(buffer, static_cast<uint16_t>(value));
     },
     [](const uint8_t* buffer) -> uint64_t {
         return xgb_deserialize_u16_le(buffer);
     }},
    {"U16BigEndian",
     2,
     UINT64_C(0x0102),
     {1, 2, 0, 0, 0, 0, 0, 0},
     [](uint8_t* buffer, uint64_t value) {
         xgb_serialize_u16_be(buffer, static_cast<uint16_t>(value));
     },
     [](const uint8_t* buffer) -> uint64_t {
         return xgb_deserialize_u16_be(buffer);
     }},
    {"U32LittleEndian",
     4,
     UINT64_C(0x01020304),
     {4, 3, 2, 1, 0, 0, 0, 0},
     [](uint8_t* buffer, uint64_t value) {
         xgb_serialize_u32_le(buffer, static_cast<uint32_t>(value));
     },
     [](const uint8_t* buffer) -> uint64_t {
         return xgb_deserialize_u32_le(buffer);
     }},
    {"U32BigEndian",
     4,
     UINT64_C(0x01020304),
     {1, 2, 3, 4, 0, 0, 0, 0},
     [](uint8_t* buffer, uint64_t value) {
         xgb_serialize_u32_be(buffer, static_cast<uint32_t>(value));
     },
     [](const uint8_t* buffer) -> uint64_t {
         return xgb_deserialize_u32_be(buffer);
     }},
    {"U64LittleEndian",
     8,
     UINT64_C(0x0102030405060708),
     {8, 7, 6, 5, 4, 3, 2, 1},
     xgb_serialize_u64_le,
     xgb_deserialize_u64_le},
    {"U64BigEndian",
     8,
     UINT64_C(0x0102030405060708),
     {1, 2, 3, 4, 5, 6, 7, 8},
     xgb_serialize_u64_be,
     xgb_deserialize_u64_be},
}};

class BytesTest : public ::testing::TestWithParam<ByteCodec> {};

TEST_P(BytesTest, WritesExactKnownBytesAtEveryAlignment) {
    const auto& codec = GetParam();
    for (size_t offset = 0; offset < 8; ++offset) {
        SCOPED_TRACE(offset);
        std::array<uint8_t, 16> buffer{};
        buffer.fill(0xa5U);
        auto expected = buffer;
        std::copy_n(codec.encoded.data(), codec.width,
                    expected.data() + offset);
        codec.write(buffer.data() + offset, codec.value);
        EXPECT_EQ(buffer, expected);
    }
}

TEST_P(BytesTest, ReadsKnownBytesWithoutChangingInputAtEveryAlignment) {
    const auto& codec = GetParam();
    for (size_t offset = 0; offset < 8; ++offset) {
        SCOPED_TRACE(offset);
        std::array<uint8_t, 16> buffer{};
        buffer.fill(0x5aU);
        std::copy_n(codec.encoded.data(), codec.width, buffer.data() + offset);
        const auto original = buffer;
        EXPECT_EQ(codec.read(buffer.data() + offset), codec.value);
        EXPECT_EQ(buffer, original);
    }
}

TEST_P(BytesTest, NullReadReturnsZeroAndNullWriteCompletes) {
    const auto& codec = GetParam();
    codec.write(nullptr, codec.value);
    EXPECT_EQ(codec.read(nullptr), UINT64_C(0));
}

TEST_P(BytesTest, PreservesZeroMaximumAndEveryIndividualBit) {
    const auto& codec = GetParam();
    std::array<uint8_t, 10> buffer{};
    buffer.fill(0xa5U);
    const uint64_t maximum = UINT64_MAX >> ((8U - codec.width) * 8U);
    for (const uint64_t value : {UINT64_C(0), maximum}) {
        codec.write(buffer.data() + 1, value);
        EXPECT_EQ(codec.read(buffer.data() + 1), value);
        for (size_t i = 0; i < codec.width; ++i) {
            EXPECT_EQ(buffer[i + 1], value == 0 ? 0U : 0xffU);
        }
    }
    for (size_t bit = 0; bit < codec.width * 8U; ++bit) {
        SCOPED_TRACE(bit);
        const uint64_t value = UINT64_C(1) << bit;
        codec.write(buffer.data() + 1, value);
        EXPECT_EQ(codec.read(buffer.data() + 1), value);
    }
    EXPECT_EQ(buffer[0], 0xa5U);
    for (size_t i = codec.width + 1; i < buffer.size(); ++i) {
        EXPECT_EQ(buffer[i], 0xa5U);
    }
}

TEST_P(BytesTest, RoundTripsOriginalSeededCorpusAndPreservesSentinels) {
    const auto& codec = GetParam();
    uint64_t value = UINT64_C(0x91e10da5c79e7b1d);
    const uint64_t maximum = UINT64_MAX >> ((8U - codec.width) * 8U);
    for (size_t iteration = 0; iteration < 4096; ++iteration) {
        SCOPED_TRACE(iteration);
        value ^= value << 13U;
        value ^= value >> 7U;
        value ^= value << 17U;
        std::array<uint8_t, 10> buffer{};
        buffer.fill(0x5aU);
        codec.write(buffer.data() + 1, value);
        ASSERT_EQ(codec.read(buffer.data() + 1), value & maximum);
        EXPECT_EQ(buffer[0], 0x5aU);
        for (size_t i = codec.width + 1; i < buffer.size(); ++i) {
            EXPECT_EQ(buffer[i], 0x5aU);
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllIntegerOrders, BytesTest, ::testing::ValuesIn(codecs),
    [](const ::testing::TestParamInfo<ByteCodec>& parameter) {
        return std::string(parameter.param.name);
    });

} /* namespace */
