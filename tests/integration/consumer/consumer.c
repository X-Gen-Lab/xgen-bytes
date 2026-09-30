/**
 * \file            consumer.c
 * \brief           Standalone public API and link consumption
 */

#include <xgen/bytes/bytes.h>
#include <xgen/bytes/version.h>

int main(void) {
    uint8_t bytes[9] = {0};
    xgb_serialize_u64_be(bytes + 1, UINT64_C(0x0102030405060708));
    if (bytes[1] != 1U || bytes[8] != 8U ||
        xgb_deserialize_u64_be(bytes + 1) != UINT64_C(0x0102030405060708)) {
        return 1;
    }
    xgb_serialize_u64_le(bytes + 1, UINT64_C(0x0102030405060708));
    if (bytes[1] != 8U || bytes[8] != 1U ||
        xgb_deserialize_u64_le(bytes + 1) != UINT64_C(0x0102030405060708)) {
        return 2;
    }
    return XGB_VERSION_MAJOR == 0 && XGB_ABI_VERSION == 1 ? 0 : 3;
}
