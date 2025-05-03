/**
 * @file Circle.cpp
 * @brief Implementation of the Circle class
 */
#include "../include/Circle.h"
#include <cmath>

Circle::Circle(const Vector2D& center, double radius) 
    : center(center), 
      radius(radius),
      velocity(0, 0),
      gravity(0),
      hasPhysics(false) {
}

Circle::Circle(double x, double y, double radius)
    : center(x, y), 
      radius(radius),
      velocity(0, 0),
      gravity(0),
      hasPhysics(false) {
}

Vector2D Circle::getCenter() const {
    return center;
}

double Circle::getRadius() const {
    return radius;
}

double Circle::getArea() const {
    return M_PI * radius * radius;
}

double Circle::getCircumference() const {
    return 2 * M_PI * radius;
}

void Circle::setCenter(const Vector2D& newCenter) {
    center = newCenter;
}

void Circle::setCenter(double x, double y) {
    center.x = x;
    center.y = y;
}

void Circle::setRadius(double newRadius) {
    radius = std::max(0.0, newRadius);  // Ensure radius is non-negative
}

void Circle::move(const Vector2D& displacement) {
    center += displacement;
}

void Circle::updatePosition(double timeStep) {
    if (!hasPhysics) {
        return;  // No physics to simulate
    }
    
    // Update position based on velocity
    center += velocity * timeStep;
    
    // Update velocity based on gravity (only affects y component)
    velocity.y += gravity * timeStep;
}

void Circle::setBallistics(const Vector2D& initialVelocity, double gravityValue) {
    velocity = initialVelocity;
    gravity = gravityValue;
    hasPhysics = true;
}

Vector2D Circle::getVelocity() const {
    return velocity;
}

bool Circle::contains(const Vector2D& point) const {
    return center.distanceSquared(point) <= radius * radius;
}

bool Circle::intersects(const Circle& other) const {
    double distanceSquared = center.distanceSquared(other.center);
    double sumRadii = radius + other.radius;
    return distanceSquared <= sumRadii * sumRadii;
}

bool Circle::isOnGround(double groundLevel) const {
    return center.y + radius >= groundLevel;
}

double Circle::distanceTo(const Circle& other) const {
    double centerDistance = center.distance(other.center);
    return std::max(0.0, centerDistance - radius - other.radius);
}

double Circle::distanceToCenter(const Circle& other) const {
    return center.distance(other.center);
}