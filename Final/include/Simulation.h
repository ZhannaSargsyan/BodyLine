#ifndef SIMULATION_H
#define SIMULATION_H

#include "Body.h"
#include "Circle.h"
#include "Logger.h"
#include "BodyBuilder.h"
#include "MovementStrategy.h"
#include "WalkerStrategy.h"
#include "SnowballStrategy.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

/**
 * @class Simulation
 * @brief Main simulation class that brings together all components
 *
 * This class demonstrates how to use the design patterns together:
 * - Builder: To construct the body with segments
 * - Strategy: To implement different movement behaviors
 * - Singleton: For the logging functionality
 */
class Simulation {
public:
    enum class Mode { WALKER, SNOWBALL };
    
    Simulation();
    ~Simulation();
    
    // Initialize the simulation
    void initialize(const std::string& configFile = "");
    
    // Set the simulation mode
    void setMode(Mode mode);
    
    // Run the simulation for a single frame
    void update(float deltaTime);
    
    // Draw the simulation
    void draw(sf::RenderWindow& window);
    
    // Handle user input
    void handleInput(const sf::Event& event);
    
    // Get information about the current state
    bool isComplete() const;
    
private:
    // Initialize different modes
    void initializeWalkerMode();
    void initializeSnowballMode();
    
    // Create a body using the Builder pattern
    std::shared_ptr<Body> createBody();
    
    // Member variables
    std::shared_ptr<Body> body;
    std::shared_ptr<Circle> target;
    std::shared_ptr<Logger> logger;
    std::unique_ptr<MovementStrategy> currentStrategy;
    
    Mode currentMode;
    bool simulationComplete;
    
    // Configuration options
    double groundLevel;
    sf::Vector2f windowSize;
};

#endif // SIMULATION_H