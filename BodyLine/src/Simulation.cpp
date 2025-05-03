/**
 * @file Simulation.cpp
 * @brief Implementation of the Simulation class
 */
#include "../include/Simulation.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

Simulation::Simulation(int width, int height, const std::string& title)
    : simulationType(SimulationType::WALKER),
      simulationRunning(false),
      autoMode(false),
      simulationSpeed(1.0f),
      autoStepTimer(0.0f),
      autoStepInterval(0.5f),
      groundLevel(400.0),
      initialBodyPosition(100.0, groundLevel),
      initialTargetPosition(500.0, groundLevel - 50.0),
      targetRadius(20.0),
      gravity(9.81),
      bodyColor(sf::Color(100, 100, 255)),
      targetColor(sf::Color(255, 100, 100)),
      trajectoryColor(sf::Color(200, 200, 200, 128)),
      groundColor(sf::Color(100, 200, 100)),
      backgroundColor(sf::Color(240, 240, 240)) {
    
    // Create the SFML window
    window.create(sf::VideoMode(width, height), title);
    window.setFramerateLimit(60);
    
    // Set up the view
    view = window.getDefaultView();
    
    // Load font
    if (!font.loadFromFile("resources/arial.ttf")) {
        // Try a fallback path
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            std::cerr << "Could not load font" << std::endl;
        }
    }
    
    // Create shared objects
    body = std::make_shared<Body>(initialBodyPosition, groundLevel);
    targetObject = std::make_shared<Circle>(initialTargetPosition, targetRadius);
    logger = std::make_shared<Logger>("simulation_log.txt");
    
    // Log initialization
    logger->logMessage("Simulation initialized");
}

