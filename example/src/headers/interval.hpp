/*
*
* This file declares and implements interval class to define
* the range of real value.
*
*/

#ifndef INTERVAL_HPP
#define INTERVAL_HPP

#include "constants.hpp"

class interval {
    public:
        double min, max;

        interval() : min(+kInfinity), max(-kInfinity) {} // Default is empty

        interval(double min, double max) : min(min), max(max) {}

        interval(const interval& a, const interval& b) {
            // Create the interval tightly enclosing the two input intervals.
            min = a.min <= b.min ? a.min : b.min;
            max = a.max >= b.max ? a.max : b.max;
        }

        double size() const {
            return max - min;
        }

        bool contains(double x) const {
            return min <= x && x <= max;
        }

        bool surrounds(double x) const {
            return min < x && x < max;
        }

        double clamp(double x) const {
            return (x < min) ? min : ((x > max) ? max : x);
        }

        interval expand(double delta) const {
            double padding = delta / 2.0;
            return interval(min - padding, max + padding);
        }

        static const interval empty, universe;
};

const interval interval::empty    = interval(+kInfinity, -kInfinity);
const interval interval::universe = interval(-kInfinity, +kInfinity);

interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}

#endif // INTERVAL_HPP