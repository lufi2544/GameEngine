/* date = January 23rd 2026 1:35 pm */


#include "d3d11.h"
#include "d3dcompiler.h"


//////// This the windows platform renderer, for now I have a list of gpu_mesh_t that will be the real GPU data that we will put to the GPU for it to render.
///  The idea in the future is to have a real pipeline, where we can remov and add meshes at runtime( mark them as stale or something ), and a proper memory
///  managment for the CPU side( right now I have a list_t for the gpu_mesh_t, but this is not optimal, since we intend to have contiguous memory on this, maybe array? or index hash map to a preallocated array or something... )
///////

// TODO: move this to another .h
struct camera_t
{
	vec3_t position;
	vec3_t target;
	vec3_t up;
	
	f32 fov; // radians
	f32 near_z;
	f32 far_z;	
};

global camera_t g_engine_camera;

struct vertex_t
{
	f32 x, y, z;
	f32 r, g, b, a;
};

struct gpu_mesh_t
{
	ID3D11Buffer* vertex_buffer;
	ID3D11Buffer* index_buffer;
	
	u32 index_count;	
};

global gpu_mesh_t g_test_gpu_mesh;

struct gpu_vertex_t
{
	f32 x, y, z;
	f32 r, g, b, a;
};


struct renderer_t
{
	ID3D11Device* device;
	ID3D11DeviceContext *context;
	IDXGISwapChain *swap_chain;
	ID3D11RenderTargetView *target_view;
	ID3D11InputLayout* input_layout;
	
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	
	ID3D11Texture2D *depth_buffer;
	ID3D11DepthStencilView *depth_view;
	ID3D11DepthStencilState* ds_wireframe_overlay;
	
	// mvp -> model/view/projection
	ID3D11Buffer *cb_mvp;	
	ID3D11Buffer *cb_debug;
	
	ID3D11RasterizerState *rs_wireframe;
	ID3D11RasterizerState *rs_solid;	
	
	list_t gpu_meshes; // gpu_mesh_t
};

// TODO move this elsewhere
global u32 g_mesh_num;

// buffer model - view - projection
struct constant_buffer_mvp
{
	float mvp[16];
};

struct constant_buffer_debug
{
	f32 debugColor[4];
	int useDebugColor;
	f32 padding[3]; // 16-byte alingment TODO:WHY
};


global renderer_t g_renderer;

struct renderer_init_params
{
	HWND window_handle;
	u32 width;
	u32 height;
};



