/* date = February 24th 2026 6:04 pm */

////////////////////
//// This struct is owned by the render thread and severs us to take a look at the current scene
//// that is being renderered.

// TODO: Remove a specific scene proxy from the scene.

struct scene_t
{
	// TODO: We will track the free scene proxies in the scene somehow.
	scene_proxy_t* scene_proxies;
	u32 current_scene_proxies;
	u32 max_scene_proxies;	
};/* date = February 24th 2026 6:06 pm */

#ifndef SCENE_H
#define SCENE_H

#endif //SCENE_H
