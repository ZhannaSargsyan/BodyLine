#ifndef SNOWBALL_STRATEGY_H
#define SNOWBALL_STRATEGY_H

#include "MovementStrategy.h"
#include <vector>

/**
 * @class SnowballStrategy
 * @brief Concrete strategy for snowball throwing
 * 
 * Implements the MovementStrategy interface to provide
 * throwing behavior for the body to throw a snowball at a target.
 */
class SnowballStrategy : public MovementStrategy {
public:
    SnowballStrategy(std::shared_ptr<Body> body, std::shared_ptr<Circle> target, 
                     double snowballRadius = 10.0, double gravity = 9.8);
    
    // Core strategy interface implementation
    void planSequence() override;
    bool executeNextMove() override;
    bool isSequenceComplete() const override;
    
    // Snowball-specific methods
    void prepareThrow(const Vector2D& position, const Vector2D& velocity);
    void throwSnowball();
    void update(double deltaTime);
    void reset();
    
    bool isActive() const;
    bool hasHitTarget() const;
    bool hasHitGround() const;
    
    Vector2D getPosition() const;
    Vector2D getVelocity() const;
    double getRadius() const;
    
private:
    // Physics state
    bool active;
    Vector2D position;
    Vector2D velocity;
    double gravity;
    double radius;
    
    // Outcome state
    bool hitTarget;
    bool hitGround;
    
    // Internal methods
    void updatePhysics(double deltaTime);
    void checkCollisions();
    bool checkGroundCollision() const;
    bool checkTargetCollision() const;
};

#endif // SNOWBALL_STRATEGY_H