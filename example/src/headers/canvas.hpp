/*
*
* Canvas is a packed array that facilitates collection of colors and output image files.
* This file declare and implement the Canvas class.
*
*/

#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <cstdint>
#include <memory>
#include <fstream>
#include <string>
#include <stdexcept>

#include <stb/stb_image_write.h>

#include "color.hpp"
#include "interval.hpp"

class Canvas {

    public:
        Canvas(
            const uint32_t width,
            const uint32_t height
        ) :
            width(width),
            height(height),
            components(3),
            index(0),
            colors(new color[width * height]) {}

        // Overload the << operator to add colors to the canvas
        Canvas& operator<<(const color& c) {
            if (index >= width * height) {
                throw std::out_of_range("ERROR::Canvas::Index out of range: attempting to add more colors than the canvas can hold.");
            }

            double r = c.r();
            double g = c.g();
            double b = c.b();

            // Replace NaN components with zero.
            if (r != r) r = 0.0;
            if (g != g) g = 0.0;
            if (b != b) b = 0.0;

            r = linear_to_gamma(c.r());
            g = linear_to_gamma(c.g());
            b = linear_to_gamma(c.b());

            colors[index++] = color(r, g, b);
            return *this;
        }

        
        // Enum class which provides some output image type.
        enum class img_type : uint8_t {
            none = 0,
            ppm = 1 << 0,
            png = 1 << 1,
            jpg = 1 << 2
        };

        // Output image file.
        // The file name cannot contain the suffix type, the function will automatically generate it. 
        void writeImage(std::string file_name, img_type type) const {
            if (index < width * height) {
                throw std::runtime_error("ERROR::Canvas::Canvas not fully filled.");
            }
            // For .ppm image
            if (static_cast<bool>(type & img_type::ppm)) {
                writePpm(file_name);
            }
            // For .png image
            if (static_cast<bool>(type & img_type::png)) {
                writePng(file_name);
            }
            // For .jpg image
            if (static_cast<bool>(type & img_type::jpg)) {
                writeJpg(file_name);
            }

            if (type == img_type::none) {
                throw std::invalid_argument("ERROR::Canvas::Unsupported image type.");
            }
        } 


    private:
        uint32_t width;                   // The width of image
        uint32_t height;                  // The height of image
        uint32_t components;              // The number of color components per pixel, in this project is 3.
        size_t   index;                   // Index pointing to the current pixel to be written
        std::unique_ptr<color[]> colors;  // Collected color data
        static const interval intensity;  // Used to clamp the range of output color

        // Helper function to write PPM files
        void writePpm(std::string file_name) const {
            file_name = file_name + ".ppm";
            std::ofstream file(file_name, std::ios::out | std::ios::binary);
            if (!file) {
                throw std::runtime_error("ERROR::Canvas::Failed to open file for writing.");
            }

            file << "P3\n" << width << " " << height << "\n255\n";
            for (size_t i = 0; i < width * height; ++i) {
                // Translate the [0,1] component values to the byte range [0,255].
                file << static_cast<uint32_t>(256 * intensity.clamp(colors[i].r())) << ' '
                     << static_cast<uint32_t>(256 * intensity.clamp(colors[i].g())) << ' '
                     << static_cast<uint32_t>(256 * intensity.clamp(colors[i].b())) << '\n';
            }
            file.close();
        }

        // Helper function to write PNG files using stb_image_write.h
        void writePng(std::string file_name) const {
            file_name = file_name + ".png";
            std::unique_ptr<uint8_t[]> data(new uint8_t[width * height * components]);
            for (size_t i = 0; i < width * height; ++i) {
                // Translate the [0,1] component values to the byte range [0,255].
                data[i * components + 0] = static_cast<uint8_t>(256 * intensity.clamp(colors[i].r()));
                data[i * components + 1] = static_cast<uint8_t>(256 * intensity.clamp(colors[i].g()));
                data[i * components + 2] = static_cast<uint8_t>(256 * intensity.clamp(colors[i].b()));
            }
            if (!stbi_write_png(file_name.c_str(), width, height, components, data.get(), width * components)) {
                throw std::runtime_error("ERROR::Canvas::Failed to write PNG file.");
            }
        }

        // Helper function to write JPG files using stb_image_write.h
        void writeJpg(std::string file_name) const {
            file_name = file_name + ".jpg";
            std::unique_ptr<uint8_t[]> data(new uint8_t[width * height * components]);
            for (size_t i = 0; i < width * height; ++i) {
                // Translate the [0,1] component values to the byte range [0,255].
                data[i * components + 0] = static_cast<uint8_t>(256 * intensity.clamp(colors[i].r()));
                data[i * components + 1] = static_cast<uint8_t>(256 * intensity.clamp(colors[i].g()));
                data[i * components + 2] = static_cast<uint8_t>(256 * intensity.clamp(colors[i].b()));
            }
            if (!stbi_write_jpg(file_name.c_str(), width, height, components, data.get(), 100)) {
                throw std::runtime_error("ERROR::Canvas::Failed to write PNG file.");
            }
        }


    public:
        // Define bitwise operators for imgtp
        friend img_type operator|(img_type lhs, img_type rhs) {
            return static_cast<img_type>(
                static_cast<std::underlying_type_t<img_type>>(lhs) |
                static_cast<std::underlying_type_t<img_type>>(rhs)
            );
        }

        friend img_type& operator|=(img_type& lhs, img_type rhs) {
            lhs = lhs | rhs;
            return lhs;
        }

        friend img_type operator&(img_type lhs, img_type rhs) {
            return static_cast<img_type>(
                static_cast<std::underlying_type_t<img_type>>(lhs) &
                static_cast<std::underlying_type_t<img_type>>(rhs)
            );
        }

        friend img_type& operator&=(img_type& lhs, img_type rhs) {
            lhs = lhs & rhs;
            return lhs;
        }

        friend img_type operator~(img_type lhs) {
            return static_cast<img_type>(
                ~static_cast<std::underlying_type_t<img_type>>(lhs)
            );
        }
};

const interval Canvas::intensity = interval(0.000, 0.999);

#endif // CANVAS_HPP
