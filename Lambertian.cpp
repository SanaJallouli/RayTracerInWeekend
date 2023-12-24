
#pragma once
#include "Lambertian.h"

#include "material.h"
#include "hittable.h"
class material;



class Lambertian : public material
{
public:
	color albedo; // the base color of the material 
	Lambertian(const color& a) : albedo(a) {}
	// always scatter 
	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& ray_emmitted) const override {

		vec3 ray_emitting = random_unit_on_hemisphere_lambertian(rec.normal);


		// Catch degenerate scatter direction
		if (ray_emitting.near_zero())
			ray_emitting = rec.normal;

		ray_emmitted = ray(rec.p, ray_emitting);
		attenuation = albedo;
		return true;

	}

};
