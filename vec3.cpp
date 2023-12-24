#ifndef VEC3_H
#define VEC3_H
#pragma once
#include <cmath>
#include <iostream>
using std::sqrt;

#include "interval.h"

class vec3
{
public:
    double e[3];

    vec3() : e{ 0,0,0 } {}
    vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; } // return is l-value 

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1 / t;
    }

    double length() const {
        return sqrt(length_squared());
    }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using point3 = vec3;


// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
        + u.e[1] * v.e[1]
        + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

inline vec3 random() { // random double gives number between [0,1)
    return vec3(random_double(), random_double(), random_double());
}


inline vec3 random(double min, double max)
{// random_double(min, max) returns a random real in [min,max).
    return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline vec3 random_unit_in_unit_sphere() {

    while (true) {
        point3 temp(random_double(-1, 1), random_double(-1, 1), random_double(-1, 1));

        if (((temp.x()) * (temp.x()) + (temp.y()) * (temp.y()) + (temp.z()) * (temp.z())) < 1)
            // the point is inside 
            return unit_vector(temp);
    }
}

inline vec3 random_unit_on_hemisphere(vec3 normal) {
    auto temp = random_unit_in_unit_sphere();
    if (dot(temp, normal) > 0) return temp; // the normal to surface and the random ray look in same direction ;
    return -temp;
}

inline vec3 random_unit_on_hemisphere_lambertian(vec3 normal) {
    auto temp = random_unit_in_unit_sphere(); // unit so that when added we get the direction we want not influenced by the length 
    if (dot(temp, normal) > 0) return temp + normal; // the normal to surface and the random ray look in same direction ; 
    return -temp + normal;
}

inline vec3 reflect(vec3 normal, vec3 r)
{
    return r - (2 * normal * (dot(r, normal)));
}


inline vec3 reflect_with_fuzz(vec3 normal, vec3 r, float fuzz)
{
    vec3 rando = random(-fuzz, fuzz); // the book uses fuzz* random_unit_vector() which does not allow for negatif
    vec3 reflected = r - (2 * normal * (dot(r, normal)));

    return rando + reflected;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}


static double reflectance(double cosine, double ref_idx) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto temp = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (temp.length_squared() < 1)
            return temp;
    }
}

#endif