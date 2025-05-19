/**
 * @file Walker.cpp
 * @brief Implementation of the Walker class
 */
#include "../include/Walker.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Walker::Walker(const std::shared_ptr<Body>& body, double walkSpeed)
    : body(body),
      currentMoveIndex(0),
      walkSpeed(walkSpeed),
      objectCaught(false) {
}

bool Walker::update(double timeStep) {
    // If sequence is complete or no sequence is planned, nothing to do
    if (isSequenceComplete() || sequence.empty()) {
        return false;
    }
    
    // Execute next move in sequence
    return executeNextMove();
}

void Walker::planCatchSequence(const Vector2D& objectPosition) {
    // Clear any existing sequence
    sequence.clear();
    currentMoveIndex = 0;
    objectCaught = false;
    
    if (logger) {
        logger->logMessage("Planning catch sequence");
        logger->logMessage("Distance to object: " + std::to_string(body->getBasePosition().distance(objectPosition)));
    }
    
    // First, plan walking to get close to the object
    addWalkingSequence(Vector2D(objectPosition.x - 50, body->getBasePosition().y));
    
    // Then, plan reaching to catch the object
    addReachingSequence(objectPosition);
    
    if (logger) {
        logger->logMessage("Total planned moves: " + std::to_string(sequence.size()));
    }
}

void Walker::planThrowSequence(const Vector2D& targetPosition) {
    // Clear any existing sequence
    sequence.clear();
    currentMoveIndex = 0;
    objectCaught = false;
    
    if (logger) {
        logger->logMessage("Planning throw sequence");
        logger->logMessage("Distance to target: " + std::to_string(body->getBasePosition().distance(targetPosition)));
    }
    
    // Plan a throwing motion aimed at the target
    addThrowingSequence(targetPosition);
    
    if (logger) {
        logger->logMessage("Total planned moves: " + std::to_string(sequence.size()));
    }
}

bool Walker::executeNextMove() {
    if (isSequenceComplete() || sequence.empty()) {
        return false;
    }
    
    const SequenceMove& move = sequence[currentMoveIndex];
    bool moveComplete = false;
    
    // Execute different types of moves based on the type
    switch (move.type) {
        case MoveType::WALK_FORWARD:
            moveComplete = executeWalkForward(move.parameter);
            break;
            
        case MoveType::WALK_BACKWARD:
            moveComplete = executeWalkBackward(move.parameter);
            break;
            
        case MoveType::REACH_UP:
            moveComplete = executeReachUp(move.segmentName, move.parameter);
            break;
            
        case MoveType::REACH_DOWN:
            moveComplete = executeReachDown(move.segmentName, move.parameter);
            break;
            
        case MoveType::REACH_LEFT:
            moveComplete = executeReachLeft(move.segmentName, move.parameter);
            break;
            
        case MoveType::REACH_RIGHT:
            moveComplete = executeReachRight(move.segmentName, move.parameter);
            break;
            
        case MoveType::RESET_POSE:
            moveComplete = executeResetPose();
            break;
    }
    
    // If move is complete, advance to the next move
    if (moveComplete) {
        currentMoveIndex++;
        
        if (logger) {
            logger->logMessage("Completed move " + std::to_string(currentMoveIndex) + 
                              " of " + std::to_string(sequence.size()));
        }
        
        // Check if we've just completed a reaching sequence and should check for caught object
        if (isSequenceComplete() && !objectCaught) {
            // Check if we've caught the target object
            if (auto target = targetObject.lock()) {
                if (body->canReachObject(*target)) {
                    objectCaught = true;
                    
                    if (logger) {
                        logger->logMessage("TARGET CAUGHT!");
                    }
                }
            }
        }
    }
    
    return moveComplete;
}

void Walker::resetSequence() {
    currentMoveIndex = 0;
    objectCaught = false;
    
    if (logger) {
        logger->logMessage("Sequence reset");
    }
}

