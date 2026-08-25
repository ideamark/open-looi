#include "logger.h"
#include <stdarg.h>

// Default log level is INFO
uint8_t global_log_level = LOG_LEVEL_INFO;

// Set the global log level
void log_set_level(uint8_t level) {
    global_log_level = level;
}

// Internal function to print log prefix with level
void log_prefix(const char* level) {
    unsigned long ms = millis();
    unsigned long seconds = ms / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    Serial.printf("[%02lu:%02lu:%02lu.%03lu] [%s] ", 
        hours % 24, minutes % 60, seconds % 60, ms % 1000, level);
}

// Log functions implementation
void log_error(const char* format, ...) {
    if (global_log_level >= LOG_LEVEL_ERROR) {
        log_prefix("ERROR");
        va_list args;
        va_start(args, format);
        char buf[256];
        vsnprintf(buf, sizeof(buf), format, args);
        Serial.println(buf);
        va_end(args);
    }
}

void log_warn(const char* format, ...) {
    if (global_log_level >= LOG_LEVEL_WARN) {
        log_prefix("WARN ");
        va_list args;
        va_start(args, format);
        char buf[256];
        vsnprintf(buf, sizeof(buf), format, args);
        Serial.println(buf);
        va_end(args);
    }
}

void log_info(const char* format, ...) {
    if (global_log_level >= LOG_LEVEL_INFO) {
        log_prefix("INFO ");
        va_list args;
        va_start(args, format);
        char buf[256];
        vsnprintf(buf, sizeof(buf), format, args);
        Serial.println(buf);
        va_end(args);
    }
}

void log_debug(const char* format, ...) {
    if (global_log_level >= LOG_LEVEL_DEBUG) {
        log_prefix("DEBUG");
        va_list args;
        va_start(args, format);
        char buf[256];
        vsnprintf(buf, sizeof(buf), format, args);
        Serial.println(buf);
        va_end(args);
    }
}

void log_trace(const char* format, ...) {
    if (global_log_level >= LOG_LEVEL_TRACE) {
        log_prefix("TRACE");
        va_list args;
        va_start(args, format);
        char buf[256];
        vsnprintf(buf, sizeof(buf), format, args);
        Serial.println(buf);
        va_end(args);
    }
}

// Raw print functions without prefix
void log_print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buf[256];
    vsnprintf(buf, sizeof(buf), format, args);
    Serial.print(buf);
    va_end(args);
}

void log_println(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buf[256];
    vsnprintf(buf, sizeof(buf), format, args);
    Serial.println(buf);
    va_end(args);
} 