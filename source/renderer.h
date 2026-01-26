/* date = January 23rd 2026 4:35 pm */


// TODO: Add the mesh list to the engie_shared_data.

global_f bool
RendererInit();

global_f void
RendererUpdate(engine_shared_data_t *engine_data);

global_f void 
RendererComputeImportedMesh(mesh_t *_mesh);
