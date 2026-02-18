/* date = January 26th 2026 4:25 pm */

struct engine_shared_data_t;

global_f void
ApplicationInit(engine_shared_data_t *engine_data);


global_f void
ApplicationUpdate(engine_shared_data_t *engine_data, f32 dt);



global_f void
ApplicationShutDown(engine_shared_data_t *engine_data);