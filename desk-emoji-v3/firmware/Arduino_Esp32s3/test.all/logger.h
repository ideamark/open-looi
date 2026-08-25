#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

// Log Levels
#define LOG_LEVEL_NONE    0
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARN    2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_DEBUG   4
#define LOG_LEVEL_TRACE   5

// Default log level
extern uint8_t global_log_level;

// Set the global log level
void log_set_level(uint8_t level);

// Log functions for different levels
void log_error(const char* format, ...);
void log_warn(const char* format, ...);
void log_info(const char* format, ...);
void log_debug(const char* format, ...);
void log_trace(const char* format, ...);

// Raw print functions (no timestamp or level indicator)
void log_print(const char* format, ...);
void log_println(const char* format, ...);

#endif // LOGGER_H 