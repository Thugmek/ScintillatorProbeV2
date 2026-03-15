#ifndef COBS_H
#define COBS_H

#include <cstdint>
#include <cstddef>

namespace cobs {

// Encode src into dst. dst must be at least src_len + 2 bytes.
// Returns number of bytes written to dst.
size_t encode(const uint8_t *src, size_t src_len, uint8_t *dst);

// Decode src into dst. dst must be at least src_len bytes.
// Returns number of bytes written to dst, or 0 on error.
size_t decode(const uint8_t *src, size_t src_len, uint8_t *dst);

} // namespace cobs

#endif /* COBS_H */
