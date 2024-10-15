#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "quad.hpp"

class triangle : public quad {
    public:
        triangle (const point3& Q, const vec3& u, const vec3& v, std::shared_ptr<material> mat) :
            quad(Q, u, v, mat) {}

        virtual bool is_interior(double a, double b, hit_record& rec) const {
            // Barycentric coordinate condition
            if (!(a > 0 && b > 0 && a + b < 1)) {
                return false;
            }

            rec.u = a;
            rec.v = b;
            return true;
        }
};

#endif