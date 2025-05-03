/**
 * @file Logger.h
 * @brief Defines a Logger class for tracking simulation state and events
 */
#ifndef LOGGER_H
#define LOGGER_H

#include "Vector2D.h"
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iostream>

class Logger {
public:
    // Constructor - can be passed a logfile path to write to
    Logger(const std::string& logFile = "");

    // Destructor - closes log file if opened
    ~Logger();
    
    // Log general activity
    void logMessage(const std::string& message);
    
    // Log position of body
    void logBodyPosition(const Vector2D& position);
    
    // Log simulation step
    void logStep(int stepNumber, const std::string& details = "");
    
    // Log snowball throw event
    void logSnowballThrow(const Vector2D& position, const Vector2D& velocity);
    
    // Log snowball hit/miss event
    void logSnowballHit(const Vector2D& targetPosition, bool hit);
    
    // Get timestamp as string
    static std::string getTimestamp();

private:
    bool loggingEnabled;         // Whether logging is active
    std::ofstream logFile;       // Output file stream
    
    // Helper method to write to both file and console
    void writeLog(const std::string& entry);
};

#endif // LOGGER_H