const std::vector<SequenceMove>& Walker::getSequence() const {
    return sequence;
}

int Walker::getCurrentMoveIndex() const {
    return currentMoveIndex;
}

bool Walker::isSequenceComplete() const {
    return currentMoveIndex >= static_cast<int>(sequence.size());
}

bool Walker::hasObjectBeenCaught() const {
    return objectCaught;
}

void Walker::setTargetObject(const std::shared_ptr<Circle>& object) {
    targetObject = object;
}

void Walker::enableLogging(std::shared_ptr<Logger> newLogger) {
    logger = newLogger;
}

void Walker::addWalkingSequence(const Vector2D& destination) {
    // Calculate how far to walk
    double distance = destination.x - body->getBasePosition().x;
    
    // Determine the move type
    MoveType moveType = (distance >= 0) ? MoveType::WALK_FORWARD : MoveType::WALK_BACKWARD;
    distance = std::abs(distance);
    
    // Create walking moves with small increments
    double stepSize = walkSpeed;
    int numSteps = static_cast<int>(std::ceil(distance / stepSize));
    
    for (int i = 0; i < numSteps; i++) {
        // For the last step, use the remaining distance
        double stepDistance = (i == numSteps - 1) ? 
            (distance - (i * stepSize)) : stepSize;
        
        SequenceMove move = {
            moveType,
            stepDistance,
            ""  // No specific segment for walking
        };
        
        sequence.push_back(move);
    }
    
    if (logger) {
        logger->logMessage("Added walking sequence: " + std::to_string(numSteps) + " moves");
    }
}

void Walker::addReachingSequence(const Vector2D& objectPosition) {
    // Get body segments that might interact with the object
    std::vector<std::string> segments = body->getSegmentNames();
    
    // Filter to only end segments that can potentially reach
    std::vector<std::string> endSegments;
    for (const auto& name : segments) {
        if (body->isEndPoint(name)) {
            endSegments.push_back(name);
        }
    }
    
    // Choose the main segments to use for reaching (typically arms)
    // In a real implementation, we might have labeled segments like "left_arm", "right_arm"
    // Here, we'll just pick some end segments
    std::vector<std::string> reachingSegments;
    if (!endSegments.empty()) {
        // Use the first few end segments for simplicity
        size_t numToUse = std::min(size_t(2), endSegments.size());
        for (size_t i = 0; i < numToUse; i++) {
            reachingSegments.push_back(endSegments[i]);
        }
    }
    
    // Create a simple reaching sequence using the selected segments
    for (const auto& segmentName : reachingSegments) {
        // Reach up if object is higher than body base
        if (objectPosition.y < body->getBasePosition().y) {
            SequenceMove moveUp = {
                MoveType::REACH_UP,
                0.2,  // Small angle increment
                segmentName
            };
            sequence.push_back(moveUp);
        }
        
        // Reach forward (right) if object is to the right
        if (objectPosition.x > body->getBasePosition().x) {
            SequenceMove moveRight = {
                MoveType::REACH_RIGHT,
                0.2,  // Small angle increment
                segmentName
            };
            sequence.push_back(moveRight);
        }
        // Reach backward (left) if object is to the left
        else {
            SequenceMove moveLeft = {
                MoveType::REACH_LEFT,
                0.2,  // Small angle increment
                segmentName
            };
            sequence.push_back(moveLeft);
        }
    }
    
    if (logger) {
        logger->logMessage("Added reaching sequence: " + std::to_string(reachingSegments.size() * 2) + " moves");
    }
}