internal_f
bool RenderInitWindows(renderer_t *_renderer, renderer_init_params _params)
{
	DXGI_SWAP_CHAIN_DESC swap_chain_description = {};
	swap_chain_description.BufferCount = 1;
	swap_chain_description.BufferDesc.Width = _params.width;
	swap_chain_description.BufferDesc.Height = _params.height;
	swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	
	swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_description.OutputWindow = _params.window_handle;
	swap_chain_description.SampleDesc.Count = 1;
	swap_chain_description.Windowed = 1;
	swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	
	
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
											   0, 
											   D3D_DRIVER_TYPE_HARDWARE,
											   0,
											   0,
											   0,
											   0,
											   D3D11_SDK_VERSION,
											   &swap_chain_description,
											   &_renderer->swap_chain,
											   &_renderer->device,
											   0,
											   &_renderer->context
											   );
	
	
	
	
	if(FAILED(hr))
	{
		return false;
	}
	
	// Rasterizer state for debugging faces.
	// Solid(normal)
	{
		D3D11_RASTERIZER_DESC rs = {};
		rs.FillMode = D3D11_FILL_SOLID;
		rs.CullMode = D3D11_CULL_NONE; 
		rs.DepthClipEnable = TRUE;
		
		_renderer->device->CreateRasterizerState(&rs, &_renderer->rs_solid);
	}
	
	// Wireframe (debug)
	{
		
		D3D11_RASTERIZER_DESC rs = {};
		rs.FillMode = D3D11_FILL_WIREFRAME;
		rs.CullMode = D3D11_CULL_NONE;
		rs.DepthClipEnable = TRUE;
		
		_renderer->device->CreateRasterizerState(&rs, &_renderer->rs_wireframe);
								
	}
	
	// Depth stencil for wireframe overlay
	{
		D3D11_DEPTH_STENCIL_DESC ds = {};
		ds.DepthEnable = TRUE;
		ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		
		_renderer->device->CreateDepthStencilState(&ds, &_renderer->ds_wireframe_overlay);
	}
	
	
	// Target view
	
	
	// Get the memory for the entire screen.
	ID3D11Texture2D* backbuffer = 0;
	_renderer->swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);	
	
	// create a render view target for this memory, so we can modify it.
	_renderer->device->CreateRenderTargetView(backbuffer, 0, &_renderer->target_view);
	
	// Decrease the memory reference count 
	backbuffer->Release();
	
	
	D3D11_TEXTURE2D_DESC depth_desc = { };
	depth_desc.Width = _params.width;
	depth_desc.Height = _params.height;
	depth_desc.MipLevels = 1;
	depth_desc.ArraySize = 1;
	depth_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_desc.SampleDesc.Count = 1;
	depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	
	_renderer->device->CreateTexture2D(&depth_desc, 0, &_renderer->depth_buffer);
	_renderer->device->CreateDepthStencilView(_renderer->depth_buffer, 0, &_renderer->depth_view);
	
	// Bind target view + depgh stencil view(DSV) to the Output Merger(final pixels buffer)
	// target view -> back buffer | depth_view -> depth_buffer
	_renderer->context->OMSetRenderTargets(1, &_renderer->target_view, _renderer->depth_view);
	
	// Viewport // TODO: resizing?
	D3D11_VIEWPORT vp = { };
	vp.Width =(FLOAT)_params.width;
	vp.Height =(FLOAT)_params.height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	
	_renderer->context->RSSetViewports(1, &vp);
	
	// Model-View-Projection
	D3D11_BUFFER_DESC constant_buffer_desc = {};
	constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	constant_buffer_desc.ByteWidth = sizeof(constant_buffer_mvp);
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	_renderer->device->CreateBuffer(&constant_buffer_desc, 0, &_renderer->cb_mvp);	
	
	
	// Debug constant buffer for wireframe
	D3D11_BUFFER_DESC debug_desc = {};
	debug_desc.Usage = D3D11_USAGE_DYNAMIC;
	debug_desc.ByteWidth = sizeof(constant_buffer_debug);
	debug_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	debug_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				
	_renderer->device->CreateBuffer(&debug_desc, 0, &_renderer->cb_debug);
	
	
	
	ID3DBlob *vs_blob = 0;
	ID3DBlob *ps_blob = 0;
	
	D3DCompileFromFile(L"triangle_vs.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vs_blob, 0);
	D3DCompileFromFile(L"triangle_ps.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &ps_blob, 0);

	
	_renderer->device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), 0, &_renderer->vertex_shader);	
	_renderer->device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), 0, &_renderer->pixel_shader);
		
	// we use a input description to create an input layout
	// TODO: cache every input layout description and cache it???
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
		
	_renderer->device->CreateInputLayout(
										 layout, 
										 2, 
										 vs_blob->GetBufferPointer(),
										 vs_blob->GetBufferSize(),
										 &_renderer->input_layout
										 );
	
	vs_blob->Release();
	ps_blob->Release();
			
	return true;	
}

global_f bool
RendererInit()
{
	renderer_init_params params;
	params.window_handle = g_engine->main_window.handle;
	params.width = 800;
	params.height = 600;
	
	return RenderInitWindows(&g_renderer, params);
}


