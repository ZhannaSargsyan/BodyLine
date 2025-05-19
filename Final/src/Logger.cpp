/**
 * @file Logger.cpp
 * @brief Implementation of the Logger class
 */
#include "../include/Logger.h"
#include "../include/Vector2D.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

Logger::Logger(const std::string& logFilePath) : initialized(false) {
    // Open log file
    logFile.open(logFilePath, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open log file: " << logFilePath << std::endl;
        return;
    }
    
    initialized = true;
    logMessage("Logger initialized");
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::logMessage(const std::string& message) {
    if (!initialized) {
        std::cerr << "Warning: Logger not initialized. Message: " << message << std::endl;
        return;
    }
    
    std::string timestamp = getTimestamp();
    logFile << timestamp << " - " << message << std::endl;
    std::cout << "LOG: " << timestamp << " - " << message << std::endl;
}

void Logger::logError(const std::string& error) {
    if (!initialized) {
        std::cerr << "Warning: Logger not initialized. Error: " << error << std::endl;
        return;
    }
    
    std::string timestamp = getTimestamp();
    logFile << timestamp << " - ERROR: " << error << std::endl;
    std::cerr << "ERROR: " << timestamp << " - " << error << std::endl;
}

void Logger::logWarning(const std::string& warning) {
    if (!initialized) {
        std::cerr << "Warning: Logger not initialized. Warning: " << warning << std::endl;
        return;
    }
    
    std::string timestamp = getTimestamp();
    logFile << timestamp << " - WARNING: " << warning << std::endl;
    std::cout << "WARNING: " << timestamp << " - " << warning << std::endl;
}

void Logger::logSnowballThrow(const Vector2D& position, const Vector2D& velocity) {
    if (!initialized) {
        std::cerr << "Warning: Logger not initialized. Snowball throw not logged." << std::endl;
        return;
    }
    
    std::stringstream ss;
    ss << "Snowball thrown from (" << position.x << ", " << position.y 
       << ") with velocity (" << velocity.x << ", " << velocity.y << ")";
    logMessage(ss.str());
}

void Logger::logSnowballHit(const Vector2D& position, bool hitTarget) {
    if (!initialized) {
        std::cerr << "Warning: Logger not initialized. Snowball hit not logged." << std::endl;
        return;
    }
    
    std::stringstream ss;
    if (hitTarget) {
        ss << "Snowball hit target at (" << position.x << ", " << position.y << ")";
    } else {
        ss << "Snowball missed target at (" << position.x << ", " << position.y << ")";
    }
    logMessage(ss.str());
}

bool Logger::isInitialized() const {
    return initialized;
}

std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}