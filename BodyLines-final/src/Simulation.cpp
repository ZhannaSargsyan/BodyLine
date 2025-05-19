/**
 * @file Simulation.cpp
 * @brief Implementation of the Simulation class
 */
#include "../include/Simulation.h"
#include <iostream>
#include <fstream>
#include <sstream>

Simulation::Simulation() 
    : currentMode(Mode::WALKER), 
      simulationComplete(false),
      groundLevel(400.0),
      windowSize(800.0f, 600.0f) {
}

Simulation::~Simulation() {
    // Clean up resources if needed
}

void Simulation::initialize(const std::string& configFile) {
    // Initialize logger
    logger = std::make_shared<Logger>("simulation_log.txt");
    logger->logMessage("Simulation initialized");
    
    // Create target object
    target = std::make_shared<Circle>(Vector2D(500.0, 350.0), 20.0);
    
    // Create body using the Builder pattern
    body = createBody();
    
    // Default to Walker mode
    setMode(Mode::WALKER);
}

void Simulation::setMode(Mode mode) {
    currentMode = mode;
    simulationComplete = false;
    
    // Initialize appropriate strategy based on mode
    switch (currentMode) {
        case Mode::WALKER:
            initializeWalkerMode();
            break;
        case Mode::SNOWBALL:
            initializeSnowballMode();
            break;
    }
    
    logger->logMessage(mode == Mode::WALKER ? "Walker mode activated" : "Snowball mode activated");
}

void Simulation::update(float deltaTime) {
    if (simulationComplete) return;
    
    // Update the current strategy
    if (currentStrategy) {
        if (!currentStrategy->isSequenceComplete()) {
            // Execute the next move in the strategy
            currentStrategy->executeNextMove();
        } else {
            simulationComplete = true;
            logger->logMessage("Simulation sequence complete");
        }
    }
    
    // Handle specific updates based on the mode
    if (currentMode == Mode::SNOWBALL) {
        auto snowballStrategy = dynamic_cast<SnowballStrategy*>(currentStrategy.get());
        if (snowballStrategy && snowballStrategy->isActive()) {
            snowballStrategy->update(deltaTime);
        }
    }
}

void Simulation::draw(sf::RenderWindow& window) {
    // Clear the window
    window.clear(sf::Color(230, 230, 230));
    
    // Draw ground
    sf::RectangleShape ground(sf::Vector2f(windowSize.x, windowSize.y - groundLevel));
    ground.setPosition(0, groundLevel);
    ground.setFillColor(sf::Color(100, 180, 100));
    window.draw(ground);
    
    // Draw target
    sf::CircleShape targetShape(target->getRadius());
    targetShape.setFillColor(sf::Color(200, 50, 50));
    targetShape.setPosition(
        target->getCenter().x - target->getRadius(),
        target->getCenter().y - target->getRadius()
    );
    window.draw(targetShape);
    
    // Draw body segments
    const auto& lines = body->getSegmentLines();
    for (const auto& line : lines) {
        sf::Vertex sfLine[] = {
            sf::Vertex(sf::Vector2f(line.first.x, line.first.y), sf::Color(50, 50, 200)),
            sf::Vertex(sf::Vector2f(line.second.x, line.second.y), sf::Color(50, 50, 200))
        };
        window.draw(sfLine, 2, sf::Lines);
    }
    
    // Draw snowball if active
    if (currentMode == Mode::SNOWBALL) {
        auto snowballStrategy = dynamic_cast<SnowballStrategy*>(currentStrategy.get());
        if (snowballStrategy && snowballStrategy->isActive()) {
            const Vector2D& pos = snowballStrategy->getPosition();
            sf::CircleShape snowball(snowballStrategy->getRadius());
            snowball.setFillColor(sf::Color(255, 255, 255));
            snowball.setPosition(
                pos.x - snowballStrategy->getRadius(),
                pos.y - snowballStrategy->getRadius()
            );
            window.draw(snowball);
        }
    }
    
    // Draw status text
    sf::Font font;
    if (font.loadFromFile("resources/arial.ttf")) {
        sf::Text statusText;
        statusText.setFont(font);
        statusText.setCharacterSize(18);
        statusText.setFillColor(sf::Color::Black);
        
        std::stringstream ss;
        ss << "Mode: " << (currentMode == Mode::WALKER ? "Walker" : "Snowball") << "\n";
        ss << "Status: " << (simulationComplete ? "Complete" : "Running") << "\n";
        
        if (currentMode == Mode::WALKER) {
            auto walkerStrategy = dynamic_cast<WalkerStrategy*>(currentStrategy.get());
            if (walkerStrategy) {
                ss << "Object Caught: " << (walkerStrategy->hasObjectBeenCaught() ? "Yes" : "No") << "\n";
            }
        } else if (currentMode == Mode::SNOWBALL) {
            auto snowballStrategy = dynamic_cast<SnowballStrategy*>(currentStrategy.get());
            if (snowballStrategy) {
                ss << "Snowball Thrown: " << (snowballStrategy->isActive() ? "Yes" : "No") << "\n";
                ss << "Target Hit: " << (snowballStrategy->hasHitTarget() ? "Yes" : "No") << "\n";
            }
        }
        
        statusText.setString(ss.str());
        statusText.setPosition(10, 10);
        window.draw(statusText);
    }
}

void Simulation::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::W:
                // Switch to Walker mode
                setMode(Mode::WALKER);
                break;
                
            case sf::Keyboard::S:
                // Switch to Snowball mode
                setMode(Mode::SNOWBALL);
                break;
                
            case sf::Keyboard::Space:
                // Execute next step
                update(0.1f);
                break;
                
            case sf::Keyboard::R:
                // Reset the current mode
                setMode(currentMode);
                break;
                
            default:
                break;
        }
    }
}

bool Simulation::isComplete() const {
    return simulationComplete;
}

void Simulation::initializeWalkerMode() {
    // Create a WalkerStrategy
    auto walkerStrategy = std::make_unique<WalkerStrategy>(body, target);
    walkerStrategy->enableLogging(logger);
    walkerStrategy->planSequence(target->getCenter());
    currentStrategy = std::move(walkerStrategy);
}

void Simulation::initializeSnowballMode() {
    // Create a SnowballStrategy
    auto snowballStrategy = std::make_unique<SnowballStrategy>(body, target);
    snowballStrategy->enableLogging(logger);
    snowballStrategy->planSequence();
    currentStrategy = std::move(snowballStrategy);
}

std::shared_ptr<Body> Simulation::createBody() {
    // Use the BodyBuilder to create a body with a humanoid structure
    BodyBuilder builder;
    builder.setBasePosition(Vector2D(100.0, 400.0))
           .setGroundLevel(groundLevel)
           .buildHumanoidBody();
    
    return builder.build();
}