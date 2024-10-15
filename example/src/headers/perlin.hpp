#ifndef PERLIN_HPP
#define PERLIN_HPP

#include <cstdint>

#include "rtweekend.hpp"

class perlin {
    public:
        perlin() {
            randvec = new vec3[point_count];
            for (int32_t i = 0; i < point_count; ++i) {
                randvec[i] = unit_vector(vec3::random(-1.0, 1.0));
            }

            perm_x = perlin_generate_perm();
            perm_y = perlin_generate_perm();
            perm_z = perlin_generate_perm();
        }

        ~perlin() {
            delete [] randvec;
            delete [] perm_x;
            delete [] perm_y;
            delete [] perm_z;
        }

        double noise(const point3& p) const {
            double u = p.x() - std::floor(p.x());
            double v = p.y() - std::floor(p.y());
            double w = p.z() - std::floor(p.z());

            int32_t i = int32_t(std::floor(p.x()));
            int32_t j = int32_t(std::floor(p.y()));
            int32_t k = int32_t(std::floor(p.z()));
            vec3 c[2][2][2];

            for (int32_t di = 0; di < 2; ++di) {
                for (int32_t dj = 0; dj < 2; ++dj) {
                    for (int32_t dk = 0; dk < 2; ++dk) {
                        // Hash
                        c[di][dj][dk] = randvec[
                            perm_x[(i + di) & 255] ^
                            perm_y[(j + dj) & 255] ^
                            perm_z[(k + dk) & 255]
                        ];
                    }
                }
            }

            return perlin_interp(c, u, v, w);
        }

        // Turbulence (Composite noise that has multiple summed frequencies is used).
        double turb(const point3& p, int depth) const {
            double accum  = 0.0;
            double weight = 1.0;
            point3 temp_p = p;

            for(int32_t i = 0; i < depth; i++) {
                accum  += weight * noise(temp_p);
                weight *= 0.5;
                temp_p *= 2.0;
            }

            return fabs(accum);
        }

    
    private:
        static const int point_count = 256;
        vec3*    randvec;
        int32_t* perm_x;
        int32_t* perm_y;
        int32_t* perm_z;

        static int32_t* perlin_generate_perm() {
            int32_t* p = new int32_t[point_count];

            for (int32_t i = 0; i < point_count; i++) {
                p[i] = i;
            }

            permute(p, point_count);

            return p;   
        }

        static void permute(int32_t* p, int32_t n) {
            for (int32_t i = n - 1; i > 0; i--) {
                int target = random_int(0, i);
                int tmp = p[i];
                p[i] = p[target];
                p[target] = tmp;
            }
        }

        // Trilinear interpolation for given point
        static double perlin_interp(const vec3 c[2][2][2], double u, double v, double w) {
            // Hermitian smoothing
            double uu = u * u * (3 - 2 * u);
            double vv = v * v * (3 - 2 * v);
            double ww = w * w * (3 - 2 * w);
            double accum = 0.0;

            for (int32_t i = 0; i < 2; i++) {
                for (int32_t j = 0; j < 2; j++) {
                    for (int32_t k = 0; k < 2; k++) {
                        vec3 weight_v(u - i, v - j, w - k);
                        accum += (i * uu + (1 - i) * (1 - uu)) *
                                 (j * vv + (1 - j) * (1 - vv)) *
                                 (k * ww + (1 - k) * (1 - ww)) *
                                 dot(c[i][j][k], weight_v);
                    }
                }
            }

            return accum;
        }
};

#endif // PERLIN_HPP