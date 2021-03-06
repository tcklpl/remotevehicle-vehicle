/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
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
        bool should_log;
        uint8_t log_severity;
        void lprint(char *message, uint8_t line_break);
    public:
        Logger(bool do_log, uint8_t severity);
        void info(char *message);
        void warn(char *message);
        void error(char *message);
        void severe(char *message);  
};

#endif
