/**
 * @file Logger.cpp
 * @brief Implementation of the Logger class
 */
#include "../include/Logger.h"
#include <iomanip>
#include <sstream>

Logger::Logger(const std::string& logFileName) : loggingEnabled(false) {
    if (!logFileName.empty()) {
        logFile.open(logFileName, std::ios::out | std::ios::trunc);
        loggingEnabled = logFile.is_open();
        
        if (loggingEnabled) {
            // Write header to log file
            logFile << "=== OOCatcher Simulation Log ===\n";
            logFile << "Started at: " << getTimestamp() << "\n";
            logFile << "================================\n\n";
        } else {
            std::cerr << "WARNING: Could not open log file: " << logFileName << std::endl;
        }
    }
}

Logger::~Logger() {
    if (loggingEnabled && logFile.is_open()) {
        // Write footer to log file
        logFile << "\n================================\n";
        logFile << "Ended at: " << getTimestamp() << "\n";
        logFile << "=== End of Simulation Log ===\n";
        
        logFile.close();
    }
}

void Logger::logMessage(const std::string& message) {
    std::string entry = getTimestamp() + " - " + message;
    writeLog(entry);
}

void Logger::logBodyPosition(const Vector2D& position) {
    std::stringstream ss;
    ss << "Body position: (" << position.x << ", " << position.y << ")";
    logMessage(ss.str());
}

void Logger::logStep(int stepNumber, const std::string& details) {
    std::stringstream ss;
    ss << "STEP " << stepNumber << ": " << (details.empty() ? "Executing" : details);
    logMessage(ss.str());
}

void Logger::logSnowballThrow(const Vector2D& position, const Vector2D& velocity) {
    std::stringstream ss;
    ss << "Snowball thrown from (" << position.x << ", " << position.y << ") ";
    ss << "with velocity (" << velocity.x << ", " << velocity.y << ")";
    logMessage(ss.str());
}

void Logger::logSnowballHit(const Vector2D& targetPosition, bool hit) {
    std::stringstream ss;
    if (hit) {
        ss << "TARGET HIT at position (" << targetPosition.x << ", " << targetPosition.y << ")";
    } else {
        ss << "TARGET MISSED at position (" << targetPosition.x << ", " << targetPosition.y << ")";
    }
    logMessage(ss.str());
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Logger::writeLog(const std::string& entry) {
    // Always print to console
    std::cout << "LOG: " << entry << std::endl;
    
    // If log file is open, write to it
    if (loggingEnabled && logFile.is_open()) {
        logFile << entry << std::endl;
        logFile.flush();  // Ensure it's written immediately
    }
}