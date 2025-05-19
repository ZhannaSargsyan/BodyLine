/**
 * @file Vector2D.h
 * @brief A 2D vector class for physics calculations
 */
#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <iostream>
#include <cmath>

// Define M_PI if not available
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Vector2D {
public:
    // Public member variables for direct access
    double x, y;
    
    // Constructors
    Vector2D();
    Vector2D(double x, double y);
    Vector2D(const Vector2D& v);
    
    // Arithmetic operators
    Vector2D operator+(const Vector2D& v) const;
    Vector2D operator-(const Vector2D& v) const;
    Vector2D operator*(double scalar) const;
    Vector2D operator/(double scalar) const;
    
    // Compound assignment operators
    Vector2D& operator+=(const Vector2D& v);
    Vector2D& operator-=(const Vector2D& v);
    Vector2D& operator*=(double scalar);
    Vector2D& operator/=(double scalar);
    
    // Comparison operators
    bool operator==(const Vector2D& v) const;
    bool operator!=(const Vector2D& v) const;
    
    // Vector operations
    double length() const;            // Magnitude of the vector
    double magnitude() const;         // Alias for length()
    double lengthSquared() const;     // Squared magnitude (more efficient)
    double distance(const Vector2D& v) const;       // Distance to another vector
    double distanceSquared(const Vector2D& v) const;  // Squared distance
    Vector2D normalized() const;      // Returns a normalized copy
    Vector2D normalize();             // Normalizes this vector and returns reference
    
    // Vector products
    double dot(const Vector2D& v) const;    // Dot product
    double cross(const Vector2D& v) const;  // 2D cross product (returns scalar)
    
    // Rotation and angles
    Vector2D rotate(double angle) const;    // Rotate by angle (radians)
    double angle() const;                   // Angle from origin
    double angleBetween(const Vector2D& v) const;  // Angle between vectors
};

// Stream operator for easier printing
std::ostream& operator<<(std::ostream& os, const Vector2D& v);

#endif // VECTOR2D_H