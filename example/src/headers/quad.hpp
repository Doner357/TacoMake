#ifndef QUAD_HPP
#define QUAD_HPP

#include "rtweekend.hpp"

#include "hittable.hpp"
#include "hittable_list.hpp"

class quad : public hittable {
    public:
        quad(const point3& Q, const vec3& u, const vec3& v, std::shared_ptr<material> mat) :
            Q(Q), u(u), v(v), mat(mat)
        {
            vec3 n = cross(u, v);
            normal = unit_vector(n);
            D = dot(normal, Q);
            w = n / dot(n, n);

            area = n.length();

            set_bounding_box();
        }

        virtual void set_bounding_box() {
            // Compute the bounding box of all four vertices.
            aabb bbox_diagonal1 = aabb(Q, Q + u + v);
            aabb bbox_diagonal2 = aabb(Q + u, Q + v);
            bbox = aabb(bbox_diagonal1, bbox_diagonal2);
        }

        aabb bounding_box() const override { return bbox; }

        bool hit (const ray& r, interval ray_t, hit_record& rec) const override {
            auto denom = dot(normal, r.direction());

            // No hit if the ray is parallel to the plane.
            if (std::fabs(denom) < 1e-8) {
                return false;
            }

            // Return false if the hit point parameter t is outside the ray interval
            double t = (D - dot(normal, r.origin())) / denom;
            if (!ray_t.contains(t)) {
                return false;
            }

            // Determine the hit point lies within the planer shape using its plane coordinates.
            vec3 intersection = r.at(t);
            vec3 planar_hitpt_vector = intersection - Q;
            double alpha = dot(w, cross(planar_hitpt_vector, v));
            double beta  = dot(w, cross(u, planar_hitpt_vector));

            if (!is_interior(alpha, beta, rec)) {
                return false;
            }

            // Ray hits the 2D shape; set the rest of the hit record and return true.
            rec.t   = t;
            rec.p   = intersection;
            rec.mat = mat;
            rec.set_face_normal(r, normal);

            return true;
        }

        virtual bool is_interior(double a, double b, hit_record& rec) const {
            interval unit_interval = interval(0.0, 1.0);
            // Given the hit point in plane coordinates, return false if it is outside the 
            // primitive, otherwise set the hit record UV coordinates and return true.

            if (!unit_interval.contains(a) || !unit_interval.contains(b)) {
                return false;
            }

            rec.u = a;
            rec.v = b;
            return true;
        }

        double pdf_value(const point3& origin, const vec3& direction) const override {
            hit_record rec;
            if (!this->hit(ray(origin, direction), interval(0.001, kInfinity), rec)) {
                return 0;
            }

            double distance_squared = rec.t * rec.t * direction.length_squared();
            double cosine = std::fabs(dot(direction, rec.normal) / direction.length());

            return distance_squared / (cosine * area);
        }

        vec3 random(const point3& origin) const override {
            point3 p = Q + (random_double() * u) + (random_double() * v);
            return p - origin;
        }

    private:
        point3 Q;
        vec3 u, v;
        vec3 w;
        std::shared_ptr<material> mat;
        aabb bbox;
        vec3 normal;
        double D;   // normal * (x, y, z) = D
        double area;
};

inline std::shared_ptr<hittable_list> box(const point3& a, const point3& b, std::shared_ptr<material> mat) {
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    std::shared_ptr<hittable_list> sides = std::make_shared<hittable_list>();

    // Construct the two opposite vertices with the minimum and maximum coordiantes.
    point3 min = point3(std::min(a.x(), b.x()), std::min(a.y(), b.y()), std::min(a.z(), b.z()));
    point3 max = point3(std::max(a.x(), b.x()), std::max(a.y(), b.y()), std::max(a.z(), b.z()));

    vec3 dx = vec3(max.x() - min.x(), 0.0, 0.0);
    vec3 dy = vec3(0.0, max.y() - min.y(), 0.0);
    vec3 dz = vec3(0.0, 0.0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}

#endif