internal_f gpu_mesh_t
RendererCreateMeshFromasset(renderer_t *r, mesh_t *asset)
{
	SCRATCH();
	
	gpu_mesh_t result = {};
	
	// Build the Vertex Buffer
	
	gpu_vertex_t* temp_verteces = (gpu_vertex_t*)push_array(temp_arena, asset->vertex_num, gpu_vertex_t);
	
	
	for(u32 i = 0; i < asset->vertex_num; ++i)
	{
		temp_verteces[i].x = asset->verteces[i].x;
		temp_verteces[i].y = asset->verteces[i].y;
		temp_verteces[i].z = asset->verteces[i].z;
		
		temp_verteces[i].r = 1;
		temp_verteces[i].g = 1;
		temp_verteces[i].b = 1;
		temp_verteces[i].a = 1;		
	}
	
	D3D11_BUFFER_DESC vb_desc = {};
	vb_desc.Usage = D3D11_USAGE_DEFAULT;
	vb_desc.ByteWidth = asset->vertex_num * sizeof(gpu_vertex_t);
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA vb_data = {};
	vb_data.pSysMem = temp_verteces;
	
	r->device->CreateBuffer(&vb_desc, &vb_data, &result.vertex_buffer);
	
	
	// Build index buffer from faces
	
	u32 index_count = asset->face_num * 3;
	u32 *temp_indeces = (u32*)push_array(temp_arena, index_count, u32);
	
	
	for(u32 i = 0; i < asset->face_num; ++i)
	{
		face_t *f = &asset->faces[i];
		
		temp_indeces[i * 3 + 0] = f->a;
		temp_indeces[i * 3 + 1] = f->b;
		temp_indeces[i * 3 + 2] = f->c;
	}
	
	D3D11_BUFFER_DESC ib_desc = {};
	ib_desc.Usage = D3D11_USAGE_DEFAULT;
	ib_desc.ByteWidth = index_count * sizeof(u32);
	ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	
	D3D11_SUBRESOURCE_DATA ib_data = {};
	ib_data.pSysMem = temp_indeces;
	
	r->device->CreateBuffer(&ib_desc, &ib_data, &result.index_buffer);
	
	result.index_count = index_count;
	
	
	g_mesh_num++;
	
	return result;	
}

global_f void
RendererComputeImportedMesh(mesh_t *_mesh, engine_shared_data_t *engine_data)
{
	renderer_t *renderer = &g_renderer;
	gpu_mesh_t gpu_mesh = RendererCreateMeshFromasset(renderer, _mesh);
	LIST_ADD(&engine_data->memory->permanent, renderer->gpu_meshes, gpu_mesh, gpu_mesh_t);		
}


