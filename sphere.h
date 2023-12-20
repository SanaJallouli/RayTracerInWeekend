#pragma once
#include "hittable.h"


class sphere : hittable
{
public : 
	point3 center_sphere; 
	float radius;


	sphere() {};

	sphere(point3 center, float radius) : center_sphere(center), radius(radius) {
	};


	bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const 
	{
		// hit means that there is a t along the ray r such that (P(t)-C).(P(t)-C)= r^2
		
		auto a = dot(r.dir, r.dir);
		auto b = dot((2 * r.dir), (r.orig - center_sphere));
		auto c = dot((r.orig - center_sphere), (r.orig - center_sphere)) - radius * radius;

		// such solution does not exist if delta is negatif
		auto delta = (b * b - 4 * a * c);

		if (delta < (double)0) return false;
		else if (delta == 0) {

			double  t = -b / (2 * a);

			if (t <= ray_tmin || ray_tmax <= t) return false;
		//where it hit, is the t along the ray that resolved the equation 
			rec.t = t;
		
		// find the unified normal : the normal is in the direction of the vector (hit)-(center_sphere)
			rec.p = r.at(rec.t);
			rec.normal = unit_vector(rec.p - center_sphere);

			return true;
		}
		else if (delta > (double)0) {  // Find the nearest root 
			//where it hit, is the t along the ray that resolved the equation 
			double  t = (-b - sqrt(delta)) / (2 * a);
			if (t <= ray_tmin || ray_tmax <= t)
			 	t = (-b + sqrt(delta)) / (2 * a);
			if (t <= ray_tmin || ray_tmax <= t) return false;
			// find the unified normal : the normal is in the direction of the vector (hit)-(center_sphere)
			rec.t = t;
			rec.p = r.at(rec.t);
			rec.normal = unit_vector(rec.p - center_sphere);
			return true;
		}

	}


};