void Simulation::configure(SimulationType type) {
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

void Simulation::initialize() {
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

void Simulation::loadConfig(const std::string& configFile) {
    std::ifstream file(configFile);
    
    if (!file.is_open()) {
        std::cerr << "Failed to load configuration file: " << configFile << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string key;
        
        if (std::getline(iss, key, '=')) {
            std::string value;
            if (std::getline(iss, value)) {
                // Remove leading/trailing whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                if (key == "ground_level") {
                    groundLevel = std::stod(value);
                } else if (key == "body_x") {
                    initialBodyPosition.x = std::stod(value);
                } else if (key == "body_y") {
                    initialBodyPosition.y = std::stod(value);
                } else if (key == "target_x") {
                    initialTargetPosition.x = std::stod(value);
                } else if (key == "target_y") {
                    initialTargetPosition.y = std::stod(value);
                } else if (key == "target_radius") {
                    targetRadius = std::stod(value);
                } else if (key == "gravity") {
                    gravity = std::stod(value);
                } else if (key == "simulation_type") {
                    if (value == "walker") {
                        simulationType = SimulationType::WALKER;
                    } else if (value == "snowball") {
                        simulationType = SimulationType::SNOWBALL;
                    }
                } else if (key == "auto_step_interval") {
                    autoStepInterval = std::stof(value);
                }
            }
        }
    }
    
    file.close();
    
    // Update body position
    body = std::make_shared<Body>(initialBodyPosition, groundLevel);
    
    // Update target position based on simulation type
    if (simulationType == SimulationType::WALKER) {
        targetObject = std::make_shared<Circle>(initialTargetPosition, targetRadius);
    } else {
        targetObject = std::make_shared<Circle>(
            Vector2D(initialBodyPosition.x + 300.0, groundLevel - 100.0),
            targetRadius
        );
    }
    
    logger->logMessage("Configuration loaded from " + configFile);
}

void Simulation::run() {
    sf::Clock clock;
    
    while (window.isOpen()) {
        // Calculate delta time
        float deltaTime = clock.restart().asSeconds();
        
        // Handle events
        handleEvents();
        
        // Update simulation
        update(deltaTime);
        
        // Render simulation
        render();
    }
}

void Simulation::update(float deltaTime) {
    if (!simulationRunning) {
        return;
    }
    
    // Apply simulation speed
    deltaTime *= simulationSpeed;
    
    // Update based on scenario type
    switch (simulationType) {
        case SimulationType::WALKER:
            updateWalker(deltaTime);
            break;
            
        case SimulationType::SNOWBALL:
            updateSnowball(deltaTime);
            break;
    }
    
    // Process auto-step if enabled
    if (autoMode) {
        autoStepTimer += deltaTime;
        
        if (autoStepTimer >= autoStepInterval) {
            autoStepTimer = 0.0f;
            
            // Execute next step in sequence
            if (simulationType == SimulationType::WALKER && walker) {
                if (!walker->isSequenceComplete()) {
                    walker->executeNextStep();
                }
            } else if (simulationType == SimulationType::SNOWBALL && snowball) {
                if (!snowball->isSequenceComplete() && !snowball->isSnowballThrown()) {
                    snowball->executeNextStep();
                }
            }
        }
    }
}

void Simulation::render() {
    window.clear(backgroundColor);
    
    // Draw ground
    sf::RectangleShape ground(sf::Vector2f(window.getSize().x, window.getSize().y - groundLevel));
    ground.setPosition(0, groundLevel);
    ground.setFillColor(groundColor);
    window.draw(ground);
    
    // Draw body segments
    drawBody();
    
    // Draw target
    drawCircle(*targetObject, targetColor);
    
    // Draw snowball if in snowball scenario
    if (simulationType == SimulationType::SNOWBALL && snowball) {
        drawCircle(snowball->getSnowball(), sf::Color::White);
        
        // Draw trajectory if snowball is thrown
        if (snowball->isSnowballThrown()) {
            const Circle& ball = snowball->getSnowball();
            drawTrajectory(ball.getCenter(), ball.getVelocity(), trajectoryColor);
        }
    }
    
    // Draw status text
    std::string statusText;
    if (simulationType == SimulationType::WALKER && walker) {
        statusText = "Walker - ";
        statusText += walker->isObjectCaught() ? "Object Caught!" : "Moving...";
    } else if (simulationType == SimulationType::SNOWBALL && snowball) {
        statusText = "Snowball - ";
        if (snowball->isSnowballThrown()) {
            statusText += snowball->isTargetHit() ? "Target Hit!" : "Snowball in air...";
        } else {
            statusText += "Preparing throw...";
        }
    }
    
    drawText(statusText, 10, 10, 20);
    
    // Draw auto-mode indicator
    if (autoMode) {
        drawText("Auto Mode (Space to pause)", 10, 40);
    } else {
        drawText("Manual Mode (Space to auto, S for step)", 10, 40);
    }
    
    window.display();
}

void Simulation::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            } else if (event.key.code == sf::Keyboard::Space) {
                // Toggle auto mode
                autoMode = !autoMode;
                autoStepTimer = 0.0f;
                logger->logMessage(autoMode ? "Auto mode enabled" : "Auto mode disabled");
            } else if (event.key.code == sf::Keyboard::S && !autoMode) {
                // Manual step
                if (simulationType == SimulationType::WALKER && walker) {
                    if (!walker->isSequenceComplete()) {
                        walker->executeNextStep();
                    }
                } else if (simulationType == SimulationType::SNOWBALL && snowball) {
                    if (!snowball->isSequenceComplete() && !snowball->isSnowballThrown()) {
                        snowball->executeNextStep();
                    }
                }
            } else if (event.key.code == sf::Keyboard::R) {
                // Reset simulation
                initialize();
                logger->logMessage("Simulation reset");
            } else if (event.key.code == sf::Keyboard::W) {
                // Switch to walker scenario
                configure(SimulationType::WALKER);
                initialize();
            } else if (event.key.code == sf::Keyboard::B) {
                // Switch to snowball scenario
                configure(SimulationType::SNOWBALL);
                initialize();
            } else if (event.key.code == sf::Keyboard::Up) {
                // Increase simulation speed
                simulationSpeed *= 1.5f;
                simulationSpeed = std::min(simulationSpeed, 5.0f);
                logger->logMessage("Simulation speed increased to " + std::to_string(simulationSpeed));
            } else if (event.key.code == sf::Keyboard::Down) {
                // Decrease simulation speed
                simulationSpeed /= 1.5f;
                simulationSpeed = std::max(simulationSpeed, 0.1f);
                logger->logMessage("Simulation speed decreased to " + std::to_string(simulationSpeed));
            }
        } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Move target object
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                Vector2D worldPos = screenToWorld(sf::Vector2f(mousePos.x, mousePos.y));
                
                targetObject->setCenter(worldPos);
                logger->logMessage("Target moved to " + std::to_string(worldPos.x) + ", " + std::to_string(worldPos.y));
                
                // Re-plan movement sequence
                if (simulationType == SimulationType::WALKER && walker) {
                    walker->planCatchSequence();
                } else if (simulationType == SimulationType::SNOWBALL && snowball) {
                    snowball->planThrowSequence();
                }
            }
        }
    }
}

