/**
 * @file Body.cpp
 * @brief Implementation of the Body class
 */
#include "../include/Body.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

Body::Body(const Vector2D& basePosition, double groundLevel)
    : basePosition(basePosition), groundLevel(groundLevel) {
    
    // Create a default articulated body with a humanoid-like structure
    
    // Torso
    addSegment("torso", 60.0, -M_PI/2, -M_PI, M_PI);
    
    // Head
    addSegment("head", 30.0, -M_PI/2, -M_PI/4, M_PI/4);
    connectSegment("torso", "head");
    
    // Left arm
    addSegment("left_upper_arm", 40.0, -M_PI, -M_PI, 0);
    connectSegment("torso", "left_upper_arm");
    addSegment("left_lower_arm", 40.0, -M_PI, -M_PI, 0);
    connectSegment("left_upper_arm", "left_lower_arm");
    addSegment("left_hand", 20.0, -M_PI, -M_PI/2, M_PI/2);
    connectSegment("left_lower_arm", "left_hand");
    
    // Right arm
    addSegment("right_upper_arm", 40.0, 0.0, 0, M_PI);
    connectSegment("torso", "right_upper_arm");
    addSegment("right_lower_arm", 40.0, 0.0, 0, M_PI);
    connectSegment("right_upper_arm", "right_lower_arm");
    addSegment("right_hand", 20.0, 0.0, -M_PI/2, M_PI/2);
    connectSegment("right_lower_arm", "right_hand");
    
    // Left leg
    addSegment("left_upper_leg", 50.0, M_PI/2, 0, M_PI);
    connectSegment("torso", "left_upper_leg");
    addSegment("left_lower_leg", 50.0, M_PI/2, 0, M_PI);
    connectSegment("left_upper_leg", "left_lower_leg");
    addSegment("left_foot", 30.0, 0.0, -M_PI/4, M_PI/4);
    connectSegment("left_lower_leg", "left_foot");
    
    // Right leg
    addSegment("right_upper_leg", 50.0, M_PI/2, 0, M_PI);
    connectSegment("torso", "right_upper_leg");
    addSegment("right_lower_leg", 50.0, M_PI/2, 0, M_PI);
    connectSegment("right_upper_leg", "right_lower_leg");
    addSegment("right_foot", 30.0, 0.0, -M_PI/4, M_PI/4);
    connectSegment("right_lower_leg", "right_foot");
    
    // Update all segments to ensure proper positioning
    updateSegments();
}

void Body::addSegment(const std::string& name, double length, double angle, 
                     double minAngle, double maxAngle) {
    if (segments.find(name) != segments.end()) {
        std::cerr << "Segment '" << name << "' already exists!" << std::endl;
        return;
    }
    
    // Create segment starting at base position
    auto segment = std::make_unique<Segment>(name, basePosition, length, angle, minAngle, maxAngle);
    
    // Add to segments map
    segments[name] = std::move(segment);
}

void Body::connectSegment(const std::string& parentName, const std::string& childName) {
    // Ensure both segments exist
    if (segments.find(parentName) == segments.end() || 
        segments.find(childName) == segments.end()) {
        std::cerr << "Cannot connect: one or both segments don't exist!" << std::endl;
        return;
    }
    
    // Add child to parent's connections
    connections[parentName].push_back(childName);
    
    // Update child segment to start from parent's end
    const auto& parentEnd = segments[parentName]->getEnd();
    segments[childName]->setStart(parentEnd);
}

Segment* Body::getSegment(const std::string& name) {
    auto it = segments.find(name);
    return (it != segments.end()) ? it->second.get() : nullptr;
}

const Segment* Body::getSegment(const std::string& name) const {
    auto it = segments.find(name);
    return (it != segments.end()) ? it->second.get() : nullptr;
}

const Vector2D& Body::getBasePosition() const {
    return basePosition;
}

double Body::getGroundLevel() const {
    return groundLevel;
}

std::vector<std::string> Body::getSegmentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : segments) {
        names.push_back(pair.first);
    }
    return names;
}

size_t Body::getSegmentCount() const {
    return segments.size();
}

bool Body::rotateSegment(const std::string& name, double deltaAngle) {
    auto segment = getSegment(name);
    if (!segment) {
        std::cerr << "Segment '" << name << "' not found!" << std::endl;
        return false;
    }
    
    bool success = segment->rotate(deltaAngle);
    if (success) {
        // Update all child segments to maintain connections
        updateChildSegments(name);
    }
    return success;
}

