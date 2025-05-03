/**
 * @file Segment.cpp
 * @brief Implementation of the Segment class
 */
#include "../include/Segment.h"
#include <cmath>
#include <algorithm>

Segment::Segment(const std::string& id, const Vector2D& start, double length, double angle,
                 double minAngle, double maxAngle)
    : id(id),
      start(start),
      length(std::max(0.1, length)),  // Ensure a minimum length
      angle(clampAngle(angle)),
      minAngle(minAngle),
      maxAngle(maxAngle) {
}

std::string Segment::getId() const {
    return id;
}

Vector2D Segment::getStart() const {
    return start;
}

Vector2D Segment::getEnd() const {
    // Calculate end point based on start, length, and angle
    return Vector2D(
        start.x + length * std::cos(angle),
        start.y + length * std::sin(angle)
    );
}

double Segment::getLength() const {
    return length;
}

double Segment::getAngle() const {
    return angle;
}

double Segment::getMinAngle() const {
    return minAngle;
}

double Segment::getMaxAngle() const {
    return maxAngle;
}

void Segment::setStart(const Vector2D& newStart) {
    start = newStart;
}

void Segment::setAngle(double newAngle) {
    angle = clampAngle(newAngle);
}

void Segment::setAngleLimits(double newMin, double newMax) {
    // Ensure min is less than max
    if (newMin <= newMax) {
        minAngle = newMin;
        maxAngle = newMax;
        // Re-clamp current angle to ensure it's within new limits
        angle = clampAngle(angle);
    }
}

bool Segment::rotate(double deltaAngle) {
    double targetAngle = angle + deltaAngle;
    return rotateTo(targetAngle);
}

bool Segment::rotateTo(double targetAngle) {
    double clampedAngle = clampAngle(targetAngle);
    
    // Check if we had to clamp the angle
    bool wasConstrained = (clampedAngle != targetAngle);
    
    // Set the new angle
    angle = clampedAngle;
    
    return !wasConstrained; // Return true if we didn't have to constrain
}

void Segment::move(const Vector2D& displacement) {
    start += displacement;
}

void Segment::connectTo(std::shared_ptr<Segment> parentSegment) {
    parent = parentSegment;
    
    if (auto sharedParent = parent.lock()) {
        // Set start point to parent's end point
        start = sharedParent->getEnd();
    }
}

void Segment::updateConnectedSegments() {
    // If we have a parent, update our start position
    if (auto sharedParent = parent.lock()) {
        start = sharedParent->getEnd();
    }
}

bool Segment::containsPoint(const Vector2D& point, double threshold) const {
    return distanceToPoint(point) <= threshold;
}

Vector2D Segment::closestPointTo(const Vector2D& point) const {
    Vector2D segmentStart = start;
    Vector2D segmentEnd = getEnd();
    Vector2D segmentVec = segmentEnd - segmentStart;
    Vector2D pointVec = point - segmentStart;
    
    // Calculate projection of point onto segment
    double projection = pointVec.dot(segmentVec) / segmentVec.lengthSquared();
    
    // Clamp to segment
    projection = std::max(0.0, std::min(1.0, projection));
    
    // Calculate the closest point
    return segmentStart + segmentVec * projection;
}

double Segment::distanceToPoint(const Vector2D& point) const {
    Vector2D closestPoint = closestPointTo(point);
    return point.distance(closestPoint);
}

bool Segment::isStartContactingGround(double groundLevel, double threshold) const {
    // Check if the start point is close to the ground level
    return std::abs(start.y - groundLevel) <= threshold;
}

bool Segment::isEndContactingGround(double groundLevel, double threshold) const {
    // Check if the end point is close to the ground level
    Vector2D end = getEnd();
    return std::abs(end.y - groundLevel) <= threshold;
}

double Segment::clampAngle(double angleToClamp) const {
    // First, normalize angle to be within [0, 2π)
    double normalizedAngle = std::fmod(angleToClamp, 2 * M_PI);
    if (normalizedAngle < 0) {
        normalizedAngle += 2 * M_PI;
    }
    
    // Check if the angle is within the allowed range
    if (minAngle <= maxAngle) {
        // Simple case: range doesn't cross 0
        return std::min(std::max(normalizedAngle, minAngle), maxAngle);
    } else {
        // Complex case: range crosses 0 (e.g., min=270°, max=90°)
        if (normalizedAngle >= minAngle || normalizedAngle <= maxAngle) {
            return normalizedAngle;  // Already in range
        } else {
            // Find the closest bound
            double distToMin = std::min(std::abs(normalizedAngle - minAngle), 
                                       std::abs(normalizedAngle - (minAngle - 2 * M_PI)));
            double distToMax = std::min(std::abs(normalizedAngle - maxAngle),
                                       std::abs(normalizedAngle - (maxAngle + 2 * M_PI)));
            
            return (distToMin <= distToMax) ? minAngle : maxAngle;
        }
    }
}