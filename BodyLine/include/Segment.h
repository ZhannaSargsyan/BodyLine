/**
 * @file Segment.h
 * @brief Defines a line segment with rotation capabilities for body articulation
 */
#ifndef SEGMENT_H
#define SEGMENT_H

#include "Vector2D.h"
#include <string>
#include <memory>
#include <cmath>   // For M_PI

// Define M_PI if not available
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Segment {
public:
    // Constructor
    Segment(const std::string& id, const Vector2D& start, double length, double angle,
            double minAngle = 0.0, double maxAngle = 2 * M_PI);
    
    // Getters
    std::string getId() const;
    Vector2D getStart() const;
    Vector2D getEnd() const;
    double getLength() const;
    double getAngle() const;
    double getMinAngle() const;
    double getMaxAngle() const;
    
    // Setters
    void setStart(const Vector2D& newStart);
    void setAngle(double newAngle);
    void setAngleLimits(double newMin, double newMax);
    
    // Movement
    bool rotate(double deltaAngle);
    bool rotateTo(double targetAngle);
    void move(const Vector2D& displacement);
    
    // Connected segments
    void connectTo(std::shared_ptr<Segment> parent);
    void updateConnectedSegments();
    
    // Check if point is on or near segment
    bool containsPoint(const Vector2D& point, double threshold = 1.0) const;
    
    // Calculate closest point on segment to a given point
    Vector2D closestPointTo(const Vector2D& point) const;
    
    // Calculate distance from a point to this segment
    double distanceToPoint(const Vector2D& point) const;
    
    // Ground contact detection
    bool isStartContactingGround(double groundLevel, double threshold = 1.0) const;
    bool isEndContactingGround(double groundLevel, double threshold = 1.0) const;

private:
    std::string id;                  // Segment identifier
    Vector2D start;                  // Start point
    double length;                   // Length of segment
    double angle;                    // Current angle in radians
    double minAngle;                 // Minimum allowed angle
    double maxAngle;                 // Maximum allowed angle
    std::weak_ptr<Segment> parent;   // Parent segment (if any)
    
    // Helper for angle constraints
    double clampAngle(double angle) const;
};

#endif // SEGMENT_H