#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class hit_record {
public:
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    // we chose to always have normals point against the ray , this function ajust the normal depending on the ray
    // outward normal is the normal always pointing out , used to know whether hit from inside or outside 
    void set_face_normal(const ray& r, const vec3& outward_normal) {
        
        // if ray and outward normal points to different direction , it means hit from outside
       // front face = true else false 
        
        front_face = dot(r.dir, outward_normal) < 0; 
     
        // if hit from inside (outward normal and ray point to same direction), front face is false , so normal takes -outward normal
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif