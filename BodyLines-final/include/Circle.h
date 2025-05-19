/**
 * @file Circle.h
 * @brief Defines a Circle class for circular objects in the simulation
 */
#ifndef CIRCLE_H
#define CIRCLE_H

#include "Vector2D.h"

class Circle {
public:
    // Constructors
    Circle(const Vector2D& center = Vector2D(0, 0), double radius = 10.0);
    Circle(double x, double y, double radius);
    
    // Getters
    Vector2D getCenter() const;
    double getRadius() const;
    double getArea() const;
    double getCircumference() const;
    
    // Setters
    void setCenter(const Vector2D& center);
    void setCenter(double x, double y);
    void setRadius(double radius);
    
    // Movement
    void move(const Vector2D& displacement);
    void updatePosition(double timeStep);
    
    // Physics
    void setBallistics(const Vector2D& initialVelocity, double gravity);
    Vector2D getVelocity() const;
    
    // Collision detection
    bool contains(const Vector2D& point) const;
    bool intersects(const Circle& other) const;
    bool isOnGround(double groundLevel) const;
    
    // Math utilities
    double distanceTo(const Circle& other) const;
    double distanceToCenter(const Circle& other) const;
    
private:
    Vector2D center;          // Center position
    double radius;            // Radius
    
    // Physics properties (for ballistics)
    Vector2D velocity;        // Current velocity
    double gravity;           // Gravity value (for ballistics)
    bool hasPhysics;          // Whether this circle has physics enabled
};

#endif // CIRCLE_H