internal_f void
SetDebugColor(renderer_t* renderer, f32 r, f32 g, f32 b, f32 a, int use)
{
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    renderer->context->Map(renderer->cb_debug, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	
    constant_buffer_debug* cb = (constant_buffer_debug*)mapped.pData;
    cb->debugColor[0] = r;
    cb->debugColor[1] = g;
    cb->debugColor[2] = b;
    cb->debugColor[3] = a;
    cb->useDebugColor = use;
	
    renderer->context->Unmap(renderer->cb_debug, 0);
}

internal_f void
RenderGPUMesh(renderer_t *renderer, gpu_mesh_t* mesh, transform_t *transform, f32* projTview)
{
	UINT stride = sizeof(gpu_vertex_t);
	UINT offset = 0;
			
	// Bind geometry
    renderer->context->IASetVertexBuffers(0, 1, &mesh->vertex_buffer, &stride, &offset);
    renderer->context->IASetIndexBuffer(mesh->index_buffer, DXGI_FORMAT_R32_UINT, 0);
	
    // Build world matrix from transform
    f32 world_matrix[16];
    f32 vp[16];
    f32 mvp[16];
		
    Mat4Identity(world_matrix);
	
	// Getting the transform matrix from local space to world space - 
	// (Column major) World = Transform * Rotation * Scale
    TransformToMatrix(world_matrix, *transform);
		
    //(Column major) world_matrix(mvp) = (proj * view * world)
    Mat4Mul(mvp, projTview, world_matrix);
	
	//////////////////////
	/// Whenever we use the Map function, we basically get the GPU memory and do stuff with it, in this case we are passing the 
	/// model - view - projection matrix to the vertex shader, so in the vertex shader, we can use it in parallel for every vertex.
	/////////////////////
	
    // Upload constant buffer
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    renderer->context->Map(renderer->cb_mvp, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	
    constant_buffer_mvp* cb = (constant_buffer_mvp*)mapped.pData;
    bytes_copy(cb->mvp, mvp, sizeof(mvp));
	
    renderer->context->Unmap(renderer->cb_mvp, 0);
	
	// Basically setting the params to the registers passed to the shader
    renderer->context->VSSetConstantBuffers(0, 1, &renderer->cb_mvp);
	renderer->context->PSSetConstantBuffers(1, 1, &renderer->cb_debug);
	
    // Draw
    renderer->context->DrawIndexed(mesh->index_count, 0, 0);
}

internal_f void
BeginFrame(renderer_t *_renderer)
{	
	float clear_color[4] = { 0.1f, 0.2f, 0.4f, 1.0f };
	_renderer->context->ClearRenderTargetView(_renderer->target_view, clear_color);
	
	_renderer->context->ClearDepthStencilView(
											  _renderer->depth_view,
											  D3D11_CLEAR_DEPTH,
											  1.0f,
											  0
											  );
	
	_renderer->context->IASetInputLayout(_renderer->input_layout);	
	_renderer->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // Treat the input as triagles	

	
	// Vertex shader
	_renderer->context->VSSetShader(_renderer->vertex_shader, 0, 0);
	
	// Pixel shader
	_renderer->context->PSSetShader(_renderer->pixel_shader, 0, 0);	
}



internal_f void
EndFrame(renderer_t *_renderer)
{
	_renderer->swap_chain->Present(1, 0);
}


internal_f void
RenderMeshes(renderer_t *renderer)
{
	float view[16];
    float proj[16];
	
	Mat4LookAtLH(view,
                 g_engine_camera.position,
                 g_engine_camera.target,
                 g_engine_camera.up);
	
	//TODO: get from the main window
    float aspect = 800.0f / 600.0f;
    Mat4PerspectiveLH(proj,
                      g_engine_camera.fov,
                      aspect,
                      g_engine_camera.near_z,
                      g_engine_camera.far_z);
	
	// we will just render the list in this case.For now this is what I have, is not cache friendly neither optimal, but is fine for now.	
	gpu_mesh_t *mesh = 0;
	LIST_FOREACH(gpu_mesh_t, mesh, g_renderer.gpu_meshes)
	{
		transform_t transform = {};
		// TODO: we would need to handle some way the transform from this, from cpu to gpu.
		transform.position = {0, 0, 0};
		transform.scale = {1, 1, 1};
		
		static float angle = 0.0f;
		angle += 0.01f;
		
		transform.rotation = {0, angle, 0};
		
		// SOLID PASS
		renderer->context->RSSetState(renderer->rs_solid);
		renderer->context->OMSetDepthStencilState(nullptr, 0);
		SetDebugColor(renderer, 0, 0, 0, 0, 0); // use vertex color
		
		f32 projTview [16];
		Mat4Mul(projTview, proj, view);
		RenderGPUMesh(renderer, mesh, &transform, projTview);
		
		// WIREFRAME PASS
		renderer->context->RSSetState(renderer->rs_wireframe);
		renderer->context->OMSetDepthStencilState(renderer->ds_wireframe_overlay, 0);
		SetDebugColor(renderer, 0, 0, 0, 1, 1); // white 
		RenderGPUMesh(renderer, mesh, &transform, projTview);
		
		renderer->context->OMSetDepthStencilState(nullptr, 0);
		
	}
}


global_f void
RendererUpdate(engine_shared_data_t *engine_data)
{
	BeginFrame(&g_renderer);
	
	RenderMeshes(&g_renderer);
	
	EndFrame(&g_renderer);
}
