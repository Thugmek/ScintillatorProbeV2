#pragma once

#include <cstdint>

namespace logging {

enum class Severity : uint8_t {
    debug = 1,
    info = 2,
    warning = 3,
    error = 4,
    critical = 5,
};

struct Component {
    const char *name;
    Severity lowest_severity;
};

void init();

void __attribute__((format(__printf__, 3, 4)))
_log_event(Severity severity, const Component *component, const char *fmt, ...);

#ifndef LOG_LOWEST_SEVERITY
  #ifdef _DEBUG
    #define LOG_LOWEST_SEVERITY 1 // DEBUG
  #else
    #define LOG_LOWEST_SEVERITY 2 // INFO
  #endif
#endif

#define _LOG_COMPONENT_ATTRS __attribute__((used, section(".data.log_components")))

#define LOG_COMPONENT(name) __log_component_##name

#define LOG_COMPONENT_DEF(name, default_severity) \
    logging::Component LOG_COMPONENT(name) _LOG_COMPONENT_ATTRS = { #name, default_severity }

#define LOG_COMPONENT_REF(component) extern logging::Component LOG_COMPONENT(component)

#define log_event(severity, component, fmt, ...)                                \
    do {                                                                        \
        _log_event(severity, &__log_component_##component, fmt, ##__VA_ARGS__); \
    } while (0)

#if LOG_LOWEST_SEVERITY <= 1
  #define log_debug(component, fmt, ...) log_event(logging::Severity::debug, component, fmt, ##__VA_ARGS__)
#else
  #define log_debug(component, fmt, ...)
#endif

#if LOG_LOWEST_SEVERITY <= 2
  #define log_info(component, fmt, ...) log_event(logging::Severity::info, component, fmt, ##__VA_ARGS__)
#else
  #define log_info(component, fmt, ...)
#endif

#if LOG_LOWEST_SEVERITY <= 3
  #define log_warning(component, fmt, ...) log_event(logging::Severity::warning, component, fmt, ##__VA_ARGS__)
#else
  #define log_warning(component, fmt, ...)
#endif

#if LOG_LOWEST_SEVERITY <= 4
  #define log_error(component, fmt, ...) log_event(logging::Severity::error, component, fmt, ##__VA_ARGS__)
#else
  #define log_error(component, fmt, ...)
#endif

#if LOG_LOWEST_SEVERITY <= 5
  #define log_critical(component, fmt, ...) log_event(logging::Severity::critical, component, fmt, ##__VA_ARGS__)
#else
  #define log_critical(component, fmt, ...)
#endif

} // namespace logging
