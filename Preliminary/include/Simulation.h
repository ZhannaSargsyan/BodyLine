/**
 * @file Simulation.h
 * @brief Defines the simulation framework for the OOCatcher project
 */
#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include "Body.h"
#include "Circle.h"
#include "Logger.h"
#include "Walker.h"
#include "Snowball.h"

enum class SimulationType {
    WALKER,
    SNOWBALL
};

class Simulation {
public:
    Simulation(int width, int height, const std::string& title = "OOCatcher Simulation");
    
    // Setup and initialization
    void configure(SimulationType type);
    void initialize();
    void loadConfig(const std::string& configFile);
    
    // Simulation control
    void run();
    void update(float deltaTime);
    void render();
    void handleEvents();
    
    // Scenario-specific
    void initializeWalker();
    void initializeSnowball();
    void updateWalker(float deltaTime);
    void updateSnowball(float deltaTime);
    
    // User interface
    void processUserInput();
    void displayInstructions() const;
    
private:
    sf::RenderWindow window;                   // SFML window for display
    sf::Font font;                             // Font for text display
    sf::View view;                             // Camera view
    
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
    float autoStepTimer;                       // Timer for auto-stepping
    float autoStepInterval;                    // Interval between auto steps
    
    // Drawing helpers
    void drawBody() const;
    void drawCircle(const Circle& circle, const sf::Color& color) const;
    void drawText(const std::string& text, float x, float y, unsigned int size = 16) const;
    void drawTrajectory(const Vector2D& start, const Vector2D& velocity, const sf::Color& color) const;
    
    // Coordinate transformations
    sf::Vector2f worldToScreen(const Vector2D& worldPos) const;
    Vector2D screenToWorld(const sf::Vector2f& screenPos) const;
    
    // Configuration options
    double groundLevel;
    Vector2D initialBodyPosition;
    Vector2D initialTargetPosition;
    double targetRadius;
    double gravity;
    sf::Color bodyColor;
    sf::Color targetColor;
    sf::Color trajectoryColor;
    sf::Color groundColor;
    sf::Color backgroundColor;
};

#endif // SIMULATION_H
