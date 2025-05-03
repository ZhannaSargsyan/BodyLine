/**
 * @file Snowball.h
 * @brief Defines a projectile that can be thrown by the body
 */
#ifndef SNOWBALL_H
#define SNOWBALL_H

#include "Body.h"
#include "Circle.h"
#include "Logger.h"
#include <memory>
#include <vector>

class Snowball {
public:
    // Constructor
    Snowball(double radius = 10.0, double gravityValue = 9.8);
    
    // Set the body that will throw this snowball
    void setThrower(const std::shared_ptr<Body>& body);
    
    // Get the snowball circle (for collision detection)
    const Circle& getCircle() const;
    
    // Setup a throw with initial velocity
    void prepareThrow(const Vector2D& initialPosition, const Vector2D& initialVelocity);
    
    // Perform the throw
    void throwSnowball();
    
    // Update the snowball position based on physics (gravity, etc.)
    void update(double timeStep);
    
    // Reset the snowball for a new throw
    void reset();
    
    // Getters
    bool isActive() const;
    bool hasHitTarget() const;
    bool hasHitGround() const;
    double getRadius() const;
    
    // Target interaction
    void setTarget(const std::shared_ptr<Circle>& target);
    bool checkTargetHit();
    
    // Enable logging
    void enableLogging(std::shared_ptr<Logger> logger);
    
private:
    Circle snowball;              // The physical representation of the snowball
    double gravity;               // Gravity value for physics simulation
    bool active;                  // Whether the snowball is currently in flight
    bool hitTarget;               // Whether the snowball has hit its target
    bool hitGround;               // Whether the snowball has hit the ground
    std::weak_ptr<Body> thrower;  // The body that throws this snowball
    std::weak_ptr<Circle> target; // The target to hit
    double groundLevel;           // The y-coordinate of the ground
    
    // Optional logger
    std::shared_ptr<Logger> logger;
};

#endif // SNOWBALL_H