#pragma once

#include "metal.h"
#include "material.h"
#include "hittable.h"
class material;

class metal : public material
{
public:
	color albedo; // the base color of the material 

	float fuzz;// the radius of the sphere on which we choose a position for the toip of the new ray : the larger this radius, the more randomized the reflected ray is. 

	metal(const color& a, float fuzz) : albedo(a), fuzz(fuzz) {}

	// always reflect 
	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& ray_emmitted)    const override {

		vec3 ray_reflected = reflect_with_fuzz(rec.normal, r_in.dir, fuzz);
		ray_emmitted = ray(rec.p, ray_reflected);
		attenuation = albedo;
		return true;

	}

};
