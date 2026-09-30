/**
 * \file            bytes.c
 * \brief           Alignment-safe integer byte serialization
 * \author          X-Gen Lab
 */

/* Byte-wise serialization preserves the original alignment-safe behavior. */
#include <xgen/bytes/bytes.h>

#include <stddef.h>

void xgb_serialize_u16_le(uint8_t* buffer, uint16_t value) {
    if (buffer != NULL) {
        buffer[0] = (uint8_t)value;
        buffer[1] = (uint8_t)(value >> 8U);
    }
}

uint16_t xgb_deserialize_u16_le(const uint8_t* buffer) {
    if (buffer == NULL) {
        return 0U;
    }
    return (uint16_t)((uint16_t)buffer[0] | ((uint16_t)buffer[1] << 8U));
}

void xgb_serialize_u32_le(uint8_t* buffer, uint32_t value) {
    if (buffer != NULL) {
        buffer[0] = (uint8_t)value;
        buffer[1] = (uint8_t)(value >> 8U);
        buffer[2] = (uint8_t)(value >> 16U);
        buffer[3] = (uint8_t)(value >> 24U);
    }
}

uint32_t xgb_deserialize_u32_le(const uint8_t* buffer) {
    if (buffer == NULL) {
        return 0U;
    }
    return (uint32_t)buffer[0] | ((uint32_t)buffer[1] << 8U) |
           ((uint32_t)buffer[2] << 16U) | ((uint32_t)buffer[3] << 24U);
}

void xgb_serialize_u64_le(uint8_t* buffer, uint64_t value) {
    if (buffer != NULL) {
        xgb_serialize_u32_le(buffer, (uint32_t)value);
        xgb_serialize_u32_le(buffer + 4U, (uint32_t)(value >> 32U));
    }
}

uint64_t xgb_deserialize_u64_le(const uint8_t* buffer) {
    if (buffer == NULL) {
        return 0U;
    }
    return (uint64_t)xgb_deserialize_u32_le(buffer) |
           ((uint64_t)xgb_deserialize_u32_le(buffer + 4U) << 32U);
}

void xgb_serialize_u16_be(uint8_t* buffer, uint16_t value) {
    if (buffer != NULL) {
        buffer[0] = (uint8_t)(value >> 8U);
        buffer[1] = (uint8_t)value;
    }
}

uint16_t xgb_deserialize_u16_be(const uint8_t* buffer) {
    if (buffer == NULL) {
        return 0U;
    }
    return (uint16_t)(((uint16_t)buffer[0] << 8U) | (uint16_t)buffer[1]);
}

void xgb_serialize_u32_be(uint8_t* buffer, uint32_t value) {
    if (buffer != NULL) {
        buffer[0] = (uint8_t)(value >> 24U);
        buffer[1] = (uint8_t)(value >> 16U);
        buffer[2] = (uint8_t)(value >> 8U);
        buffer[3] = (uint8_t)value;
    }
}

uint32_t xgb_deserialize_u32_be(const uint8_t* buffer) {
    if (buffer == NULL) {
        return 0U;
    }
    return ((uint32_t)buffer[0] << 24U) | ((uint32_t)buffer[1] << 16U) |
           ((uint32_t)buffer[2] << 8U) | (uint32_t)buffer[3];
}

void xgb_serialize_u64_be(uint8_t* buffer, uint64_t value) {
    if (buffer != NULL) {
        xgb_serialize_u32_be(buffer, (uint32_t)(value >> 32U));
        xgb_serialize_u32_be(buffer + 4U, (uint32_t)value);
    }
}

uint64_t xgb_deserialize_u64_be(const uint8_t* buffer) {
    if (buffer == NULL) {
        return 0U;
    }
    return ((uint64_t)xgb_deserialize_u32_be(buffer) << 32U) |
           (uint64_t)xgb_deserialize_u32_be(buffer + 4U);
}
