
#include "../include/BodyBuilder.h"
#include "../include/WalkerStrategy.h"
#include "../include/SnowballStrategy.h"
#include "../include/Logger.h"
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

enum class SimulationType { WALKER, SNOWBALL };

class TextSimulationWithPatterns {
public:
    TextSimulationWithPatterns(const std::string& title = "OOCatcher - CS 323 Project (Pattern Demo)")
        : simulationType(SimulationType::WALKER),
          simulationRunning(true),
          autoMode(false),
          simulationSpeed(1.0f),
          groundLevel(400.0),
          initialBodyPosition(100.0, 400.0),
          initialTargetPosition(500.0, 350.0),
          targetRadius(20.0),
          gravity(9.8) {
        
        std::cout << "=== " << title << " ===" << std::endl;
        
        // Create Logger (Singleton pattern)
        logger = std::make_shared<Logger>("simulation_log.txt");
        
        // Create target object
        targetObject = std::make_shared<Circle>(initialTargetPosition, targetRadius);
        
        // Create body using the Builder pattern
        BodyBuilder builder;
        body = builder.setBasePosition(initialBodyPosition)
                     .setGroundLevel(groundLevel)
                     .buildHumanoidBody()
                     .build();
        
        // Log initialization
        logger->logMessage("Text Simulation initialized");
        
        // Load configuration
        loadConfiguration("config/default.cfg");
        
        // Initialize Walker mode by default
        initializeWalker();
    }
    
    void run() {
        simulationRunning = true;
        logger->logMessage("Simulation started");
        
        displayInstructions();
        
        while (simulationRunning) {
            displayStatus();
            
            char command;
            std::cout << "Command (s=step, a=auto, q=quit, w=walker, b=snowball, r=reset): ";
            std::cin >> command;
            
            processCommand(command);
        }
    }
    
private:
    void loadConfiguration(const std::string& configFile) {
        std::cout << "Loading configuration from: " << configFile << std::endl;
        logger->logMessage("Using default configuration");
        
        // Apply configuration based on mode
        if (simulationType == SimulationType::WALKER) {
            logger->logMessage("Configured for Walker scenario");
        } else {
            initialTargetPosition = Vector2D(400.0, 300.0);
            targetObject->setCenter(initialTargetPosition);
            logger->logMessage("Configured for Snowball scenario");
        }
    }
    
    void processCommand(char command) {
        switch (command) {
            case 's':
                executeStep();
                break;
            case 'a':
                autoExecute();
                break;
            case 'q':
                simulationRunning = false;
                logger->logMessage("Simulation stopped by user");
                break;
            case 'w':
                logger->logMessage("Configured for Walker scenario");
                simulationType = SimulationType::WALKER;
                initializeWalker();
                logger->logMessage("Simulation started");
                break;
            case 'b':
                logger->logMessage("Configured for Snowball scenario");
                simulationType = SimulationType::SNOWBALL;
                initialTargetPosition = Vector2D(400.0, 300.0);
                targetObject->setCenter(initialTargetPosition);
                initializeSnowball();
                logger->logMessage("Simulation started");
                break;
            case 'r':
                logger->logMessage("Simulation reset");
                if (simulationType == SimulationType::WALKER) {
                    initializeWalker();
                } else {
                    initializeSnowball();
                }
                break;
            default:
                std::cout << "Unknown command" << std::endl;
        }
    }
    
