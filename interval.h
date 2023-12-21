#pragma once
#include "RayTracingInAWeekend.h"
class interval
{

public :

	double min; 
	double max; 


	interval() : min(-infinity), max(infinity) {};

	interval(double x, double y) : min(x), max(y) {};

	bool contains(double x) {
		return ((x >= min) && (x <= max));
	}

	bool surrounds(double x) const {
	return (min < x && x < max);
}

static const interval empty, universe;

};
const static interval empty(+infinity, -infinity);
const static interval universe(-infinity, +infinity);
