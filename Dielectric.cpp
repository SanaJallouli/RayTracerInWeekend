#pragma once
#include "dielectric.h"
#include "material.h"
#include "hittable.h"

class material;




class dielectric : public material
{
public:
	color albedo;
	double refraction_index;
	dielectric(color albedo, double refraction_index) :albedo(albedo), refraction_index(refraction_index) {};


	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& ray_emmitted) const override {


		// assume one of the two medium is air with refraction index = 1
		// if ray is comming from air to object then the ray hit the hittable from outside and the index ratio is where_ray-comming/where_ray_entered

		auto refraction_ratio = 1.0 / refraction_index;
		if (!rec.front_face)
			refraction_ratio = refraction_index;// ray is from inside the hittable indx_inside/index_air 


		auto cos_theta = fmin(dot(-unit_vector(r_in.dir), rec.normal), 1); // r and n are not in same direction, do not forget to flip r to find cos 

		auto sin_theta = sqrt(1 - cos_theta * cos_theta);

		if ((sin_theta * refraction_ratio > 1) || (reflectance(cos_theta, refraction_ratio) > random_double())) // this means no solution possible 
		{
			// reflect this ray 
			vec3 ray_reflected = reflect(unit_vector(r_in.dir), rec.normal); // rec.normal is always set as unit_vector already
			ray_emmitted = ray(rec.p, unit_vector(ray_reflected));
			attenuation = albedo;
			return true;
		}


		vec3 ray_refracted = refract(unit_vector(r_in.dir), rec.normal, refraction_ratio);
		ray_emmitted = ray(rec.p, unit_vector(ray_refracted));
		attenuation = albedo;
		return true;

	}
};
