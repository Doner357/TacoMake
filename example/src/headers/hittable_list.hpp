/*
*
* This file declare and implement hittable list class to store a
* list of hittable objects.
*
*/

#ifndef HITTABLE_LIST_HPP
#define HITTABLE_LIST_HPP

#include "hittable.hpp"

#include <memory>
#include <vector>

class hittable_list : public hittable {
    public:
        std::vector<std::shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(std::shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }

        void add(std::shared_ptr<hittable> object) {
            objects.push_back(object);
            bbox = aabb(bbox, object->bounding_box());
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            hit_record temp_rec;
            bool hit_anything = false;
            double closest_so_far = ray_t.max;

            for (const auto& object : objects) {
                if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }

            return hit_anything;
        }

        aabb bounding_box() const override { return bbox; }

        double pdf_value(const point3& origin, const vec3& direction) const override {
            double weight = 1.0 / objects.size();
            double sum    = 0.0;

            for (const auto& object : objects) {
                sum += weight * object->pdf_value(origin, direction);
            }

            return sum;
        }

        vec3 random(const point3& origin) const override {
            int32_t int_size = static_cast<int32_t>(objects.size());
            return objects[random_int(0, int_size - 1)]->random(origin);
        }


    private:
        aabb bbox;
};

#endif // HITTABLE_LIST_HPP