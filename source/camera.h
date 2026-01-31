/* date = January 31st 2026 6:49 pm */

struct camera_t
{
	vec3_t position;
	vec3_t target;
	vec3_t up;
	
	f32 fov; // radians
	f32 near_z;
	f32 far_z;	
};