bool Body::rotateSegmentTo(const std::string& name, double targetAngle) {
    auto segment = getSegment(name);
    if (!segment) {
        std::cerr << "Segment '" << name << "' not found!" << std::endl;
        return false;
    }
    
    bool success = segment->rotateTo(targetAngle);
    if (success) {
        // Update all child segments to maintain connections
        updateChildSegments(name);
    }
    return success;
}

void Body::moveBaseTo(const Vector2D& newBase) {
    // Calculate the displacement vector
    Vector2D displacement = newBase - basePosition;
    
    // Update base position
    basePosition = newBase;
    
    // Move all segments that are directly connected to the base
    for (auto& pair : segments) {
        const std::string& name = pair.first;
        Segment* segment = pair.second.get();
        
        // Check if this is a "root" segment (no parent)
        bool isRoot = true;
        for (const auto& conn : connections) {
            for (const auto& child : conn.second) {
                if (child == name) {
                    isRoot = false;
                    break;
                }
            }
            if (!isRoot) break;
        }
        
        if (isRoot) {
            // Move the segment's start point
            segment->setStart(segment->getStart() + displacement);
            // Update its children recursively
            updateChildSegments(name);
        }
    }
}

bool Body::hasMinimumGroundContacts(int minContacts) const {
    return countGroundContacts() >= minContacts;
}

int Body::countGroundContacts() const {
    int count = 0;
    
    // Check all segments' endpoints
    for (const auto& pair : segments) {
        const Segment* segment = pair.second.get();
        
        if (segment->isStartContactingGround(groundLevel)) {
            count++;
        }
        
        if (segment->isEndContactingGround(groundLevel)) {
            count++;
        }
    }
    
    return count;
}

std::vector<std::string> Body::getSegmentsContactingGround() const {
    std::vector<std::string> contactingSegments;
    
    for (const auto& pair : segments) {
        const std::string& name = pair.first;
        const Segment* segment = pair.second.get();
        
        if (segment->isStartContactingGround(groundLevel) || 
            segment->isEndContactingGround(groundLevel)) {
            contactingSegments.push_back(name);
        }
    }
    
    return contactingSegments;
}

bool Body::canReachObject(const Circle& object, int minTouchingPoints) const {
    return getSegmentsTouchingObject(object).size() >= static_cast<size_t>(minTouchingPoints);
}

std::vector<std::string> Body::getSegmentsTouchingObject(const Circle& object) const {
    std::vector<std::string> touchingSegments;
    
    for (const auto& pair : segments) {
        const std::string& name = pair.first;
        const Segment* segment = pair.second.get();
        
        // Check if the segment is an endpoint (not parent to any other segments)
        if (isEndPoint(name)) {
            // Check if either endpoint is within the circle
            Vector2D end = segment->getEnd();
            if (object.contains(end) || segment->distanceToPoint(object.getCenter()) <= object.getRadius()) {
                touchingSegments.push_back(name);
            }
        }
    }
    
    return touchingSegments;
}

void Body::updateSegments() {
    // Start by finding the "root" segments (not children of any other segment)
    std::vector<std::string> rootSegments;
    
    for (const auto& pair : segments) {
        const std::string& name = pair.first;
        
        // Check if this segment is a child of any other segment
        bool isChild = false;
        for (const auto& conn : connections) {
            if (std::find(conn.second.begin(), conn.second.end(), name) != conn.second.end()) {
                isChild = true;
                break;
            }
        }
        
        if (!isChild) {
            rootSegments.push_back(name);
        }
    }
    
    // Update all segments starting from the root segments
    for (const auto& rootName : rootSegments) {
        segments[rootName]->setStart(basePosition);
        updateChildSegments(rootName);
    }
}

std::vector<std::pair<Vector2D, Vector2D>> Body::getSegmentLines() const {
    std::vector<std::pair<Vector2D, Vector2D>> lines;
    
    for (const auto& pair : segments) {
        const Segment* segment = pair.second.get();
        lines.emplace_back(segment->getStart(), segment->getEnd());
    }
    
    return lines;
}

void Body::updateChildSegments(const std::string& parentName) {
    const auto& parentConnections = connections.find(parentName);
    if (parentConnections == connections.end()) {
        return; // No children
    }
    
    const Segment* parentSegment = getSegment(parentName);
    if (!parentSegment) {
        return; // Parent segment not found
    }
    
    // Update all children of this segment
    for (const auto& childName : parentConnections->second) {
        Segment* childSegment = getSegment(childName);
        if (childSegment) {
            // Connect the child's start to the parent's end
            childSegment->setStart(parentSegment->getEnd());
            
            // Recursively update this child's children
            updateChildSegments(childName);
        }
    }
}

bool Body::isEndPoint(const std::string& segmentName) const {
    // A segment is an endpoint if it's not a parent to any other segment
    return connections.find(segmentName) == connections.end();
}
