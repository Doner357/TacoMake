/*
*
* This file declare and implement the color with three double components: R, G B.
*
*/

#ifndef COLOR_HPP
#define COLOR_HPP

#include <cmath>

#include "vec3.hpp"

// For less code, color is just an alias for vec3.
using color = vec3;

// Utility Functions
inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) {
        // Do gamma 2 instead of 2.2, since it can be simplified to square root.
        return std::sqrt(linear_component);
    }

    return 0.0;
}

#endif // COLOR_HPP