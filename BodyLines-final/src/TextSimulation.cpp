#include "../include/Body.h"
#include "../include/Circle.h"
#include "../include/Walker.h"
#include "../include/Snowball.h"
#include "../include/Logger.h"
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <thread>

enum class SimulationType {
    WALKER,
    SNOWBALL
};

class TextSimulation {
public:
    TextSimulation(const std::string& title = "OOCatcher Text Simulation")
        : simulationType(SimulationType::WALKER),
          simulationRunning(false),
          autoMode(false),
          simulationSpeed(1.0f),
          groundLevel(400.0),
          initialBodyPosition(100.0, groundLevel),
          initialTargetPosition(500.0, groundLevel - 50.0),
          targetRadius(20.0),
          gravity(9.81) {
        
        std::cout << "=== " << title << " ===" << std::endl;
        
        // Create shared objects
        body = std::make_shared<Body>(initialBodyPosition, groundLevel);
        targetObject = std::make_shared<Circle>(initialTargetPosition, targetRadius);
        logger = std::make_shared<Logger>("simulation_log.txt");
        
        // Log initialization
        logger->logMessage("Text Simulation initialized");
    }
    
    void configure(SimulationType type) {
        simulationType = type;
        
        // Reset any existing objects
        body = std::make_shared<Body>(initialBodyPosition, groundLevel);
        
        switch (simulationType) {
            case SimulationType::WALKER:
                targetObject = std::make_shared<Circle>(initialTargetPosition, targetRadius);
                logger->logMessage("Configured for Walker scenario");
                break;
                
            case SimulationType::SNOWBALL:
                targetObject = std::make_shared<Circle>(
                    Vector2D(initialBodyPosition.x + 300.0, groundLevel - 100.0),
                    targetRadius
                );
                logger->logMessage("Configured for Snowball scenario");
                break;
        }
    }
    
    void initialize() {
        switch (simulationType) {
            case SimulationType::WALKER:
                initializeWalker();
                break;
                
            case SimulationType::SNOWBALL:
                initializeSnowball();
                break;
        }
        
        simulationRunning = true;
        logger->logMessage("Simulation started");
        displayInstructions();
    }
    
    void loadConfig(const std::string& configFile) {
        std::cout << "Loading configuration from: " << configFile << std::endl;
        // In this text version, we use default values
        logger->logMessage("Using default configuration");
    }
    
