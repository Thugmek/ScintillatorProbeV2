#include <cstdint>
#include "hal.hpp"
#include "logging/log.hpp"

LOG_COMPONENT_DEF(Main, logging::Severity::debug);

int main() {
    hal::init();
    log_info(Main, "Firmware started");

    while (true) {
        // Main loop - halt here for debugger
        __asm volatile("nop");
    }
}
