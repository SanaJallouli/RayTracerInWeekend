#pragma once

#include "ray.h"
#include "vec3.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "interval.h"

using color = vec3;

color ray_color(const ray& r) { // shade of blue : depend on the y coord of the ray
    vec3 unit = unit_vector(r.dir);
    float y_coord = 0.5 * (unit.y() + 1.0);
    auto initial_color = vec3(1, 1, 1);
    auto last_color = vec3(0, 0, 1);
    auto blend = (1 - y_coord) * initial_color + last_color * y_coord;
    return blend;
}

bool hits_sphere(const ray& r) {
    // hits sphere means that there is at least one solution for the eqution : (P(r.at(t))-C).(P(t)-C)= r^2 
    // place a sphere in -1 on the z axis , of radius 1
    auto center_sphere = vec3(0, 0, -1); // even if we put it behind the camera, we end up with the same result, our code does not distinguish yet in front of camera or not
    auto a = dot(r.dir, r.dir);
    auto b = dot((2 * r.dir), (r.orig - center_sphere));
    auto c = dot((r.orig - center_sphere), (r.orig - center_sphere)) - 0.1 * 0.1;// 
    auto sqrtt = (b * b - 4 * a * c);
    if (sqrtt < (double)0) { return false; }
    else return true;
}

color ray_color_sphere(const ray& r) {
    // place a sphere in -1 on the z axis , of radius 1
    if (hits_sphere(r)) return vec3(1, 0, 0); // if there is a point t along that r that hits the sphere 
    else return ray_color(r);
}

color hits_sphere_hit_point(const ray& r) {
    // hits sphere means that there is at least one solution for the eqution : (P(r.at(t))-C).(P(t)-C)= r^2 
    // place a sphere in -1 on the z axis , of radius 1
    auto center_sphere = vec3(0, 0, -1); // even if we put it behind the camera, we end up with the same result, our code does not distinguish yet in front of camera or not
    auto a = dot(r.dir, r.dir);
    auto b = dot((2 * r.dir), (r.orig - center_sphere));
    auto c = dot((r.orig - center_sphere), (r.orig - center_sphere)) - 0.1 * 0.1;// 
    auto delta = (b * b - 4 * a * c);

    if (delta < (double)0) { return ray_color(r); }
    else {
        double t = (-b - sqrt(delta)) / (2 * a);
        point3 p = r.at(t);// point at t along the ray, where t hits the sphere
        vec3 normal = unit_vector(p - center_sphere);
        // map normals from [-1..1] to [0..1] 
        return 0.5 * color(normal.x() + 1, normal.y() + 1, normal.z() + 1);
    };

}

color hits_hittable_list_with_interval(hittable_list hittables, const ray& r, interval interval_t_to_consider) {
    hit_record hit_info;
    if (hittables.hit_with_interval(r, interval_t_to_consider, hit_info)) {
        return 0.5 * color(hit_info.normal.x() + 1, hit_info.normal.y() + 1, hit_info.normal.z() + 1);
    }
    else  return ray_color(r);
}


color hits_hittable_list(hittable_list hittables, const ray&r, double min , double max) {
    hit_record hit_info;
   if (hittables.hit(r, min, max , hit_info)) // this will loop over the hittable, and return the info on the hit with the nearest object to the camera if any. 
   { // there was a hit, hit info contains the info on the hit with the nraset object 
     // take the info on the normal (already unit 1) to use it as color.
    // transpose normal from [-1..1] to [0..1]
       return 0.5 * color(hit_info.normal.x() + 1, hit_info.normal.y() + 1, hit_info.normal.z() + 1);
   }
   else  return ray_color(r);

}


color hits_sphere_use_hittable(const ray& r, sphere s, double min, double max) {
    hit_record hit_info;
    if (s.hit(r, min, max, hit_info))
        return 0.5 * color(hit_info.normal.x() + 1, hit_info.normal.y() + 1, hit_info.normal.z() + 1);
    else return ray_color(r);
}

