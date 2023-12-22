#pragma once
#include "vec3.h"
#include "ray.h"
#include "hittable_list.h"
#include "material.h"
using color = vec3;
class hit_record;
class camera
{
public : 
	// camera 
	vec3 camera_center;
	double focal_legth;


	// viewport 
	vec3 viewport_center;
	vec3 viewport_corner;
	int viewport_width;
	int viewport_height ;
	
	vec3  viewport_delta_u;
	vec3 viewport_delta_v; 

	vec3 viewport_u;
	vec3 viewport_v;


	// pixels - image
	int aspect_ratio = 1 ; // this will be modifiable from main
	int image_width = 100 ;  // modifiable from main 
	int image_height;
	point3 postion_pixel_00;
	

	int num_sample = 10;
	int    max_depth = 200;
	void initialize() {

		// inintalize image 
		image_height = static_cast<int>(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		// camera 
		camera_center = vec3(0, 0, 0);
		focal_legth = 1; 
		
		// viewport 
		viewport_height = 2; 
		// aspect ratio may change because imahe_height was less than 1 or because of rounding 
		// need to use the actual aspect ratio so that viewport and image have the exact same aspect ratio. otherwise it will stretch or squiches the rendered image 
		auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height); // heigth * actual aspect ratio 
		viewport_center = vec3(0, 0, -focal_legth);

		//we add the camera_center in case we set one that is not at origin 
		viewport_corner =   camera_center+ viewport_center + vec3(-viewport_width / 2,0, 0) + vec3(0, viewport_height / 2, 0);


		viewport_u = vec3(viewport_width, 0, 0); // points to same direction as x 
		viewport_v = vec3(0, -viewport_height, 0); // points agains y 

		viewport_delta_u = viewport_u / image_width; // viewport_u is vector from left corner to right corner of viewport, to find the dlta between the pixels , we need to divide viewport_u/ number of pixels on the width which is the width of the image 
		viewport_delta_v = viewport_v / image_height;


		postion_pixel_00 = viewport_corner + viewport_delta_u/2 + viewport_delta_v/2;
	}
	void write_color(std::ostream& out, color pixel_color) {
		// Write the translated [0,255] value of each color component.
		interval intensity(0.000, 0.999);
		out << static_cast<int>(255.999 * linear_to_gamma(pixel_color.x())) << ' '
			<< static_cast<int>(255.999 * linear_to_gamma(pixel_color.y())) << ' '
			<< static_cast<int>(255.999 * linear_to_gamma(pixel_color.z()))<< '\n';
	}
	
	double linear_to_gamma(double x) {
		return sqrt(x);
	}

	color ray_color(const ray& r) { // shade of blue : depend on the y coord of the ray
		vec3 unit = unit_vector(r.dir);
		float y_coord = 0.5 * (unit.y() + 1.0); // with focal legth 1 and in case the ray is not emmitted, and there is not hit, the z will be null 
		auto initial_color = vec3(1, 1, 1);
		auto last_color = vec3(0, 0, 1);
		auto blend = (1 - y_coord) * initial_color + last_color * y_coord;
		return  blend;// 0.5 * color(unit.x() + 1, unit.y() + 1, unit.z());
	}

	color hits_hittable_list_with_interval(hittable_list hittables, const ray& r, interval interval_t_to_consider) {
		hit_record hit_info;
		if (hittables.hit_with_interval(r, interval_t_to_consider, hit_info)) {
			return 0.5 * color(hit_info.normal.x() + 1, hit_info.normal.y() + 1, hit_info.normal.z() + 1); 
		}
		return ray_color(r);
	}
	
	color hits_hittable_list_with_interval_diffuse_surface(hittable_list hittables, const ray& r, interval interval_t_to_consider, int max) {
		hit_record hit_info;
    
		// to avoid overflowing the stack or taking too long in case each new emmitted ray keeps hitting objects 
		if (max > 0) {
			if (hittables.hit_with_interval(r, interval_t_to_consider, hit_info))
			{// instead of taking the color of the normals 
			   // if there is a hit, we will generate a random emmitted ray
				   // we will in turn see if that ray hit an object
				vec3 ray_diffused = random_unit_on_hemisphere_lambertian(hit_info.normal);// the difused ray on the normal of the surface 
				// at some point, the generated ray will not hit anything and will return a color, this color will be attenuated by the factor (here 0.8 ) each time it hit an object 
				return 0.5 * hits_hittable_list_with_interval_diffuse_surface(hittables, ray(hit_info.p, ray_diffused), interval_t_to_consider, max -1);
			}
		}
		// the ray did not hit
		return ray_color(r); 

	}


