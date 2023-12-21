#pragma once

#include "vec3.h"

class ray
{

public:
	// what defines the ray is it s origin and direction 
	point3 orig; 
	vec3 dir;

	ray() {};
	ray(point3 origin, vec3 direction) : orig{ origin }, dir{ direction }{};

	point3 origin() const { return orig; }
	vec3 direction() const { return dir; }

	point3 at(double t) const { // moves the point along the ray : what color is seen along the ray 
		return orig + t * dir; 
	}

};

