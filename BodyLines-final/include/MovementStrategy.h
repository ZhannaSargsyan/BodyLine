#ifndef MOVEMENT_STRATEGY_H
#define MOVEMENT_STRATEGY_H

#include <memory>
#include "Body.h"
#include "Circle.h"
#include "Logger.h"

/**
 * @class MovementStrategy
 * @brief Abstract base class for movement strategies
 * 
 * This class defines the interface for all movement strategies
 * using the Strategy pattern. Different movement behaviors
 * can be implemented by creating concrete strategy classes.
 */
class MovementStrategy {
public:
    MovementStrategy(std::shared_ptr<Body> body, std::shared_ptr<Circle> target)
        : body(body), target(target) {}
    
    virtual ~MovementStrategy() = default;
    
    // Core strategy interface methods
    virtual void planSequence() = 0;
    virtual bool executeNextMove() = 0;
    virtual bool isSequenceComplete() const = 0;
    
    // Optional logging support
    virtual void enableLogging(std::shared_ptr<Logger> logger) {
        this->logger = logger;
    }
    
    // Target management
    virtual void setTarget(std::shared_ptr<Circle> newTarget) {
        target = newTarget;
    }
    
protected:
    std::shared_ptr<Body> body;
    std::shared_ptr<Circle> target;
    std::shared_ptr<Logger> logger;
};

#endif // MOVEMENT_STRATEGY_H