    void run() {
        char command;
        
        while (simulationRunning) {
            // Display status
            displayStatus();
            
            // Get user command
            std::cout << "\nCommand (s=step, a=auto, q=quit, w=walker, b=snowball, r=reset): ";
            std::cin >> command;
            
            processCommand(command);
            
            // Slight delay for readability
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    
    void displayStatus() {
        std::cout << "\n----- Current Status -----" << std::endl;
        
        if (simulationType == SimulationType::WALKER) {
            std::cout << "Mode: Walker" << std::endl;
            if (walker) {
                std::cout << "Target position: (" << targetObject->getCenter().x << ", " 
                          << targetObject->getCenter().y << ")" << std::endl;
                std::cout << "Body position: (" << body->getBasePosition().x << ", " 
                          << body->getBasePosition().y << ")" << std::endl;
                std::cout << "Segments: " << body->getSegmentCount() << std::endl;
                std::cout << "Ground contacts: " << body->countGroundContacts() << std::endl;
                std::cout << "Object caught: " << (walker->hasObjectBeenCaught() ? "Yes" : "No") << std::endl;
                std::cout << "Sequence complete: " << (walker->isSequenceComplete() ? "Yes" : "No") << std::endl;
            }
        } else {
            std::cout << "Mode: Snowball" << std::endl;
            if (snowball) {
                std::cout << "Target position: (" << targetObject->getCenter().x << ", " 
                          << targetObject->getCenter().y << ")" << std::endl;
                std::cout << "Body position: (" << body->getBasePosition().x << ", " 
                          << body->getBasePosition().y << ")" << std::endl;
                std::cout << "Snowball position: (" << snowball->getCircle().getCenter().x << ", " 
                          << snowball->getCircle().getCenter().y << ")" << std::endl;
                std::cout << "Snowball thrown: " << (snowball->isActive() ? "Yes" : "No") << std::endl;
                std::cout << "Target hit: " << (snowball->hasHitTarget() ? "Yes" : "No") << std::endl;
                std::cout << "Sequence complete: " << (!snowball->isActive() ? "Yes" : "No") << std::endl;
            }
        }
    }
    
    void processCommand(char command) {
        switch (command) {
            case 's': // Step
                executeStep();
                break;
                
            case 'a': // Auto mode
                autoExecute();
                break;
                
            case 'q': // Quit
                simulationRunning = false;
                std::cout << "Quitting simulation..." << std::endl;
                break;
                
            case 'w': // Switch to Walker
                configure(SimulationType::WALKER);
                initialize();
                break;
                
            case 'b': // Switch to Snowball
                configure(SimulationType::SNOWBALL);
                initialize();
                break;
                
            case 'r': // Reset
                initialize();
                std::cout << "Simulation reset" << std::endl;
                break;
                
            default:
                std::cout << "Unknown command" << std::endl;
        }
    }
    
    void executeStep() {
        if (simulationType == SimulationType::WALKER && walker) {
            if (!walker->isSequenceComplete()) {
                bool success = walker->executeNextMove();
                std::cout << "Executed step: " << (success ? "Success" : "Failed") << std::endl;
                
                if (walker->hasObjectBeenCaught()) {
                    std::cout << "Object caught successfully!" << std::endl;
                }
            } else {
                std::cout << "Sequence already complete" << std::endl;
            }
        } else if (simulationType == SimulationType::SNOWBALL && snowball) {
            // For snowball, we've implemented a more direct approach
            if (!snowball->isActive() && !snowball->hasHitTarget() && !snowball->hasHitGround()) {
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
                snowball->prepareThrow(initialPos, velocity);
                snowball->throwSnowball();
                std::cout << "Snowball thrown!" << std::endl;
                
                // Simulate physics for 3 seconds
                for (int i = 0; i < 30; i++) {
                    snowball->update(0.1);
                    std::cout << ".";
                    std::cout.flush();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    
                    if (snowball->hasHitTarget() || snowball->hasHitGround()) {
                        if (snowball->hasHitTarget()) {
                            std::cout << "\nTarget hit!" << std::endl;
                        } else {
                            std::cout << "\nSnowball hit the ground." << std::endl;
                        }
                        break;
                    }
                }
                std::cout << std::endl;
            } else {
                if (snowball->hasHitTarget()) {
                    std::cout << "Target already hit!" << std::endl;
                } else if (snowball->hasHitGround()) {
                    std::cout << "Snowball hit the ground. Try again." << std::endl;
                    snowball->reset();
                } else {
                    std::cout << "Snowball still in flight..." << std::endl;
                }
            }
        }
    }
    
    void autoExecute() {
        std::cout << "Auto-executing sequence..." << std::endl;
        
        if (simulationType == SimulationType::WALKER && walker) {
            // Execute all walker moves until sequence is complete
            while (!walker->isSequenceComplete()) {
                walker->executeNextMove();
                displayStatus();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            
            // Final status check
            if (walker->hasObjectBeenCaught()) {
                std::cout << "Object caught successfully!" << std::endl;
            } else {
                std::cout << "Sequence complete but object not caught." << std::endl;
            }
            
        } else if (simulationType == SimulationType::SNOWBALL && snowball) {
            // For snowball, we'll execute a single throw
            if (!snowball->isActive() && !snowball->hasHitTarget() && !snowball->hasHitGround()) {
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
                snowball->prepareThrow(initialPos, velocity);
                snowball->throwSnowball();
                std::cout << "Snowball thrown!" << std::endl;
                
                // Simulate physics for trajectory
                for (int i = 0; i < 50; i++) {
                    snowball->update(0.1);
                    std::cout << ".";
                    std::cout.flush();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    
                    if (snowball->hasHitTarget() || snowball->hasHitGround()) {
                        break;
                    }
                }
                
                if (snowball->hasHitTarget()) {
                    std::cout << "\nTARGET HIT! Great throw!" << std::endl;
                } else if (snowball->hasHitGround()) {
                    std::cout << "\nSnowball missed and hit the ground." << std::endl;
                } else {
                    std::cout << "\nSnowball still in flight (simulation timeout)." << std::endl;
                }
            } else {
                if (snowball->hasHitTarget()) {
                    std::cout << "Target already hit!" << std::endl;
                } else if (snowball->hasHitGround()) {
                    std::cout << "Snowball hit the ground. Try again." << std::endl;
                    snowball->reset();
                } else {
                    std::cout << "Snowball still in flight..." << std::endl;
                }
            }
        }
        std::cout << "Auto-execution complete." << std::endl;
    }
    
private:
    void initializeWalker() {
        // Create a Walker with the body
        walker = std::make_unique<Walker>(body);
        
        // Set target object and enable logging
        walker->setTargetObject(targetObject);
        walker->enableLogging(logger);
        
        // Plan catching sequence to the target object
        walker->planCatchSequence(targetObject->getCenter());
        
        logger->logMessage("Walker scenario initialized");
        std::cout << "Walker scenario initialized" << std::endl;
    }
    
    void initializeSnowball() {
        // Create a Snowball
        snowball = std::make_unique<Snowball>();
        
        // Configure the snowball with thrower and target
        snowball->setThrower(body);
        snowball->setTarget(targetObject);
        snowball->enableLogging(logger);
        
        // Default position is handled in executeStep
        
        logger->logMessage("Snowball scenario initialized");
        std::cout << "Snowball scenario initialized" << std::endl;
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
    std::shared_ptr<Logger> logger;            // Logger for actions
    
    // Scenario-specific components
    std::unique_ptr<Walker> walker;            // Walker scenario
    std::unique_ptr<Snowball> snowball;        // Snowball scenario
    
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