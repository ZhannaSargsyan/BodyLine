/**
 * @file WalkerStrategy.cpp
 * @brief Implementation of the WalkerStrategy class
 */
#include "../include/WalkerStrategy.h"
#include <cmath>
#include <iostream>

WalkerStrategy::WalkerStrategy(std::shared_ptr<Body> body, std::shared_ptr<Circle> target, double walkSpeed)
    : MovementStrategy(body, target), walkSpeed(walkSpeed), objectCaught(false), 
      currentMoveIndex(0), minGroundContacts(2), minObjectContacts(3) {
}

void WalkerStrategy::planSequence() {
    if (!target) {
        if (logger) logger->logMessage("Error: No target set for planning catch sequence");
        return;
    }
    planSequence(target->getCenter());
}

void WalkerStrategy::planSequence(const Vector2D& objectPosition) {
    // Clear any existing moves
    plannedMoves.clear();
    objectCaught = false;
    currentMoveIndex = 0;
    
    if (logger) {
        logger->logMessage("Planning catch sequence");
        double distance = (objectPosition - body->getBasePosition()).magnitude();
        logger->logMessage("Distance to object: " + std::to_string(distance));
    }
    
    // Plan walking to get close to the object
    addWalkingSequence(objectPosition);
    
    // Plan reaching to grab the object
    addReachingSequence(objectPosition);
    
    if (logger) {
        logger->logMessage("Total planned moves: " + std::to_string(plannedMoves.size()));
    }
}

bool WalkerStrategy::executeNextMove() {
    if (isSequenceComplete()) {
        return false;
    }
    
    bool success = false;
    Move currentMove = plannedMoves.front();
    plannedMoves.pop_front();
    
    switch (currentMove.type) {
        case Move::Type::WALK:
            success = executeWalkMove(currentMove);
            break;
        case Move::Type::REACH:
            success = executeReachMove(currentMove);
            break;
        case Move::Type::GRAB:
            // Execute grab action
            // Check if we're in position to grab
            if (body->canReachObject(*target, minObjectContacts)) {
                objectCaught = true;
                if (logger) logger->logMessage("Object caught successfully!");
                success = true;
            } else {
                if (logger) logger->logMessage("Failed to grab object");
                success = false;
            }
            break;
    }
    
    // Log progress
    if (logger) {
        logger->logMessage("Completed move " + std::to_string(++currentMoveIndex) + " of " + 
                          std::to_string(currentMoveIndex + plannedMoves.size()));
    }
    
    return success;
}

bool WalkerStrategy::isSequenceComplete() const {
    return plannedMoves.empty();
}

bool WalkerStrategy::hasObjectBeenCaught() const {
    return objectCaught;
}

void WalkerStrategy::setWalkSpeed(double speed) {
    walkSpeed = speed;
}

double WalkerStrategy::getWalkSpeed() const {
    return walkSpeed;
}

void WalkerStrategy::addWalkingSequence(const Vector2D& targetPos) {
    // Calculate number of walking steps to reach close to the target
    Vector2D startPos = body->getBasePosition();
    double distance = (targetPos - startPos).magnitude();
    double reachDistance = 50.0; // Distance at which we start reaching
    double walkingDistance = distance - reachDistance;
    
    if (walkingDistance <= 0) {
        // Already close enough, no walking needed
        return;
    }
    
    int numWalkingSteps = static_cast<int>(walkingDistance / walkSpeed);
    Vector2D stepVector = (targetPos - startPos).normalize() * walkSpeed;
    
    // Add walking moves
    for (int i = 0; i < numWalkingSteps; i++) {
        Move walkMove;
        walkMove.type = Move::Type::WALK;
        walkMove.position = startPos + stepVector * (i + 1);
        plannedMoves.push_back(walkMove);
    }
    
    if (logger) {
        logger->logMessage("Added walking sequence: " + std::to_string(numWalkingSteps) + " moves");
    }
}

void WalkerStrategy::addReachingSequence(const Vector2D& targetPos) {
    // Add reaching moves for arms
    std::vector<std::string> reachingSegments = {"left_lower_arm", "right_lower_arm", "left_hand", "right_hand"};
    int reachingSteps = 0;
    
    for (const auto& segmentName : reachingSegments) {
        Move reachMove;
        reachMove.type = Move::Type::REACH;
        reachMove.segmentName = segmentName;
        reachMove.position = targetPos;
        
        // Calculate appropriate rotation to point toward the target
        auto segment = body->getSegment(segmentName);
        if (segment) {
            Vector2D toTarget = targetPos - segment->getStart();
            double targetAngle = std::atan2(toTarget.y, toTarget.x);
            reachMove.rotationAmount = targetAngle - segment->getAngle();
            
            // Normalize rotation to be between -PI and PI
            while (reachMove.rotationAmount > M_PI) reachMove.rotationAmount -= 2 * M_PI;
            while (reachMove.rotationAmount < -M_PI) reachMove.rotationAmount += 2 * M_PI;
            
            plannedMoves.push_back(reachMove);
            reachingSteps++;
        }
    }
    
    // Finally, add a grab move
    Move grabMove;
    grabMove.type = Move::Type::GRAB;
    plannedMoves.push_back(grabMove);
    
    if (logger) {
        logger->logMessage("Added reaching sequence: " + std::to_string(reachingSteps) + " moves");
    }
}

bool WalkerStrategy::executeWalkMove(const Move& move) {
    if (!body->hasMinimumGroundContacts(minGroundContacts)) {
        if (logger) logger->logMessage("Cannot move - insufficient ground contacts");
        return false;
    }
    
    // Move the body base to the new position
    body->moveBaseTo(move.position);
    return true;
}

bool WalkerStrategy::executeReachMove(const Move& move) {
    if (!body->hasMinimumGroundContacts(minGroundContacts)) {
        if (logger) logger->logMessage("Cannot reach - insufficient ground contacts");
        return false;
    }
    
    auto segment = body->getSegment(move.segmentName);
    if (!segment) {
        if (logger) logger->logMessage("Segment not found: " + move.segmentName);
        return false;
    }
    
    // Rotate the segment toward the target
    return body->rotateSegment(move.segmentName, move.rotationAmount);
}