    void executeStep() {
        if (simulationType == SimulationType::WALKER && walkerStrategy) {
            if (!walkerStrategy->isSequenceComplete()) {
                bool success = walkerStrategy->executeNextMove();
                std::cout << "Executed step: " << (success ? "Success" : "Failed") << std::endl;
                
                if (walkerStrategy->hasObjectBeenCaught()) {
                    std::cout << "Object caught successfully!" << std::endl;
                }
            } else {
                std::cout << "Sequence already complete" << std::endl;
            }
        } else if (simulationType == SimulationType::SNOWBALL && snowballStrategy) {
            // For snowball, we've implemented a more direct approach
            if (!snowballStrategy->isActive() && !snowballStrategy->hasHitTarget() && !snowballStrategy->hasHitGround()) {
                // Prepare and throw the snowball
                Vector2D initialPos = body->getBasePosition();
                initialPos.y -= 50; // Offset from body base
                Vector2D targetPos = targetObject->getCenter();
                
                // Calculate initial velocity (basic ballistic equation)
                double dx = targetPos.x - initialPos.x;
                double dy = targetPos.y - initialPos.y;
                double gravity = 9.8;
                double time = std::sqrt(2 * dx / gravity); // Simplified time of flight
                
                Vector2D velocity(dx/time, -gravity*time/2 + dy/time);
                
                // Set up and throw
                snowballStrategy->prepareThrow(initialPos, velocity);
                snowballStrategy->throwSnowball();
                std::cout << "Snowball thrown!" << std::endl;
                
                // Simulate physics for 3 seconds
                for (int i = 0; i < 30; i++) {
                    snowballStrategy->update(0.1);
                    std::cout << ".";
                    std::cout.flush();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    
                    if (snowballStrategy->hasHitTarget() || snowballStrategy->hasHitGround()) {
                        if (snowballStrategy->hasHitTarget()) {
                            std::cout << "\nTarget hit!" << std::endl;
                        } else {
                            std::cout << "\nSnowball hit the ground." << std::endl;
                        }
                        break;
                    }
                }
                std::cout << std::endl;
            } else {
                if (snowballStrategy->hasHitTarget()) {
                    std::cout << "Target already hit!" << std::endl;
                } else if (snowballStrategy->hasHitGround()) {
                    std::cout << "Snowball hit the ground. Try again." << std::endl;
                    snowballStrategy->reset();
                } else {
                    std::cout << "Snowball still in flight..." << std::endl;
                }
            }
        }
    }
    
