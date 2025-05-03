/**
 * @file Vector2D.cpp
 * @brief Implementation of the Vector2D class
 */
#include "../include/Vector2D.h"
#include <cmath>

Vector2D::Vector2D() : x(0), y(0) {}

Vector2D::Vector2D(double x, double y) : x(x), y(y) {}

Vector2D::Vector2D(const Vector2D& v) : x(v.x), y(v.y) {}

Vector2D Vector2D::operator+(const Vector2D& v) const {
    return Vector2D(x + v.x, y + v.y);
}

Vector2D Vector2D::operator-(const Vector2D& v) const {
    return Vector2D(x - v.x, y - v.y);
}

Vector2D Vector2D::operator*(double scalar) const {
    return Vector2D(x * scalar, y * scalar);
}

Vector2D Vector2D::operator/(double scalar) const {
    if (scalar == 0) {
        return *this;  // Avoid division by zero
    }
    return Vector2D(x / scalar, y / scalar);
}

Vector2D& Vector2D::operator+=(const Vector2D& v) {
    x += v.x;
    y += v.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

Vector2D& Vector2D::operator*=(double scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2D& Vector2D::operator/=(double scalar) {
    if (scalar != 0) {
        x /= scalar;
        y /= scalar;
    }
    return *this;
}

bool Vector2D::operator==(const Vector2D& v) const {
    const double EPSILON = 1e-6;  // Small tolerance for floating-point comparisons
    return std::abs(x - v.x) < EPSILON && std::abs(y - v.y) < EPSILON;
}

bool Vector2D::operator!=(const Vector2D& v) const {
    return !(*this == v);
}

double Vector2D::length() const {
    return std::sqrt(x * x + y * y);
}

double Vector2D::lengthSquared() const {
    return x * x + y * y;
}

double Vector2D::distance(const Vector2D& v) const {
    return (*this - v).length();
}

double Vector2D::distanceSquared(const Vector2D& v) const {
    return (*this - v).lengthSquared();
}

Vector2D Vector2D::normalized() const {
    double len = length();
    if (len == 0) {
        return Vector2D(0, 0);
    }
    return Vector2D(x / len, y / len);
}

void Vector2D::normalize() {
    double len = length();
    if (len != 0) {
        x /= len;
        y /= len;
    }
}

double Vector2D::dot(const Vector2D& v) const {
    return x * v.x + y * v.y;
}

double Vector2D::cross(const Vector2D& v) const {
    return x * v.y - y * v.x;
}

Vector2D Vector2D::rotate(double angle) const {
    double cosA = std::cos(angle);
    double sinA = std::sin(angle);
    return Vector2D(
        x * cosA - y * sinA,
        x * sinA + y * cosA
    );
}

double Vector2D::angle() const {
    return std::atan2(y, x);
}

double Vector2D::angleBetween(const Vector2D& v) const {
    double dot = this->dot(v);
    double det = this->cross(v);
    return std::atan2(det, dot);
}

std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}