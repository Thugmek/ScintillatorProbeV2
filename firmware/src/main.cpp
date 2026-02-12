#include <cstdint>

static volatile int32_t a;
static volatile int32_t b;
static volatile int32_t c;

int main() {
    a = 5;
    b = 6;
    c = a + b;

    while (true) {
        // Main loop - halt here for debugger
        __asm volatile("nop");
    }
}
