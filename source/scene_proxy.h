/* date = February 22nd 2026 8:06 pm */


/// Data structure that is handled by the render thread so we can manage the scene data and render it.
struct scene_proxy_t
{			
	transform_t transform;	
	u32 render_flags;
};


