#ifndef i_Log
#define i_Log

class Logger;

#include <stdint.h>
#include <Arduino.h>

#define LOG_LVL_INFO    0
#define LOG_LVL_WARNING 1
#define LOG_LVL_ERROR   2
#define LOG_LVL_SEVERE  3

extern Logger logger;

class Logger {
    private:
        uint8_t should_log, log_severity;
        void lprint(char *message, uint8_t line_break);
    public:
        Logger(uint8_t do_log, uint8_t severity);
        void info(char *message);
        void warn(char *message);
        void error(char *message);
        void severe(char *message);  
};

#endif
