/**
 * @file Walker.h
 * @brief Defines a Walker that controls a Body to walk and catch objects
 */
#ifndef WALKER_H
#define WALKER_H

#include "Body.h"
#include "Circle.h"
#include "Logger.h"
#include <memory>
#include <vector>
#include <functional>

// Sequence move types
enum class MoveType {
    WALK_FORWARD,
    WALK_BACKWARD,
    REACH_UP,
    REACH_DOWN,
    REACH_LEFT,
    REACH_RIGHT,
    RESET_POSE
};

// A step in a motion sequence
struct SequenceMove {
    MoveType type;
    double parameter; // Amount to move or specific angle
    std::string segmentName; // For specific segment movements
};

class Walker {
public:
    // Constructor
    Walker(const std::shared_ptr<Body>& body, double walkSpeed = 5.0);
    
    // Update the walker's motion
    bool update(double timeStep);
    
    // Plan a sequence to catch an object at the given position
    void planCatchSequence(const Vector2D& objectPosition);
    
    // Plan a sequence to throw a snowball at the given position
    void planThrowSequence(const Vector2D& targetPosition);
    
    // Execute the next move in the sequence
    bool executeNextMove();
    
    // Reset the sequence (stop current execution and start over)
    void resetSequence();
    
    // Getters
    const std::vector<SequenceMove>& getSequence() const;
    int getCurrentMoveIndex() const;
    bool isSequenceComplete() const;
    
    // Check if the object has been caught
    bool hasObjectBeenCaught() const;
    
    // Set the object to catch
    void setTargetObject(const std::shared_ptr<Circle>& object);
    
    // Enable logging
    void enableLogging(std::shared_ptr<Logger> logger);
    
private:
    std::shared_ptr<Body> body;                 // The controlled body
    std::weak_ptr<Circle> targetObject;         // The object to catch
    std::vector<SequenceMove> sequence;         // Planned sequence of moves
    int currentMoveIndex;                       // Current position in the sequence
    double walkSpeed;                           // Speed for walking motions
    bool objectCaught;                          // Whether the object has been caught
    
    // Helper methods for planning sequences
    void addWalkingSequence(const Vector2D& destination);
    void addReachingSequence(const Vector2D& objectPosition);
    void addThrowingSequence(const Vector2D& targetPosition);
    
    // Helper methods for executing moves
    bool executeWalkForward(double distance);
    bool executeWalkBackward(double distance);
    bool executeReachUp(const std::string& segmentName, double angle);
    bool executeReachDown(const std::string& segmentName, double angle);
    bool executeReachLeft(const std::string& segmentName, double angle);
    bool executeReachRight(const std::string& segmentName, double angle);
    bool executeResetPose();
    
    // Logging
    std::shared_ptr<Logger> logger;
};

#endif // WALKER_H