#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <memory>
#include <cmath>

#include <rtw/rtw_stb_image.hpp>

#include "color.hpp"
#include "point3.hpp"
#include "perlin.hpp"

class texture {
    public:
        virtual ~texture() = default;

        virtual color value(double u, double v, const point3& p) const = 0;
};


// Texture only contains constant color on every pixel
class solid_color : public texture {
    public:
        solid_color(const color& albedo) : albedo(albedo) {}

        solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

        color value(double u, double v, const point3& p) const override {
            return albedo;
        }
    
    private:
        color albedo;
};


// Checker Texture
class checker_texture : public texture {
    public:
        checker_texture(double scale, std::shared_ptr<texture> even, std::shared_ptr<texture> odd) :
            inv_scale(1.0 / scale), even(even), odd(odd) {}

        checker_texture(double scale, const color& c1, const color& c2) :
            inv_scale(1.0 / scale),
            even(std::make_shared<solid_color>(c1)),
            odd(std::make_shared<solid_color>(c2))
            {}

        color value(double u, double v, const point3& p) const override {
            int32_t x_integer = static_cast<int>(std::floor(inv_scale * p.x()));
            int32_t y_integer = static_cast<int>(std::floor(inv_scale * p.y()));
            int32_t z_integer = static_cast<int>(std::floor(inv_scale * p.z()));

            bool is_even = (x_integer + y_integer + z_integer) % 2 == 0;

            return is_even ? even->value(u, v, p) : odd->value(u, v, p);
        }


    private:
        double inv_scale;
        std::shared_ptr<texture> even;
        std::shared_ptr<texture> odd;
};


// Texture using image
class image_texture : public texture {
    public:
        image_texture(const char* filename) : image(filename) {}

        color value(double u, double v, const point3& p) const override {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (image.height() <= 0) {
                return color(0.0, 1.0, 1.0);
            }

            // Clamp input texture coordinates to [0,1] x [1,0]
            u = interval(0, 1).clamp(u);
            v = 1.0 - interval(0, 1).clamp(v);

            int32_t i = int(u * image.width());
            int32_t j = int(v * image.height());
            const unsigned char* pixel = image.pixel_data(i, j);

            double color_scale = 1.0 / 255.0;
            return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
        }

    private:
        rtw_image image;
};


class noise_texture : public texture {
    public:
        noise_texture() {}

        noise_texture(double scale) : scale(scale) {}

        color value(double u, double v, const point3& p) const override {
            return color(0.5, 0.5, 0.5) * (1.0 + std::sin(scale * p.z() + 10 * noise.turb(p, 7.0)));
        }

    private:
        perlin noise;
        double scale;
};

#endif