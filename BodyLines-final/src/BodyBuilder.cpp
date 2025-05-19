/**
 * @file BodyBuilder.cpp
 * @brief Implementation of the BodyBuilder class
 */
#include "../include/BodyBuilder.h"
#include <iostream>

BodyBuilder::BodyBuilder() 
    : basePosition(100.0, 400.0), groundLevel(400.0) {
}

BodyBuilder& BodyBuilder::setBasePosition(const Vector2D& position) {
    basePosition = position;
    return *this;
}

BodyBuilder& BodyBuilder::setGroundLevel(double level) {
    groundLevel = level;
    return *this;
}

BodyBuilder& BodyBuilder::addSegment(const std::string& name, double length, double angle, 
                                    double minAngle, double maxAngle) {
    SegmentSpec spec = {length, angle, minAngle, maxAngle};
    segmentSpecs[name] = spec;
    return *this;
}

BodyBuilder& BodyBuilder::connectSegments(const std::string& parentName, const std::string& childName) {
    Connection conn = {parentName, childName};
    connections.push_back(conn);
    return *this;
}

BodyBuilder& BodyBuilder::buildHumanoidBody() {
    // Reset any existing specifications
    reset();
    
    // Torso
    addSegment("torso", 60.0, -M_PI/2, -M_PI, M_PI);
    
    // Head
    addSegment("head", 30.0, -M_PI/2, -M_PI/4, M_PI/4);
    connectSegments("torso", "head");
    
    // Left arm
    addSegment("left_upper_arm", 40.0, -M_PI, -M_PI, 0);
    connectSegments("torso", "left_upper_arm");
    addSegment("left_lower_arm", 40.0, -M_PI, -M_PI, 0);
    connectSegments("left_upper_arm", "left_lower_arm");
    addSegment("left_hand", 20.0, -M_PI, -M_PI/2, M_PI/2);
    connectSegments("left_lower_arm", "left_hand");
    
    // Right arm
    addSegment("right_upper_arm", 40.0, 0.0, 0, M_PI);
    connectSegments("torso", "right_upper_arm");
    addSegment("right_lower_arm", 40.0, 0.0, 0, M_PI);
    connectSegments("right_upper_arm", "right_lower_arm");
    addSegment("right_hand", 20.0, 0.0, -M_PI/2, M_PI/2);
    connectSegments("right_lower_arm", "right_hand");
    
    // Left leg
    addSegment("left_upper_leg", 50.0, M_PI/2, 0, M_PI);
    connectSegments("torso", "left_upper_leg");
    addSegment("left_lower_leg", 50.0, M_PI/2, 0, M_PI);
    connectSegments("left_upper_leg", "left_lower_leg");
    addSegment("left_foot", 30.0, 0.0, -M_PI/4, M_PI/4);
    connectSegments("left_lower_leg", "left_foot");
    
    // Right leg
    addSegment("right_upper_leg", 50.0, M_PI/2, 0, M_PI);
    connectSegments("torso", "right_upper_leg");
    addSegment("right_lower_leg", 50.0, M_PI/2, 0, M_PI);
    connectSegments("right_upper_leg", "right_lower_leg");
    addSegment("right_foot", 30.0, 0.0, -M_PI/4, M_PI/4);
    connectSegments("right_lower_leg", "right_foot");
    
    return *this;
}

BodyBuilder& BodyBuilder::buildSimpleBody() {
    // Reset any existing specifications
    reset();
    
    // Create a simple 3-segment body
    addSegment("torso", 50.0, -M_PI/2, -M_PI, M_PI);
    addSegment("left_arm", 40.0, -3*M_PI/4, -M_PI, 0);
    connectSegments("torso", "left_arm");
    addSegment("right_arm", 40.0, -M_PI/4, 0, M_PI);
    connectSegments("torso", "right_arm");
    addSegment("left_leg", 50.0, M_PI/2, 0, M_PI);
    connectSegments("torso", "left_leg");
    addSegment("right_leg", 50.0, M_PI/2, 0, M_PI);
    connectSegments("torso", "right_leg");
    
    return *this;
}

std::shared_ptr<Body> BodyBuilder::build() {
    auto body = std::make_shared<Body>(basePosition, groundLevel);
    
    // Add all segments
    for (const auto& pair : segmentSpecs) {
        const std::string& name = pair.first;
        const SegmentSpec& spec = pair.second;
        
        body->addSegment(name, spec.length, spec.angle, spec.minAngle, spec.maxAngle);
    }
    
    // Add all connections
    for (const auto& conn : connections) {
        body->connectSegment(conn.parent, conn.child);
    }
    
    // Update all segments to ensure proper positioning
    body->updateSegments();
    
    return body;
}

void BodyBuilder::reset() {
    segmentSpecs.clear();
    connections.clear();
    
    // Keep default base position and ground level
}