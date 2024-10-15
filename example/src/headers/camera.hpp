/*
*
* This file declares and implement the camera class, which
* compacts the rendering functions.
*
*/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <cstdint>
#include <string>
#include <chrono>

#include "rtweekend.hpp"
#include "canvas.hpp"
#include "hittable.hpp"
#include "pdf.hpp"
#include "material.hpp"

class Camera {
    public:
        // -- Image --
        // Image attributes
        double   aspect_ratio      = 1.0;  // Ratio of image width over height
        uint32_t image_width       = 100;  // Rendered image width in pixel count
        uint32_t samples_per_pixel = 10;   // Count of random samples for each pixel
        int32_t  max_depth         = 10;   // Maximum number of ray bounces into scene
        color    background;               // Scene background color (black)

        double vfov     = 90.0; // Vertical view angle (field of view)
        point3 lookfrom = point3( 0.0,  0.0,  0.0); // Point camera is looking from
        point3 lookat   = point3( 0.0,  0.0, -1.0); // Point camera is looking at
        vec3   vup      = vec3(0.0, 1.0, 0.0);      // Camera-relative "up" direction 

        double defocus_angle = 0;   // Variation angle of rays through each pixel
        double focus_dist    = 10;  // Distance from camera lookfrom point to plane of perfect focus


        void render(const hittable& world, const hittable& lights, std::string image_name) {
            // Inittialize
            initialize();

            // -- Canvas --
            // Canvas to store and write image
            Canvas canvas(image_width, image_height);
            // Image type to be written
            Canvas::img_type image_type = Canvas::img_type::ppm  |  // .ppm
                                          Canvas::img_type::png  |  // .png
                                          Canvas::img_type::jpg;    // .jpg

            // Timer
            std::chrono::time_point start_time = std::chrono::steady_clock::now();

            // -- Render --
            for (uint32_t j = 0; j < image_height; j++) {
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (uint32_t i = 0; i < image_width; i++) {
                    color pixel_color(0.0, 0.0, 0.0);
                    for (int32_t s_j = 0; s_j < sqrt_spp; s_j++) {
                        for (int32_t s_i = 0; s_i < sqrt_spp; s_i++) {
                            ray r = get_ray(i, j, s_i, s_j);
                            pixel_color += ray_color(r, max_depth, world, lights);
                        }
                    }

                    canvas << pixel_color * pixel_samples_scale;
                }
            }
            std::clog << "\rDone.                       \n";

            // Stop timer
            std::chrono::time_point end_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> duration = end_time - start_time;
            
            std::chrono::duration time = end_time - start_time;
            std::chrono::hours h = std::chrono::duration_cast<std::chrono::hours>(time);
            time -= h;
            std::chrono::minutes m = std::chrono::duration_cast<std::chrono::minutes>(time);
            time -= m;
            std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(time);

            std::ofstream time_record;
            time_record.open(image_name + std::string(".txt"));
            time_record << "Takes: " << h << ' ' << m << ' ' << s << std::endl;
            time_record.close();

            // Write images
            canvas.writeImage(image_name, image_type);
        }


    private:
        uint32_t image_height;         // Rendered image height
        double   pixel_samples_scale;  // Color scale factor for a sum of pixel samples
        int32_t  sqrt_spp;             // Square root of number of samples per pixel
        double   recip_sqrt_spp;       // 1 / sqrt_spp
        point3   center;               // Camera center
        point3   pixel00_loc;          // Location of pixel 0, 0
        vec3     pixel_delta_u;        // Offset to pixel to the right
        vec3     pixel_delta_v;        // Offset to pixel below
        vec3     u, v, w;              // Camera frame basis vectors
        vec3     defocus_disk_u;       // Defocus disk horizontal radius
        vec3     defocus_disk_v;       // Defocus disk vertical radius


        void initialize() {
            // Calculate the image height, and ensure that it's at least 1.
            image_height = static_cast<uint32_t>(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            sqrt_spp = static_cast<int32_t>(std::sqrt(samples_per_pixel));
            pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);
            recip_sqrt_spp = 1.0 / sqrt_spp;

            pixel_samples_scale = 1.0 / samples_per_pixel;

            center = lookfrom;
            
            // Determine viewport dimensions.
            double theta           = degrees_to_radians(vfov);
            double h               = std::tan(theta / 2);
            double viewport_height = 2 * h * focus_dist;
            double viewport_width  = viewport_height * (static_cast<double>(image_width) / image_height);

            // Calculate the u, v, w unit basis vectors for the camera coordinate frame
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
            vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

            // Calculate the horizontal and vertical delta vectors from pixel to pixel.
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Calculate the location of the upper left pixel.
            point3 viewport_upper_left = center - (focus_dist * w) - viewport_u / 2.0 - viewport_v / 2.0;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            // Calculate the camera defocus disk basis vectors.
            double defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        ray get_ray(uint32_t i, uint32_t j, int32_t s_i, int32_t s_j) const {
            // Construct a camera ray originating from the defocus disk and directed at a randomly
            // sampled point around the pixel location i, j for stratified sample square s_i, s_j.
            vec3 offset = sample_square_stratified(s_i, s_j);
            point3 pixel_sample = pixel00_loc +
                                  ((i + offset.x()) * pixel_delta_u) +
                                  ((j + offset.y()) * pixel_delta_v);

            point3 ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            vec3 ray_direction = pixel_sample - ray_origin;
            double ray_time = random_double();

            return ray(ray_origin, ray_direction, ray_time);
        }

        vec3 sample_square_stratified(int32_t s_i, int32_t s_j) const {
            // Returns the vector to a random point in the square sub-pixel specified by grid
            // indices s_i and s_j, for an idealized unit square pixel [-.5,-.5] to [+.5,+.5].

            double px = ((s_i + random_double()) * recip_sqrt_spp) - 0.5;
            double py = ((s_j + random_double()) * recip_sqrt_spp) - 0.5;

            return vec3(px, py, 0);
        }

        vec3 sample_square() const {
            // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
            return vec3(random_double() - 0.5, random_double() - 0.5, 0);
        }

        point3 defocus_disk_sample() const {
            // Returns a random point in the camera defocus disk.
            vec3 p = random_in_unit_disk();
            return center + (p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
        }

        color ray_color(const ray& r, int32_t depth, const hittable& world, const hittable& lights) const {
            // If we've exceeded the ray bounce limit, no more light is gathered
            if (depth <= 0) {
                return color(0.0, 0.0, 0.0);
            }

            hit_record rec;
            
            // If the ray hits nothing, return the background color.
            if (!world.hit(r, interval(0.001, kInfinity), rec)) {
                return background;
            }

            scatter_record srec;
            color  color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

            if (!rec.mat->scatter(r, rec, srec)) {
                return color_from_emission;
            }

            if (srec.skip_pdf) {
                return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, world, lights);
            }

            std::shared_ptr<pdf> light_ptr = std::make_shared<hittable_pdf>(lights, rec.p);
            mixture_pdf p(light_ptr, srec.pdf_ptr);

            ray scattered = ray(rec.p, p.generate(), r.time());
            double pdf_value = p.value(scattered.direction());

            double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

            color sample_color = ray_color(scattered, depth - 1, world, lights);
            color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_value;

            return color_from_emission + color_from_scatter;
        }
};

#endif // CAMERA_HPP