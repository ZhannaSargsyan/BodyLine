/**
 * @file SnowballStrategy.cpp
 * @brief Implementation of the SnowballStrategy class
 */
#include "../include/SnowballStrategy.h"
#include <cmath>
#include <iostream>
#include <sstream>

SnowballStrategy::SnowballStrategy(std::shared_ptr<Body> body, std::shared_ptr<Circle> target, 
                                   double snowballRadius, double gravity)
    : MovementStrategy(body, target), active(false), gravity(gravity), radius(snowballRadius),
      hitTarget(false), hitGround(false) {
    // Default starting position (will be updated before throw)
    position = Vector2D(0, 0);
    velocity = Vector2D(0, 0);
}

void SnowballStrategy::planSequence() {
    // For snowball strategy, planning is minimal since we just 
    // need to calculate the throw parameters at execution time
    reset();
    if (logger) {
        logger->logMessage("Snowball sequence planned");
    }
}

bool SnowballStrategy::executeNextMove() {
    // In this strategy, execute will throw the snowball if not already thrown
    if (!active && !hitTarget && !hitGround) {
        // Auto-calculate a good throwing position and velocity if not set
        if (position.x == 0 && position.y == 0) {
            Vector2D bodyPos = body->getBasePosition();
            Vector2D targetPos = target->getCenter();
            
            // Set position slightly above the body
            position = Vector2D(bodyPos.x, bodyPos.y - 50.0);
            
            // Calculate a parabolic trajectory
            double dx = targetPos.x - position.x;
            double dy = targetPos.y - position.y;
            double time = std::sqrt(2 * dx / gravity);
            velocity = Vector2D(dx / time, -gravity * time / 2 + dy / time);
        }
        
        throwSnowball();
        return true;
    }
    return false;
}

bool SnowballStrategy::isSequenceComplete() const {
    // The sequence is complete after the snowball hits something or is deactivated
    return hitTarget || hitGround || !active;
}

void SnowballStrategy::prepareThrow(const Vector2D& position, const Vector2D& velocity) {
    this->position = position;
    this->velocity = velocity;
    active = false;
    hitTarget = false;
    hitGround = false;
    
    if (logger) {
        std::stringstream ss;
        ss << "Snowball prepared at position " << position.x << ", " << position.y;
        logger->logMessage(ss.str());
    }
}

void SnowballStrategy::throwSnowball() {
    if (active) return; // Already thrown
    
    active = true;
    hitTarget = false;
    hitGround = false;
    
    if (logger) {
        std::stringstream ss;
        ss << "Snowball thrown from (" << position.x << ", " << position.y 
           << ") with velocity (" << velocity.x << ", " << velocity.y << ")";
        logger->logMessage(ss.str());
    }
}

void SnowballStrategy::update(double deltaTime) {
    if (!active) return;
    
    // Update physics
    updatePhysics(deltaTime);
    
    // Check for collisions
    checkCollisions();
}

void SnowballStrategy::reset() {
    active = false;
    hitTarget = false;
    hitGround = false;
    position = Vector2D(0, 0);
    velocity = Vector2D(0, 0);
}

bool SnowballStrategy::isActive() const {
    return active;
}

bool SnowballStrategy::hasHitTarget() const {
    return hitTarget;
}

bool SnowballStrategy::hasHitGround() const {
    return hitGround;
}

Vector2D SnowballStrategy::getPosition() const {
    return position;
}

Vector2D SnowballStrategy::getVelocity() const {
    return velocity;
}

double SnowballStrategy::getRadius() const {
    return radius;
}

void SnowballStrategy::updatePhysics(double deltaTime) {
    // Apply gravity
    velocity.y += gravity * deltaTime;
    
    // Update position
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
}

void SnowballStrategy::checkCollisions() {
    // Check collision with ground
    if (checkGroundCollision()) {
        hitGround = true;
        active = false;
        
        if (logger) {
            std::stringstream ss;
            ss << "Snowball hit ground at (" << position.x << ", " << position.y << ")";
            logger->logMessage(ss.str());
        }
        return;
    }
    
    // Check collision with target
    if (checkTargetCollision()) {
        hitTarget = true;
        active = false;
        
        if (logger) {
            std::stringstream ss;
            ss << "Snowball hit target at (" << position.x << ", " << position.y << ")";
            logger->logMessage(ss.str());
        }
        return;
    }
}

bool SnowballStrategy::checkGroundCollision() const {
    if (!body) return false;
    
    // Check if the snowball's bottom edge is at or below ground level
    return position.y + radius >= body->getGroundLevel();
}

bool SnowballStrategy::checkTargetCollision() const {
    if (!target) return false;
    
    // Calculate distance between circle centers
    double distance = (position - target->getCenter()).magnitude();
    
    // Check if distance is less than sum of radii
    return distance <= (radius + target->getRadius());
}