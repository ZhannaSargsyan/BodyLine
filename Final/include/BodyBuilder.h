#ifndef BODY_BUILDER_H
#define BODY_BUILDER_H

#include "Body.h"
#include "Vector2D.h"
#include <memory>
#include <string>
#include <map>

/**
 * @class BodyBuilder
 * @brief Builder class for creating Body objects
 * 
 * This class implements the Builder pattern to create complex Body objects
 * with various segments and connections in a flexible, step-by-step manner.
 */
class BodyBuilder {
public:
    BodyBuilder();
    
    // Core builder methods
    BodyBuilder& setBasePosition(const Vector2D& position);
    BodyBuilder& setGroundLevel(double level);
    
    // Segment creation methods
    BodyBuilder& addSegment(const std::string& name, double length, double angle, 
                          double minAngle = -M_PI, double maxAngle = M_PI);
    BodyBuilder& connectSegments(const std::string& parentName, const std::string& childName);
    
    // Predefined body configurations
    BodyBuilder& buildHumanoidBody();
    BodyBuilder& buildSimpleBody();
    
    // Build method to create the final Body object
    std::shared_ptr<Body> build();
    
    // Reset builder for reuse
    void reset();
    
private:
    Vector2D basePosition;
    double groundLevel;
    
    struct SegmentSpec {
        double length;
        double angle;
        double minAngle;
        double maxAngle;
    };
    
    struct Connection {
        std::string parent;
        std::string child;
    };
    
    std::map<std::string, SegmentSpec> segmentSpecs;
    std::vector<Connection> connections;
};

#endif // BODY_BUILDER_H