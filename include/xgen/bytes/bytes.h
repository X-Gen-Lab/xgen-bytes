/**
 * \file            bytes.h
 * \brief           Alignment-safe unsigned integer byte serialization
 * \author          X-Gen Lab
 * \details         Buffers remain caller-owned and are borrowed only until
 *                  the call returns. No pointers are retained, no heap is
 *                  used and no initialization is required. Calls are
 *                  synchronous, nonblocking and perform fixed 2/4/8-byte work.
 *                  Arbitrarily aligned byte buffers are supported. The caller
 *                  must prevent concurrent conflicting access to each buffer;
 *                  independent buffers are reentrant. No locking or atomic
 *                  multi-byte access is provided. ISR use requires the caller
 *                  to satisfy its execution budget and buffer ownership rules.
 *                  These primitives do not validate enclosing message lengths
 *                  or define a protocol. Non-NULL buffers must have the stated
 *                  readable or writable capacity before the call.
 */

#ifndef XGB_BYTES_H
#define XGB_BYTES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Write a 16-bit little-endian unsigned integer.
 * \param[out]      buffer: Caller-owned destination with at least 2
 *                  writable bytes, or NULL. No alignment is required.
 * \param[in]       value: Unsigned 16-bit integer to encode.
 * \note            NULL is a no-op. Otherwise exactly 2 bytes are written;
 *                  all surrounding bytes remain unchanged. The write is not
 *                  atomic. The shared ownership and context contract above
 *                  applies, and no length check is performed.
 */
void xgb_serialize_u16_le(uint8_t* buffer, uint16_t value);

/**
 * \brief           Read a 16-bit little-endian unsigned integer.
 * \param[in]       buffer: Caller-owned input with at least 2 readable
 *                  bytes, or NULL. No alignment is required.
 * \return          Decoded unsigned integer, or zero when buffer is NULL.
 * \note            Exactly 2 bytes are read for non-NULL input. The input
 *                  is unchanged. The shared ownership and context contract
 *                  above applies, and no length check is performed.
 */
uint16_t xgb_deserialize_u16_le(const uint8_t* buffer);

/**
 * \brief           Write a 32-bit little-endian unsigned integer.
 * \param[out]      buffer: Caller-owned destination with at least 4
 *                  writable bytes, or NULL. No alignment is required.
 * \param[in]       value: Unsigned 32-bit integer to encode.
 * \note            NULL is a no-op. Otherwise exactly 4 bytes are written;
 *                  all surrounding bytes remain unchanged. The write is not
 *                  atomic. The shared ownership and context contract above
 *                  applies, and no length check is performed.
 */
void xgb_serialize_u32_le(uint8_t* buffer, uint32_t value);

/**
 * \brief           Read a 32-bit little-endian unsigned integer.
 * \param[in]       buffer: Caller-owned input with at least 4 readable
 *                  bytes, or NULL. No alignment is required.
 * \return          Decoded unsigned integer, or zero when buffer is NULL.
 * \note            Exactly 4 bytes are read for non-NULL input. The input
 *                  is unchanged. The shared ownership and context contract
 *                  above applies, and no length check is performed.
 */
uint32_t xgb_deserialize_u32_le(const uint8_t* buffer);

/**
 * \brief           Write a 64-bit little-endian unsigned integer.
 * \param[out]      buffer: Caller-owned destination with at least 8
 *                  writable bytes, or NULL. No alignment is required.
 * \param[in]       value: Unsigned 64-bit integer to encode.
 * \note            NULL is a no-op. Otherwise exactly 8 bytes are written;
 *                  all surrounding bytes remain unchanged. The write is not
 *                  atomic. The shared ownership and context contract above
 *                  applies, and no length check is performed.
 */
void xgb_serialize_u64_le(uint8_t* buffer, uint64_t value);

/**
 * \brief           Read a 64-bit little-endian unsigned integer.
 * \param[in]       buffer: Caller-owned input with at least 8 readable
 *                  bytes, or NULL. No alignment is required.
 * \return          Decoded unsigned integer, or zero when buffer is NULL.
 * \note            Exactly 8 bytes are read for non-NULL input. The input
 *                  is unchanged. The shared ownership and context contract
 *                  above applies, and no length check is performed.
 */
uint64_t xgb_deserialize_u64_le(const uint8_t* buffer);

/**
 * \brief           Write a 16-bit big-endian unsigned integer.
 * \param[out]      buffer: Caller-owned destination with at least 2
 *                  writable bytes, or NULL. No alignment is required.
 * \param[in]       value: Unsigned 16-bit integer to encode.
 * \note            NULL is a no-op. Otherwise exactly 2 bytes are written;
 *                  all surrounding bytes remain unchanged. The write is not
 *                  atomic. The shared ownership and context contract above
 *                  applies, and no length check is performed.
 */
void xgb_serialize_u16_be(uint8_t* buffer, uint16_t value);

/**
 * \brief           Read a 16-bit big-endian unsigned integer.
 * \param[in]       buffer: Caller-owned input with at least 2 readable
 *                  bytes, or NULL. No alignment is required.
 * \return          Decoded unsigned integer, or zero when buffer is NULL.
 * \note            Exactly 2 bytes are read for non-NULL input. The input
 *                  is unchanged. The shared ownership and context contract
 *                  above applies, and no length check is performed.
 */
uint16_t xgb_deserialize_u16_be(const uint8_t* buffer);

/**
 * \brief           Write a 32-bit big-endian unsigned integer.
 * \param[out]      buffer: Caller-owned destination with at least 4
 *                  writable bytes, or NULL. No alignment is required.
 * \param[in]       value: Unsigned 32-bit integer to encode.
 * \note            NULL is a no-op. Otherwise exactly 4 bytes are written;
 *                  all surrounding bytes remain unchanged. The write is not
 *                  atomic. The shared ownership and context contract above
 *                  applies, and no length check is performed.
 */
void xgb_serialize_u32_be(uint8_t* buffer, uint32_t value);

/**
 * \brief           Read a 32-bit big-endian unsigned integer.
 * \param[in]       buffer: Caller-owned input with at least 4 readable
 *                  bytes, or NULL. No alignment is required.
 * \return          Decoded unsigned integer, or zero when buffer is NULL.
 * \note            Exactly 4 bytes are read for non-NULL input. The input
 *                  is unchanged. The shared ownership and context contract
 *                  above applies, and no length check is performed.
 */
uint32_t xgb_deserialize_u32_be(const uint8_t* buffer);

/**
 * \brief           Write a 64-bit big-endian unsigned integer.
 * \param[out]      buffer: Caller-owned destination with at least 8
 *                  writable bytes, or NULL. No alignment is required.
 * \param[in]       value: Unsigned 64-bit integer to encode.
 * \note            NULL is a no-op. Otherwise exactly 8 bytes are written;
 *                  all surrounding bytes remain unchanged. The write is not
 *                  atomic. The shared ownership and context contract above
 *                  applies, and no length check is performed.
 */
void xgb_serialize_u64_be(uint8_t* buffer, uint64_t value);

/**
 * \brief           Read a 64-bit big-endian unsigned integer.
 * \param[in]       buffer: Caller-owned input with at least 8 readable
 *                  bytes, or NULL. No alignment is required.
 * \return          Decoded unsigned integer, or zero when buffer is NULL.
 * \note            Exactly 8 bytes are read for non-NULL input. The input
 *                  is unchanged. The shared ownership and context contract
 *                  above applies, and no length check is performed.
 */
uint64_t xgb_deserialize_u64_be(const uint8_t* buffer);

#ifdef __cplusplus
}
#endif

#endif
