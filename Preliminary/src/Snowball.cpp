/**
 * @file Snowball.cpp
 * @brief Implementation of the Snowball class
 */
#include "../include/Snowball.h"
#include <cmath>
#include <iostream>

Snowball::Snowball(double radius, double gravityValue)
    : snowball(Vector2D(0, 0), radius),
      gravity(gravityValue),
      active(false),
      hitTarget(false),
      hitGround(false),
      groundLevel(400) {  // Default ground level, can be updated by Body
}

void Snowball::setThrower(const std::shared_ptr<Body>& body) {
    thrower = body;
    
    // Update ground level from thrower
    if (auto sharedThrower = thrower.lock()) {
        groundLevel = sharedThrower->getGroundLevel();
    }
}

const Circle& Snowball::getCircle() const {
    return snowball;
}

void Snowball::prepareThrow(const Vector2D& initialPosition, const Vector2D& initialVelocity) {
    // Position the snowball at the initial position
    snowball.setCenter(initialPosition);
    
    // Set up ballistics with initial velocity and gravity
    snowball.setBallistics(initialVelocity, gravity);
    
    // Reset status flags
    active = false;
    hitTarget = false;
    hitGround = false;
    
    if (logger) {
        logger->logMessage("Snowball prepared at position " + 
                          std::to_string(initialPosition.x) + ", " + 
                          std::to_string(initialPosition.y));
    }
}

void Snowball::throwSnowball() {
    active = true;
    
    // Log the throw
    if (logger) {
        Vector2D pos = snowball.getCenter();
        Vector2D vel = snowball.getVelocity();
        logger->logSnowballThrow(pos, vel);
    }
}

void Snowball::update(double timeStep) {
    if (!active) {
        return;
    }
    
    // Update snowball position based on physics
    snowball.updatePosition(timeStep);
    
    // Check for collisions with ground
    if (snowball.isOnGround(groundLevel)) {
        hitGround = true;
        active = false;
        
        if (logger) {
            logger->logMessage("Snowball hit ground at x = " + 
                              std::to_string(snowball.getCenter().x));
        }
    }
    
    // Check for collisions with target
    checkTargetHit();
}

void Snowball::reset() {
    active = false;
    hitTarget = false;
    hitGround = false;
    
    // Reset position
    snowball.setCenter(Vector2D(0, 0));
    
    // Reset physics
    snowball.setBallistics(Vector2D(0, 0), gravity);
    
    if (logger) {
        logger->logMessage("Snowball reset");
    }
}

bool Snowball::isActive() const {
    return active;
}

bool Snowball::hasHitTarget() const {
    return hitTarget;
}

bool Snowball::hasHitGround() const {
    return hitGround;
}

double Snowball::getRadius() const {
    return snowball.getRadius();
}

void Snowball::setTarget(const std::shared_ptr<Circle>& newTarget) {
    target = newTarget;
}

bool Snowball::checkTargetHit() {
    if (!active || hitTarget || hitGround) {
        return false;
    }
    
    if (auto sharedTarget = target.lock()) {
        if (snowball.intersects(*sharedTarget)) {
            hitTarget = true;
            active = false;
            
            if (logger) {
                logger->logSnowballHit(sharedTarget->getCenter(), true);
                logger->logMessage("TARGET HIT! Distance to center: " + 
                                  std::to_string(snowball.distanceToCenter(*sharedTarget)));
            }
            
            return true;
        }
    }
    
    return false;
}

void Snowball::enableLogging(std::shared_ptr<Logger> newLogger) {
    logger = newLogger;
}