void Simulation::initializeWalker() {
    // Create walker scenario
    walker = std::make_unique<Walker>(body, targetObject, logger);
    walker->initializeDefaultBody();
    walker->planCatchSequence();
    
    logger->logMessage("Walker scenario initialized");
}

void Simulation::initializeSnowball() {
    // Create snowball scenario
    snowball = std::make_unique<Snowball>(body, targetObject, logger);
    snowball->initializeDefaultBody();
    snowball->planThrowSequence();
    
    logger->logMessage("Snowball scenario initialized");
}

void Simulation::updateWalker(float deltaTime) {
    // Nothing to do here as walker is updated through step execution
}

void Simulation::updateSnowball(float deltaTime) {
    if (snowball && snowball->isSnowballThrown()) {
        // Update snowball physics simulation
        snowball->simulateThrow(deltaTime);
    }
}

void Simulation::processUserInput() {
    // This is handled in handleEvents()
}

void Simulation::displayInstructions() const {
    std::cout << "=== OOCatcher Simulation Instructions ===" << std::endl;
    std::cout << "Space - Toggle automatic execution mode" << std::endl;
    std::cout << "S - Execute a single step (in manual mode)" << std::endl;
    std::cout << "R - Reset simulation" << std::endl;
    std::cout << "W - Switch to Walker scenario" << std::endl;
    std::cout << "B - Switch to Snowball scenario" << std::endl;
    std::cout << "Up/Down - Increase/decrease simulation speed" << std::endl;
    std::cout << "Left Mouse Click - Move target object" << std::endl;
    std::cout << "Esc - Exit" << std::endl;
    std::cout << "=========================================" << std::endl;
}

void Simulation::drawBody() const {
    const auto& lines = body->getSegmentLines();
    
    for (const auto& line : lines) {
        sf::Vertex sfLine[] = {
            sf::Vertex(worldToScreen(line.first), bodyColor),
            sf::Vertex(worldToScreen(line.second), bodyColor)
        };
        
        window.draw(sfLine, 2, sf::Lines);
        
        // Draw joints as circles
        sf::CircleShape joint(3);
        joint.setFillColor(sf::Color::Black);
        joint.setOrigin(3, 3);
        joint.setPosition(worldToScreen(line.first));
        window.draw(joint);
        
        joint.setPosition(worldToScreen(line.second));
        window.draw(joint);
    }
}

void Simulation::drawCircle(const Circle& circle, const sf::Color& color) const {
    sf::CircleShape shape(circle.getRadius());
    shape.setFillColor(color);
    shape.setOrigin(circle.getRadius(), circle.getRadius());
    shape.setPosition(worldToScreen(circle.getCenter()));
    
    window.draw(shape);
}

void Simulation::drawText(const std::string& text, float x, float y, unsigned int size) const {
    sf::Text textObj;
    textObj.setFont(font);
    textObj.setString(text);
    textObj.setCharacterSize(size);
    textObj.setFillColor(sf::Color::Black);
    textObj.setPosition(x, y);
    
    window.draw(textObj);
}

void Simulation::drawTrajectory(const Vector2D& start, const Vector2D& velocity, const sf::Color& color) const {
    // Draw predicted trajectory
    sf::VertexArray trajectory(sf::LineStrip);
    
    Vector2D pos = start;
    Vector2D vel = velocity;
    const double dt = 0.1;
    
    for (int i = 0; i < 100; ++i) {
        // Add current position to trajectory
        trajectory.append(sf::Vertex(worldToScreen(pos), color));
        
        // Update position and velocity for next point
        vel.y += gravity * dt;
        pos += vel * dt;
        
        // Stop if trajectory hits the ground
        if (pos.y >= groundLevel) {
            pos.y = groundLevel;
            trajectory.append(sf::Vertex(worldToScreen(pos), color));
            break;
        }
    }
    
    window.draw(trajectory);
}

sf::Vector2f Simulation::worldToScreen(const Vector2D& worldPos) const {
    return sf::Vector2f(worldPos.x, worldPos.y);
}

Vector2D Simulation::screenToWorld(const sf::Vector2f& screenPos) const {
    return Vector2D(screenPos.x, screenPos.y);
}
