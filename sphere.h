#pragma once
#include "hittable.h"

#include "interval.h"
class material;

class sphere : public hittable
{
public : 
	point3 center_sphere; 
	float radius;
material* mat;

	sphere():radius(0), center_sphere(vec3(0,0,0)) {};
	sphere(point3 _center, double _radius)
		: center_sphere(_center), radius(_radius){}
	sphere(point3 _center, double _radius, material* _material)
		: center_sphere(_center), radius(_radius), mat(_material) {}

	bool hit_with_interval(const ray& r, interval ray_t_interval_to_consider, hit_record& rec) const override {

		// hits == there is a position t along the ray t where the ray intersects the sphere 
		// t solves the equation of sphere == equation of ray 
		// t solves (P(t)-C).(P(t)-C)= r^2
		auto a = dot(r.dir, r.dir);
		auto b = dot((2 * r.dir), (r.orig - center_sphere));
		auto c = dot((r.orig - center_sphere), (r.orig - center_sphere)) - radius * radius;

		// such solution does not exist if delta is negatif
		auto delta = (b * b - 4 * a * c);
		// delta < 0 , there is no solution , there is no t along the ray where the ray could have hit the sphere 
		if (delta < 0) return false;
		if (delta == 0) // there is only one t along the ray where the intersection can happens 
		{
			// for this delta , the position t along the ray is :
			double  t = (-b - sqrt(delta)) / (2 * a);

			// the coordinates of the point where the ray hits the sphere is :
			point3 p = r.at(t); // origin_of_ray+ t*direction_ray 

			// need to make sure t is in the interval we are considering 
			if (ray_t_interval_to_consider.contains(t)) {
				rec.t = t;

				// this the real normal of the sphere , uniformized to length 1 
				vec3 outward_normal = unit_vector(center_sphere - p);

				rec.set_face_normal(r, outward_normal); // normal always points against the ray, to know if we hit from inside or outside . we compare the outward normal with the normal , if they are in the same direction , flip the normal and set face as false (meaning from inside)
				return true;
			}
		}

		if (delta > 0) // there is two solutions 
		{
			// for this delta , the position t along the ray is :
			double  t = (-b - sqrt(delta)) / (2 * a); // start by traking the closest one (smallest t)
			// check if t is in the considered interval 

			if (ray_t_interval_to_consider.contains(t))
			{
				// the coordinates of the point where the ray hits the sphere is :
				point3 p = r.at(t); // origin_of_ray+ t*direction_ray 

				rec.t = t;
				vec3 outward_normal = unit_vector(center_sphere - p);
				rec.set_face_normal(r, outward_normal);
				rec.mat = mat;
				return true;
			}
			else {
				t = t = (-b + sqrt(delta)) / (2 * a); // try with second solution 
				if (ray_t_interval_to_consider.contains(t))
				{
					// the coordinates of the point where the ray hits the sphere is :
					point3 p = r.at(t); // origin_of_ray+ t*direction_ray 

					rec.t = t;
					vec3 outward_normal = unit_vector(center_sphere - p);
					rec.set_face_normal(r, outward_normal);
					rec.mat = mat;
					return true;
				}
			 return false;
			}


		}

	}

	bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override
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
			rec.mat = mat;
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
			vec3 outward = unit_vector(rec.p - center_sphere);
			rec.set_face_normal(r, outward);
			rec.mat = mat;
			if (!rec.front_face) 
				std::clog << "from inside ";

			return true;
		}

	}


};