    void autoExecute() {
        std::cout << "Auto-executing sequence..." << std::endl;
        
        if (simulationType == SimulationType::WALKER && walkerStrategy) {
            // Execute all walker moves until sequence is complete
            while (!walkerStrategy->isSequenceComplete()) {
                walkerStrategy->executeNextMove();
                displayStatus();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            
            // Final status check
            if (walkerStrategy->hasObjectBeenCaught()) {
                std::cout << "Object caught successfully!" << std::endl;
            } else {
                std::cout << "Sequence complete but object not caught." << std::endl;
            }
            
        } else if (simulationType == SimulationType::SNOWBALL && snowballStrategy) {
            // For snowball, we'll execute a single throw
            if (!snowballStrategy->isActive() && !snowballStrategy->hasHitTarget() && !snowballStrategy->hasHitGround()) {
                // Prepare and throw the snowball
                Vector2D initialPos = body->getBasePosition();
                initialPos.y -= 50; // Offset from body base
                Vector2D targetPos = targetObject->getCenter();
                
                // Calculate initial velocity (basic ballistic equation)
                double dx = targetPos.x - initialPos.x;
                double dy = targetPos.y - initialPos.y;
                double gravity = 9.8;
                double time = std::sqrt(2 * dx / gravity); // Simplified time of flight
                
                Vector2D velocity(dx/time, -gravity*time/2 + dy/time);
                
                // Set up and throw
                snowballStrategy->prepareThrow(initialPos, velocity);
                snowballStrategy->throwSnowball();
                std::cout << "Snowball thrown!" << std::endl;
                
                // Simulate physics for trajectory
                for (int i = 0; i < 50; i++) {
                    snowballStrategy->update(0.1);
                    std::cout << ".";
                    std::cout.flush();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    
                    if (snowballStrategy->hasHitTarget() || snowballStrategy->hasHitGround()) {
                        break;
                    }
                }
                
                if (snowballStrategy->hasHitTarget()) {
                    std::cout << "\nTARGET HIT! Great throw!" << std::endl;
                } else if (snowballStrategy->hasHitGround()) {
                    std::cout << "\nSnowball missed and hit the ground." << std::endl;
                } else {
                    std::cout << "\nSnowball still in flight (simulation timeout)." << std::endl;
                }
            } else {
                if (snowballStrategy->hasHitTarget()) {
                    std::cout << "Target already hit!" << std::endl;
                } else if (snowballStrategy->hasHitGround()) {
                    std::cout << "Snowball hit the ground. Try again." << std::endl;
                    snowballStrategy->reset();
                } else {
                    std::cout << "Snowball still in flight..." << std::endl;
                }
            }
        }
        std::cout << "Auto-execution complete." << std::endl;
    }
    
    void initializeWalker() {
        // Create the Walker Strategy (Strategy pattern)
        walkerStrategy = std::make_unique<WalkerStrategy>(body, targetObject);
        
        // Enable logging
        walkerStrategy->enableLogging(logger);
        
        // Plan the sequence
        walkerStrategy->planSequence(targetObject->getCenter());
        
        // Clear snowball strategy
        snowballStrategy = nullptr;
        
        logger->logMessage("Walker scenario initialized");
        std::cout << "Walker scenario initialized" << std::endl;
    }
    
    void initializeSnowball() {
        // Create the Snowball Strategy (Strategy pattern)
        snowballStrategy = std::make_unique<SnowballStrategy>(body, targetObject);
        
        // Enable logging
        snowballStrategy->enableLogging(logger);
        
        // Clear walker strategy
        walkerStrategy = nullptr;
        
        logger->logMessage("Snowball scenario initialized");
        std::cout << "Snowball scenario initialized" << std::endl;
    }
    
    void displayStatus() const {
        std::cout << "\n----- Current Status -----" << std::endl;
        std::cout << "Mode: " << (simulationType == SimulationType::WALKER ? "Walker" : "Snowball") << std::endl;
        std::cout << "Target position: " << targetObject->getCenter() << std::endl;
        std::cout << "Body position: " << body->getBasePosition() << std::endl;
        
        if (simulationType == SimulationType::WALKER && walkerStrategy) {
            std::cout << "Segments: " << body->getSegmentCount() << std::endl;
            std::cout << "Ground contacts: " << body->countGroundContacts() << std::endl;
            std::cout << "Object caught: " << (walkerStrategy->hasObjectBeenCaught() ? "Yes" : "No") << std::endl;
            std::cout << "Sequence complete: " << (walkerStrategy->isSequenceComplete() ? "Yes" : "No") << std::endl;
        } else if (simulationType == SimulationType::SNOWBALL && snowballStrategy) {
            std::cout << "Snowball position: " << (snowballStrategy->isActive() ? snowballStrategy->getPosition() : Vector2D(0, 0)) << std::endl;
            std::cout << "Snowball thrown: " << (snowballStrategy->isActive() ? "Yes" : "No") << std::endl;
            std::cout << "Target hit: " << (snowballStrategy->hasHitTarget() ? "Yes" : "No") << std::endl;
            std::cout << "Sequence complete: " << (snowballStrategy->isSequenceComplete() ? "Yes" : "No") << std::endl;
        }
        std::cout << std::endl;
    }
    
    void displayInstructions() const {
        std::cout << "\n=== OOCatcher Text Simulation Instructions ===" << std::endl;
        std::cout << "s - Execute a single step" << std::endl;
        std::cout << "a - Auto-execute all steps" << std::endl;
        std::cout << "r - Reset simulation" << std::endl;
        std::cout << "w - Switch to Walker scenario" << std::endl;
        std::cout << "b - Switch to Snowball scenario" << std::endl;
        std::cout << "q - Quit" << std::endl;
        std::cout << "==========================================" << std::endl;
    }
    
    std::shared_ptr<Body> body;                // The anthropomorphic body
    std::shared_ptr<Circle> targetObject;      // Target object (for both scenarios)
    std::shared_ptr<Logger> logger;            // Logger for actions (Singleton pattern)
    
    // Strategy pattern implementations
    std::unique_ptr<WalkerStrategy> walkerStrategy;      // Walker mode strategy
    std::unique_ptr<SnowballStrategy> snowballStrategy;  // Snowball mode strategy
    
    SimulationType simulationType;             // Current scenario type
    bool simulationRunning;                    // Simulation state
    bool autoMode;                             // Auto-execute movement sequence
    float simulationSpeed;                     // Speed multiplier
    
    // Configuration options
    double groundLevel;
    Vector2D initialBodyPosition;
    Vector2D initialTargetPosition;
    double targetRadius;
    double gravity;
};

// Main function for text-based simulator
int main() {
    TextSimulationWithPatterns simulation;
    simulation.run();
    return 0;
}