/*
*
* This file contains some math constants, utility functions,
* and general main headers
*
*/

#ifndef RTWEEKEND_HPP
#define RTWEEKEND_HPP

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

// Constants
#include "constants.hpp"

// Utility Functions
inline double degrees_to_radians(double degrees) {
    return degrees * kPi / 180.0;
}

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
    return static_cast<int>(random_double(static_cast<double>(min), static_cast<double>(max)));
}

// Common Headers
#include "vec3.hpp"
#include "color.hpp"
#include "point3.hpp"
#include "interval.hpp"
#include "ray.hpp"

#endif // RTWEEKEND_HPP