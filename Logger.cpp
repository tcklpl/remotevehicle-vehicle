/*
    RemoteVehicle - library for controlling a remote vehicle using a ESP32CAM board.
    Created by Luan Negroni Sibinel, August 24, 2021.
    Released under the MIT license.
*/
#include "Logger.h"

Logger logger = Logger(0, LOG_LVL_SEVERE);

Logger::Logger(bool do_log, uint8_t severity) {
    should_log = do_log;
    log_severity = severity;
}

void Logger::lprint(char *message, uint8_t line_break) {
    line_break ? Serial.println(message) : Serial.print(message);
}

void Logger::info(char *message) {
    if (!should_log) return;
    if (log_severity > LOG_LVL_INFO) return;
    lprint("[  INFO  ] ", 0);
    lprint(message, 1);
}

void Logger::warn(char *message) {
    if (!should_log) return;
    if (log_severity > LOG_LVL_WARNING) return;
    lprint("[  WARN  ] ", 0);
    lprint(message, 1);
}

void Logger::error(char *message) {
    if (!should_log) return;
    if (log_severity > LOG_LVL_ERROR) return;
    lprint("[  ERROR ] ", 0);
    lprint(message, 1);
}

void Logger::severe(char *message) {
    if (!should_log) return;
    lprint("[ SEVERE ] ", 0);
    lprint(message, 1);
}
