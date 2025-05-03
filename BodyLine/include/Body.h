/**
 * @file Body.h
 * @brief Defines the anthropomorphic body composed of connected segments
 */
#ifndef BODY_H
#define BODY_H

#include "Segment.h"  // Already includes M_PI definition
#include "Circle.h"
#include <vector>
#include <memory>
#include <map>
#include <string>

// Forward declarations
class Simulation;

class Body {
public:
    // Initialize body with its base position
    Body(const Vector2D& basePosition, double groundLevel);
    virtual ~Body() = default;
    
    // Add and access segments
    void addSegment(const std::string& name, double length, double angle, 
                   double minAngle = -M_PI, double maxAngle = M_PI);
    void connectSegment(const std::string& parentName, const std::string& childName);
    Segment* getSegment(const std::string& name);
    const Segment* getSegment(const std::string& name) const;
    
    // Getters
    const Vector2D& getBasePosition() const;
    double getGroundLevel() const;
    std::vector<std::string> getSegmentNames() const;
    size_t getSegmentCount() const;
    
    // Body movement and constraints
    bool rotateSegment(const std::string& name, double deltaAngle);
    bool rotateSegmentTo(const std::string& name, double targetAngle);
    void moveBaseTo(const Vector2D& newBase);
    
    // Ground contact checks
    bool hasMinimumGroundContacts(int minContacts = 2) const;
    int countGroundContacts() const;
    std::vector<std::string> getSegmentsContactingGround() const;
    
    // Object interaction
    bool canReachObject(const Circle& object, int minTouchingPoints = 3) const;
    std::vector<std::string> getSegmentsTouchingObject(const Circle& object) const;
    
    // Update segments after position changes
    void updateSegments();
    
    // For visualization
    std::vector<std::pair<Vector2D, Vector2D>> getSegmentLines() const;
    
    // Check if a segment is an endpoint (not connected to any children)
    bool isEndPoint(const std::string& segmentName) const;
    
protected:
    Vector2D basePosition;               // Base position of the body
    double groundLevel;                  // Ground level (y-coordinate)
    
    std::map<std::string, std::unique_ptr<Segment>> segments;  // Named segments
    std::map<std::string, std::vector<std::string>> connections;  // Parent-to-children connections
    
    // Helper methods
    void updateChildSegments(const std::string& parentName);
};

#endif // BODY_H
