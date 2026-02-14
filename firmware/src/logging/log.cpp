#include "logging/log.hpp"

#include <cstdarg>
#include <cstdio>
#include "SEGGER_RTT.h"
#include "stm32h7xx_hal.h"

namespace logging {

static bool initialized = false;

static const char *severity_str(Severity severity) {
    switch (severity) {
    case Severity::debug:    return "DEBUG";
    case Severity::info:     return "INFO ";
    case Severity::warning:  return "WARN ";
    case Severity::error:    return "ERROR";
    case Severity::critical: return "CRITI";
    default:                 return "?????";
    }
}

void init() {
    SEGGER_RTT_Init();
    initialized = true;
}

void _log_event(Severity severity, const Component *component, const char *fmt, ...) {
    if (!initialized) {
        init();
    }

    if (severity < component->lowest_severity) {
        return;
    }

    // Format: "1.234s [INFO  HAL] message\r\n"
    uint32_t tick = HAL_GetTick();
    uint32_t sec = tick / 1000;
    uint32_t ms = tick % 1000;

    char buf[256];
    int pos = snprintf(buf, sizeof(buf), "%lu.%03lus [%s %s] ",
        (unsigned long)sec, (unsigned long)ms,
        severity_str(severity), component->name);

    if (pos > 0 && static_cast<size_t>(pos) < sizeof(buf)) {
        va_list args;
        va_start(args, fmt);
        int msg_len = vsnprintf(buf + pos, sizeof(buf) - pos, fmt, args);
        va_end(args);
        if (msg_len > 0) {
            pos += msg_len;
            if (static_cast<size_t>(pos) >= sizeof(buf)) {
                pos = sizeof(buf) - 1;
            }
        }
    }

    // Append \r\n
    if (static_cast<size_t>(pos) + 2 < sizeof(buf)) {
        buf[pos++] = '\r';
        buf[pos++] = '\n';
    }

    SEGGER_RTT_WriteNoLock(0, buf, pos);
}

} // namespace logging