void Walker::addThrowingSequence(const Vector2D& targetPosition) {
    // Similar to reaching, but with the intent to throw
    std::vector<std::string> segments = body->getSegmentNames();
    
    // Find arm-like segments to use for throwing
    std::vector<std::string> throwingSegments;
    for (const auto& name : segments) {
        if (body->isEndPoint(name)) {
            throwingSegments.push_back(name);
            break; // Just use one arm for simplicity
        }
    }
    
    if (throwingSegments.empty()) {
        if (logger) {
            logger->logMessage("No suitable segments found for throwing");
        }
        return;
    }
    
    const std::string& armSegment = throwingSegments[0];
    
    // Wind up (move arm back)
    SequenceMove windUp = {
        MoveType::REACH_LEFT,
        0.6,  // Larger angle for wind-up
        armSegment
    };
    sequence.push_back(windUp);
    
    // Throw (move arm forward quickly)
    SequenceMove throwMove = {
        MoveType::REACH_RIGHT,
        1.2,  // Large angle for throwing motion
        armSegment
    };
    sequence.push_back(throwMove);
    
    // Follow through
    SequenceMove followThrough = {
        MoveType::REACH_RIGHT,
        0.3,
        armSegment
    };
    sequence.push_back(followThrough);
    
    // Reset arm position
    SequenceMove reset = {
        MoveType::RESET_POSE,
        0.0,
        ""
    };
    sequence.push_back(reset);
    
    if (logger) {
        logger->logMessage("Added throwing sequence: 4 moves");
    }
}

bool Walker::executeWalkForward(double distance) {
    // Move the body forward
    Vector2D currentPos = body->getBasePosition();
    Vector2D newPos = currentPos + Vector2D(distance, 0);
    body->moveBaseTo(newPos);
    
    // Always complete the move in one step for simplicity
    return true;
}

bool Walker::executeWalkBackward(double distance) {
    // Move the body backward
    Vector2D currentPos = body->getBasePosition();
    Vector2D newPos = currentPos - Vector2D(distance, 0);
    body->moveBaseTo(newPos);
    
    // Always complete the move in one step for simplicity
    return true;
}

bool Walker::executeReachUp(const std::string& segmentName, double angle) {
    if (segmentName.empty()) {
        return true; // Skip if no segment specified
    }
    
    // Rotate the segment upward (negative y is up, so rotation is negative)
    Segment* segment = body->getSegment(segmentName);
    if (segment) {
        double currentAngle = segment->getAngle();
        return body->rotateSegmentTo(segmentName, currentAngle - angle);
    }
    
    return true; // Complete the move if segment doesn't exist
}

bool Walker::executeReachDown(const std::string& segmentName, double angle) {
    if (segmentName.empty()) {
        return true; // Skip if no segment specified
    }
    
    // Rotate the segment downward (positive y is down, so rotation is positive)
    Segment* segment = body->getSegment(segmentName);
    if (segment) {
        double currentAngle = segment->getAngle();
        return body->rotateSegmentTo(segmentName, currentAngle + angle);
    }
    
    return true; // Complete the move if segment doesn't exist
}

bool Walker::executeReachLeft(const std::string& segmentName, double angle) {
    if (segmentName.empty()) {
        return true; // Skip if no segment specified
    }
    
    // Rotate the segment left (negative x is left, target angle is PI)
    Segment* segment = body->getSegment(segmentName);
    if (segment) {
        double currentAngle = segment->getAngle();
        return body->rotateSegmentTo(segmentName, currentAngle + angle);
    }
    
    return true; // Complete the move if segment doesn't exist
}

bool Walker::executeReachRight(const std::string& segmentName, double angle) {
    if (segmentName.empty()) {
        return true; // Skip if no segment specified
    }
    
    // Rotate the segment right (positive x is right, target angle is 0)
    Segment* segment = body->getSegment(segmentName);
    if (segment) {
        double currentAngle = segment->getAngle();
        return body->rotateSegmentTo(segmentName, currentAngle - angle);
    }
    
    return true; // Complete the move if segment doesn't exist
}

bool Walker::executeResetPose() {
    // Reset all segments to their default angles
    std::vector<std::string> segments = body->getSegmentNames();
    
    for (const auto& name : segments) {
        // Reset to a neutral position (0 degrees)
        body->rotateSegmentTo(name, 0.0);
    }
    
    // Always complete in one step for simplicity
    return true;
}