	color hits_hittable_list_with_materials(hittable_list hittables, const ray& r, interval interval_t_to_consider, int max) {
		hit_record hit_info;

		// to avoid overflowing the stack or taking too long in case each new emmitted ray keeps hitting objects 
		if (max > 0) {
			if (hittables.hit_with_interval(r, interval_t_to_consider, hit_info))
			{
				// there were a hit at t along ray r , likely to emmit a ray back 
				// the material of the hittable object describes how the emmitted ray will be scattered
				// the material we just hitted is stored in the hit_info
				
				// the scatter function describes how the scattered ray will behave , it will need some info like p and normal that are stored in hit_info so pass it to it 
				ray scattered_ray;
				color attenuation;
				hit_info.mat->scatter(r, hit_info, attenuation, scattered_ray);
				// attenuation is the color of the material, if it is red(1,0,0) so it will make the scattered ray not attenuated at all on the x-axis, and remove from it the green and blue componenets , so it absorbed red and green so that it appears red .
					return attenuation * hits_hittable_list_with_materials(hittables, scattered_ray, interval_t_to_consider, max-1);
				
			}
		}
		// the ray did not hit
		return ray_color(r);

	}
	void render(hittable_list objects, interval ray_position_to_consider) {
		initialize();


		// p3 rendering :
// number of columns             ----- number of rows 
// number of pixels in the width ----- number of pixels in the
		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
 

// loop over all the pixels in height of the image  
		for (int j = 0; j < image_height; ++j)
		{
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			// loop over all the pixels in the width of the image 
			for (int i = 0; i < image_width; ++i) {

				point3 pixel_center = postion_pixel_00 + i * viewport_delta_u + j * viewport_delta_v;//vec3(postion_pixel_00.x() + i, postion_pixel_00.y() - j, - focal_legth);
				vec3 ray_direction = pixel_center - camera_center;
				ray r(camera_center, ray_direction); // ray takes the camera center and the direction of the ray 
				// the direction of the ray is the vector between the camera center and the pixel we are outputting its color 

				color pixel_color = hits_hittable_list_with_interval(objects, r, ray_position_to_consider );
				write_color(std::cout, pixel_color);
			

			}
		}
		std::clog << "\rDone.                 \n";
	}

	void render_withAntialiasing(hittable_list objects, interval ray_position_to_consider) {
		initialize();


		// p3 rendering :
// number of columns             ----- number of rows 
// number of pixels in the width ----- number of pixels in the
		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";


		// loop over all the pixels in height of the image  
		for (int j = 0; j < image_height; ++j)
		{
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			// loop over all the pixels in the width of the image 
			for (int i = 0; i < image_width; ++i) {

				point3 pixel_center = postion_pixel_00 + i * viewport_delta_u + j * viewport_delta_v;//vec3(postion_pixel_00.x() + i, postion_pixel_00.y() - j, - focal_legth);
				vec3 ray_direction = pixel_center - camera_center;
				ray r(camera_center, ray_direction); // ray takes the camera center and the direction of the ray 
				// the direction of the ray is the vector between the camera center and the pixel we are outputting its color

				color pixel_color = hits_hittable_list_with_interval(objects, r, ray_position_to_consider);

				auto sum_color = pixel_color;

				// we want to project many rays to the square centered in this pixel with a width of maximum delta_u (so 0.5 delta_u from the center)and max width delta_v 
				for (int k =0; k < num_sample; ++k) 
				{
					auto random_on_x = viewport_delta_u * (-0.5 + (random_double()));// random double gives us [0..1) we want to go from [-0.5 ..0.5)
						auto random_on_y = viewport_delta_v * (-0.5 + random_double());
						auto pixel_temp_position = pixel_center + random_on_x + random_on_y;
						ray ray_temp(camera_center, pixel_temp_position - camera_center);
						sum_color += hits_hittable_list_with_materials(objects, ray_temp, ray_position_to_consider, max_depth);
				}
				

				write_color(std::cout, sum_color/ (num_sample+1));


			}
		}
		std::clog << "\rDone.                 \n";
	}


};

