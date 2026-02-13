#include <cstdint>
#include "hal.hpp"

static volatile int32_t a;
static volatile int32_t b;
static volatile int32_t c;

int main() {
    hal::init();

    while (true) {
        // Main loop - halt here for debugger
        __asm volatile("nop");
    }
}
