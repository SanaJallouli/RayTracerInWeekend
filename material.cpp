#pragma once
#include "material.h"
#include "hittable.h"
using color = vec3;
class hit_record;

class material {
public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

