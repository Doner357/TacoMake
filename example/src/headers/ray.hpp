/*
*
* This file declare and implement the ray class.
*
*/

#ifndef RAY_HPP
#define RAY_HPP

#include "vec3.hpp"
#include "point3.hpp"

class ray {
    public:
        ray() {}

        ray(const point3& origin, const vec3& direction) :
        orig(origin), dir(unit_vector(direction)), tm(0) {}

        ray(const point3& origin, const vec3& direction, double time) :
        orig(origin), dir(unit_vector(direction)), tm(time) {}

        const point3& origin() const { return orig; }
        const vec3& direction() const { return dir; }

        double time() const { return tm; }

        // Function for ray P(t) = origin + t * direction
        point3 at(double t) const {
            return orig + t * dir;
        }

    private:
        point3 orig;
        vec3 dir;
        double tm;
};


#endif // RAY_HPP