void write_color(std::ostream& out, color pixel_color) {
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}
int main() {

    // set up image dimensions 
    auto aspect_ratio = 16.0 / 9.0; // ideal value , the real aspect_ratio can change b/c of rounding or if height is set to 1 
    int image_width = 400;
    int image_height = image_width / aspect_ratio;
    image_height = (image_height < 1) ? 1 : image_height;


    // set up viewport : 
    // Viewport widths less than one are ok since they are real valued.
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height); // need the aspect ratio of viewport to match exactly the one of the output image : use the actual aspect ratio not the ideal ones (in case it changed due to rounding or height was <0 )


    // set up the camera 

    point3 camera_center = vec3(0, 0, 0);
    auto focal_length = 1; // distance between camera center and the viewport 

    // describe the viewport in terms of V and U (horizontal span, vertical span ), and delta U,V 
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);


    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u / image_width;// width of pixel in viewport or their spacing ?
    auto pixel_delta_v = viewport_v / image_height;// height of pixel in viewport 

    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;

    //pixel 0,0 is not in the edge of the view port but 0.5*delta_u right and 0.5*delta_v down  

    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);



    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

        for (int i = 0; i < image_width; ++i) {

            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            //1
           // color pixel_color = ray_color_sphere(r); // just output red if hits = red circle
            

            //2
           //  color pixel_color = hits_sphere_hit_point(r); // output the surface normals where it hits = shades
            

            //3
            /*
            sphere s(vec3(0, 0, -1), 0.5);
            double min = 0.001;
            double max =  0.7;
            color pixel_color = hits_sphere_use_hittable(r, s, min, max);
            */

            // 4
            // use list of hittable objects 
         /*   hittable_list myhittables;
            auto sphere1 = std::make_shared<sphere>(vec3(0, 0, -1), 0.3);
            auto eye = std::make_shared<sphere>(vec3(-0.15, 0.15, -0.7), 0.1);
            auto eye2 = std::make_shared<sphere>(vec3(0.15, 0.15, -0.7), 0.1);
            auto sphere2 = std::make_shared<sphere>(vec3(0, -(viewport_height/2), -1), 0.7);
            myhittables.add(sphere1);
            myhittables.add(sphere2);
            myhittables.add(eye);
            myhittables.add(eye2);
            double min = 0.001; // if the ray hits the hittable at a position in the ray (t) that is smaller than min, do not consider that hit. if you are too close to the center emitting the ray, do not consider the hit, see if there is another hit along a further position in the ray. if such 2nd hit exist, it is likely that we are hitting the object from inside this time. 
            double max = 10; // if the ray hits the hittable at a position in the ray (t) further than max, do not concider the hit. see if we hit at a position that is closer to center 
           
            //color pixel_color = hits_hittable_list(myhittables,r, min, max);
            */


            // 5- use interval class 
            hittable_list myhittables;
            auto sphere1 = std::make_shared<sphere>(vec3(0, 0, -1), 0.3);
            auto sphere2 = std::make_shared<sphere>(vec3(0, -(viewport_height / 2), -1), 0.7);
            myhittables.add(sphere1);
            myhittables.add(sphere2);
        
            double min = 0.001; // if the ray hits the hittable at a position in the ray (t) that is smaller than min, do not consider that hit. if you are too close to the center emitting the ray, do not consider the hit, see if there is another hit along a further position in the ray. if such 2nd hit exist, it is likely that we are hitting the object from inside this time. 
            double max = 10; // if the ray hits the hittable at a position in the ray (t) further than max, do not concider the hit. see if we hit at a position that is closer to center 
            interval ray_position_to_consider(min, max);
           
          

            color pixel_color = hits_hittable_list_with_interval(myhittables, r, ray_position_to_consider);
            write_color(std::cout, pixel_color);
            //std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }


    std::clog << "\rDone.                 \n";
}