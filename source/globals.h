/* date = January 22nd 2026 6:27 pm */


///////// Global variables along the engine ////////

struct engine_t;
global engine_t *g_engine;

struct camera_t;
global camera_t* g_engine_camera;


struct render_mailbox_t;
global render_mailbox_t *g_render_mailbox;


#define MAX_MESH_COUNT 10
#define MAX_FPS 60