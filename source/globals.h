/* date = January 22nd 2026 6:27 pm */


///////// Global variables along the engine ////////

struct engine_t;
global engine_t *g_engine;

struct camera_t;
global camera_t* g_engine_camera;


struct render_mailbox_t;
global render_mailbox_t *g_render_mailbox;


struct scene_t;
global scene_t *g_scene;

struct reserver_t;
global reserver_t *g_engine_reserver;
global reserver_t *g_render_reserver;


#define MAX_FPS 120

// COMPONENTS
#define MAX_COMPONENTS_PER_ACTOR 15
#define MAX_ACTORS 20
#define MAX_COMPONENT_SIZE 128
