#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <memory>
#include <mutex>

// Forward declaration
class Vector2D;

/**
 * @class Logger
 * @brief Logger class for consistent logging
 * 
 * This class is designed to provide straightforward logging functions
 * with a central logging mechanism.
 */
class Logger {
public:
    // Constructor - takes log file path
    Logger(const std::string& logFilePath = "simulation_log.txt");
    ~Logger();
    
    // Core logging methods
    void logMessage(const std::string& message);
    void logError(const std::string& error);
    void logWarning(const std::string& warning);
    
    // Specialized logging methods for physics events
    void logSnowballThrow(const Vector2D& position, const Vector2D& velocity);
    void logSnowballHit(const Vector2D& position, bool hitTarget);
    
    // Check if logger is initialized
    bool isInitialized() const;
    
private:
    // Get current timestamp
    std::string getTimestamp() const;
    
    std::ofstream logFile;
    bool initialized;
};

#endif // LOGGER_H