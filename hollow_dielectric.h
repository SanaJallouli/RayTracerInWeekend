#pragma once
#include "material.h"
#include "hittable.h"

class material;





class hollow_dielectric : public material
{
public:
	color albedo;
	double refraction_index;
	hollow_dielectric(color albedo, double refraction_index) :albedo(albedo), refraction_index(refraction_index) {};


	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& ray_emmitted) const override {


		// assume one of the two medium is air with refraction index = 1
		// if ray is comming from air to object then the ray hit the hittable from outside and the index ratio is where_ray-comming/where_ray_entered

		// if it is hollow, there will be a thin material so the first ray will enter from air to the material, then be refracted from the material to the inside of the sphere that is air again 
		// the second refracted ray will then go from air to material again, then be refracted from material to air to leave the sphere 
		// so everytime ray hit sphere , it will hit enter from air to material then material to air 
		
// so even if hit from inside the ratio is air/ index 
		auto refraction_ratio = 1.0 / refraction_index;
	/*	if (!rec.front_face)
			refraction_ratio = refraction_index;// ray is from inside the hittable indx_inside/index_air 

	*/
		auto cos_theta = dot(-unit_vector(r_in.dir), rec.normal); // r and n are not in same direction, do not forget to flip r to find cos 

		auto sin_theta = sqrt(1 - cos_theta * cos_theta);

		if ((sin_theta * refraction_ratio > 1) || (reflectance(cos_theta, refraction_ratio) > random_double())) // this means no solution possible 
		{
			// reflect this ray 
			vec3 ray_reflected = reflect(unit_vector(r_in.dir), rec.normal); // rec.normal is always set as unit_vector already
			ray_emmitted = ray(rec.p, unit_vector(ray_reflected));
			attenuation = albedo;
			
		}
		else {

			vec3 ray_refracted = refract(unit_vector(r_in.dir), rec.normal, refraction_ratio);
			ray_emmitted = ray(rec.p, unit_vector(ray_refracted));
			attenuation = albedo;
			return true;
		}

	// to be continued !


	}
};
