#ifndef _BLINN_PHONG_SHADER_H_
#define _BLINN_PHONG_SHADER_H_

#include <cmath>
#include <algorithm>
#include "shader.h"
#include "scene/components/light.h"

using namespace utils;

/*!
 * This class represents the Blinn-Phong shader.
 */
class BlinnPhongShader : public Shader {

    private:
        // Ambient light
        RGB alight;

    public:
        /*!
         * Blinn-Phong shader constructor.
         *
         * @param alight_ Ambient light
         */
        BlinnPhongShader(RGB alight_) : alight(alight_)
            {/* empty */}

        RGB color(const Ray& r, const Scene& scene, int nrays) const {
            (void) nrays;
            // Check hit
            HitRecord hr;
            if (intersect(r, scene, hr)) {
                // Get Blinn-Phong material
                BlinnPhongMaterial* material =
                    dynamic_cast<BlinnPhongMaterial*>(hr.material);
                // Ambient light on the shape
                RGB c = material->ka * alight;
                // Check shadows
                HitRecord shr;
                for (auto& light : scene.lights) {
                    if (!intersect(Ray(hr.origin, light->getDirection()), scene,
                            shr)) {
                        c += blinnPhong(r, light, hr);
                    }
                }
                // Fix specular
                c = RGB(std::min(1.f, float(c[RGB::R])),
                        std::min(1.f, float(c[RGB::G])),
                        std::min(1.f, float(c[RGB::B])));
                // Return resulting color
                return c;
            } else {
                return background(r, scene);
            }
        }

    private:
        /*!
         * Applies the Blinn-Phong shadding model.
         *
         * @param r Incoming ray
         * @param light Scene light
         * @param hr Hit record
         *
         * @return Corresponding color
         */
        RGB blinnPhong(const Ray& r, Light* light, HitRecord& hr) const {
            // L.N
            BlinnPhongMaterial* material =
                dynamic_cast<BlinnPhongMaterial*>(hr.material);
            Vec3 ln = unitVector(light->getDirection() - r.getDirection());
            float lambertian = std::max(0.f, dot(ln, hr.normal));
            float specular = 0.0;
            // Blinn-Phong
            Vec3 H = unitVector(ln + unitVector(-r.getDirection()));
            // N.H
            specular = std::max(0.f, dot(hr.normal, H));
            specular = std::pow(specular, material->p);
            // 
            return material->kd * lambertian * light->getIntensity() +
                   material->ks * specular * light->getIntensity();
        }
};

#endif /* _BLINN_PHONG_SHADER_H_ */
