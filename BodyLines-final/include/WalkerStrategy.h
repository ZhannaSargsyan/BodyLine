#ifndef WALKER_STRATEGY_H
#define WALKER_STRATEGY_H

#include "MovementStrategy.h"
#include <vector>
#include <deque>

/**
 * @class WalkerStrategy
 * @brief Concrete strategy for walking movement
 * 
 * Implements the MovementStrategy interface to provide
 * walking behavior for the body to approach and catch an object.
 */
class WalkerStrategy : public MovementStrategy {
public:
    WalkerStrategy(std::shared_ptr<Body> body, std::shared_ptr<Circle> target, double walkSpeed = 5.0);
    
    // Core strategy interface implementation
    void planSequence() override;
    void planSequence(const Vector2D& objectPosition);
    bool executeNextMove() override;
    bool isSequenceComplete() const override;
    
    // Walker-specific methods
    bool hasObjectBeenCaught() const;
    void setWalkSpeed(double speed);
    double getWalkSpeed() const;
    
private:
    struct Move {
        enum class Type { WALK, REACH, GRAB };
        Type type;
        Vector2D position;
        std::string segmentName;
        double rotationAmount;
    };
    
    void addWalkingSequence(const Vector2D& targetPos);
    void addReachingSequence(const Vector2D& targetPos);
    bool executeWalkMove(const Move& move);
    bool executeReachMove(const Move& move);
    
    double walkSpeed;
    std::deque<Move> plannedMoves;
    bool objectCaught;
    int currentMoveIndex;
    int minGroundContacts;
    int minObjectContacts;
};

#endif // WALKER_STRATEGY_H