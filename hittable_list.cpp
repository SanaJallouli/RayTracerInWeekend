#pragma once
#include "hittable_list.h"

# include "hittable.h"
#include <memory>
#include <vector>
using namespace std;


class hittable_list : hittable
{
public:
	vector<shared_ptr<hittable>> objects;
	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }

	void add(shared_ptr<hittable> object) {
		objects.push_back(object);
	}

	bool hit_with_interval(const ray& r, interval interval_considered, hit_record& rec) const override {
		bool hitted = false;
		rec.t = interval_considered.max;
		for (auto obj : objects) {
			hit_record temp_rec;
			if (obj->hit_with_interval(r, interval_considered, temp_rec)) {
				hitted = true;
				if (temp_rec.t < rec.t) {
					rec = temp_rec;
				}
			}

		}
		return hitted;
	}


	// we have a ray, and we will see if for that ray, there is one hittable object along its way that is hit. 
	// loop over all the hittable objects we have registered (in our list) and call their hit method. 
	// the hit method of an object will rturn true if it was hit by that ray (if there is a t along the ray that solves the equation defining the intersection of ray-object).
	// the hit method records the position of the hit, t, the normal (always against the ray), hitting from inside or outside
	bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override
	{
		// each type of hittable object defines its own hit method. polymorphically call the right one. 
		// we are calling hit for the same ray 
		// the ray can hit many things on its way 
		// in that case the thing that should be recorded (to then appear on the viewport) is the closest one it hits 
		// if something is hit , we will return true 

		bool hitted = false;
		// ray_tmax : furtherest point along the ray, for which we will consider the hit. 
		rec.t = ray_tmax; // start by this then see if we have a hit that is closer to the camera ; 

		for (const auto& obj : objects)
		{
			hit_record record_temp;
			if (obj->hit(r, ray_tmin, ray_tmax, record_temp)) {
				hitted = true;
				if (rec.t > record_temp.t) // we just hit a hittable in a point that is closer to the camera than the previous ones.  
					rec = record_temp; // our new hit record takes the record of the closest hittable to the camera 
			}


		};
		return hitted;
	}


};

