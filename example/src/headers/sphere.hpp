/*
*
* This file declare and implement sphere class.
*
*/

#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <cmath>
#include <memory>

#include "hittable.hpp"
#include "vec3.hpp"
#include "point3.hpp"


class sphere : public hittable {
    public :
        // Stationary Sphere
        sphere(const point3& center, double radius, std::shared_ptr<material> mat) :
        center1(center), radius(std::max(0.0, radius)), mat(mat), is_moving(false) {
            vec3 rvec = vec3(radius, radius, radius);
            bbox = aabb(center1 - rvec, center1 + rvec);
        }

        // Moving sphere
        sphere(const point3& center1, const point3& center2, double radius,
               std::shared_ptr<material> mat) :
        center1(center1), radius(std::max(0.0, radius)), mat(mat), is_moving(true) {
            vec3 rvec = vec3(radius, radius, radius);
            aabb box1(center1 - rvec, center1 + rvec);
            aabb box2(center2 - rvec, center2 + rvec);
            bbox = aabb(box1, box2);

            center_vec = center2 - center1;
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Solve quadratic equation for sphere intersection.
            // Use a simplified solution.
            point3 center = is_moving ? sphere_center(r.time()) : center1;
            vec3 oc = center - r.origin();
            double a = r.direction().length_squared();
            double h = dot(r.direction(), oc);
            double c = oc.length_squared() - radius * radius;

            double discriminant = h * h - a * c;
            if (discriminant < 0) {
                return false;
            }

            double sqrtd = std::sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            double root = (h - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root)) {
                    return false;
                }
            }

            // Update records
            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            get_sphere_uv(outward_normal, rec.u, rec.v);
            rec.mat = mat;

            return true;
        }

        aabb bounding_box() const override { return bbox; }

        double pdf_value(const point3& origin, const vec3& direction) const override {
            // This method only works for stationary spheres.

            hit_record rec;
            if (!this->hit(ray(origin, direction), interval(0.001, kInfinity), rec)) {
                return 0;
            }

            double cos_theta_max = std::sqrt(1 - radius * radius / (center1 - origin).length_squared());
            double solid_angle = 2 * kPi * (1 - cos_theta_max);

            return 1 / solid_angle;
        }

        vec3 random(const point3& origin) const override {
            vec3 direction = center1 - origin;
            double distance_squared = direction.length_squared();
            onb uvw(direction);
            return uvw.transform(random_to_sphere(radius, distance_squared));
        }
    
    private:
        point3 center1;
        double radius;
        std::shared_ptr<material> mat;
        bool is_moving;
        vec3 center_vec;
        aabb bbox;

        point3 sphere_center(double time) const {
            // Linearly interpolate from center1 to center2 according to time, where t=0 yields
            // center1, and t=1 yields center2.
            return center1 + time * center_vec;
        }

        static void get_sphere_uv(const point3& p, double& u, double& v) {
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            double theta = std::acos(-p.y());
            double phi   = std::atan2(-p.z(), p.x()) + kPi;

            u = phi / (2 * kPi);
            v = theta / kPi;
        }

        static vec3 random_to_sphere(double radius, double distance_squared) {
            double r1 = random_double();
            double r2 = random_double();
            double z  = 1 + r2 * (std::sqrt(1 - radius * radius / distance_squared) - 1);

            double phi = 2 * kPi * r1;
            double x = std::cos(phi) * std::sqrt(1 - z * z);
            double y = std::sin(phi) * std::sqrt(1 - z * z);

            return vec3(x, y, z);
        }
};

#endif // SPHERE_HPP