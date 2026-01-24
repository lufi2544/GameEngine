/* date = January 23rd 2026 1:35 pm */


#include "d3d11.h"
#include "d3dcompiler.h"


struct vertex
{
	f32 x, y, z;
	f32 r, g, b, a;
};


global vertex triangle_verteces[] = 
{
    { -1.0f,  1.0f, 0.0f,  1, 0, 0, 1 },
    {  1.0f, -1.0f, 0.0f,  0, 1, 0, 1 },
    { -1.0f, -1.0f, 0.0f,  0, 0, 1, 1 },
};


struct renderer
{
	ID3D11Device* device;
	ID3D11DeviceContext *context;
	IDXGISwapChain *swap_chain;
	ID3D11RenderTargetView *target_view;
	ID3D11Buffer *triangle_vertex_buffer;
	ID3D11InputLayout* input_layout;
	
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* pixel_shader;
	
	ID3D11Texture2D *depth_buffer;
	ID3D11DepthStencilView *depth_view;
	
	// mvp -> model/view/projection
	ID3D11Buffer *cb_mvp;
};

struct constant_buffer_mvp
{
	float mvp[16];
};


global renderer g_renderer;

struct renderer_init_params
{
	HWND window_handle;
	u32 width;
	u32 height;
};



internal_f
bool RenderInitWindows(renderer *_renderer, renderer_init_params _params)
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
	
	// Viewport
	D3D11_VIEWPORT vp = { };
	vp.Width =(FLOAT)_params.width;
	vp.Height =(FLOAT)_params.height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	
	_renderer->context->RSSetViewports(1, &vp);	
	
	D3D11_BUFFER_DESC vertex_buffer_description = {};
	vertex_buffer_description.Usage = D3D11_USAGE_DEFAULT;
	vertex_buffer_description.ByteWidth = sizeof(triangle_verteces);
	vertex_buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA vertex_buffer_data = { };
	vertex_buffer_data.pSysMem = triangle_verteces;
	
	HRESULT hr_vertex_buffer = _renderer->device->CreateBuffer(&vertex_buffer_description, &vertex_buffer_data, &_renderer->triangle_vertex_buffer);
	
	// Model-View-Projection
	D3D11_BUFFER_DESC constant_buffer_desc = {};
	constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	constant_buffer_desc.ByteWidth = sizeof(constant_buffer_mvp);
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	_renderer->device->CreateBuffer(&constant_buffer_desc, 0, &_renderer->cb_mvp);
	
	ID3DBlob *vs_blob = 0;
	ID3DBlob *ps_blob = 0;
	
	D3DCompileFromFile(L"triangle_vs.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vs_blob, 0);
	D3DCompileFromFile(L"triangle_ps.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &ps_blob, 0);

	
	_renderer->device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), 0, &_renderer->vertex_shader);	
	_renderer->device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), 0, &_renderer->pixel_shader);
	
	vs_blob->Release();
	ps_blob->Release();
	
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


internal_f void
BeginFrame(renderer *_renderer)
{
	float clear_color[4] = { 0.1f, 0.2f, 0.4f, 1.0f };
	_renderer->context->ClearRenderTargetView(_renderer->target_view, clear_color);
	
	_renderer->context->ClearDepthStencilView(
											  _renderer->depth_view,
											  D3D11_CLEAR_DEPTH,
											  1.0f,
											  0
											  );
	
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	
	// Input assembly
	_renderer->context->IASetVertexBuffers(0, 1, &_renderer->triangle_vertex_buffer, &stride, &offset);
	_renderer->context->IASetInputLayout(_renderer->input_layout);
	
	// Treat the input as triagles
	_renderer->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// Vertex shader
	_renderer->context->VSSetShader(_renderer->vertex_shader, 0, 0);
	
	// Pixel shader
	_renderer->context->PSSetShader(_renderer->pixel_shader, 0, 0);
	
	
	float mvp[16];
	
	bytes_set(mvp, 0, sizeof(mvp));
	mvp[0] = 1;
	mvp[5] = 1;
	mvp[10] = 1;
	mvp[15] = 1;
	
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	_renderer->context->Map(_renderer->cb_mvp, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	
	constant_buffer_mvp* cb = (constant_buffer_mvp*)(mapped.pData);
	bytes_copy(cb->mvp, mvp, sizeof(mvp));
	
	_renderer->context->Unmap(_renderer->cb_mvp, 0);
	
	_renderer->context->VSSetConstantBuffers(0, 1, &_renderer->cb_mvp);
	
	_renderer->context->Draw(3, 0);
}


internal_f void
EndFrame(renderer *_renderer)
{
	_renderer->swap_chain->Present(1, 0);
}


global_f void
RendererUpdate()
{
	BeginFrame(&g_renderer);
	
	EndFrame(&g_renderer);
}