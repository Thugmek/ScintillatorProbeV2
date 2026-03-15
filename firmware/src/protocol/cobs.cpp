#include "protocol/cobs.h"

namespace cobs {

size_t encode(const uint8_t *src, size_t src_len, uint8_t *dst) {
    size_t read_idx = 0;
    size_t write_idx = 1;
    size_t code_idx = 0;
    uint8_t code = 1;

    while (read_idx < src_len) {
        if (src[read_idx] == 0x00) {
            dst[code_idx] = code;
            code = 1;
            code_idx = write_idx++;
        } else {
            dst[write_idx++] = src[read_idx];
            code++;
            if (code == 0xFF) {
                dst[code_idx] = code;
                code = 1;
                code_idx = write_idx++;
            }
        }
        read_idx++;
    }
    dst[code_idx] = code;
    return write_idx;
}

size_t decode(const uint8_t *src, size_t src_len, uint8_t *dst) {
    size_t read_idx = 0;
    size_t write_idx = 0;

    while (read_idx < src_len) {
        uint8_t code = src[read_idx++];
        if (code == 0) return 0; // invalid

        for (uint8_t i = 1; i < code; i++) {
            if (read_idx >= src_len) return 0;
            dst[write_idx++] = src[read_idx++];
        }

        // Insert implicit zero between groups, but not after the last group
        // and not after a 0xFF block (which doesn't imply a zero)
        if (code != 0xFF && read_idx < src_len) {
            dst[write_idx++] = 0x00;
        }
    }

    return write_idx;
}

} // namespace cobs
