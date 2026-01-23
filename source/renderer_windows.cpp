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
	{  0.0f,  0.5f,  0.0f,    1,  0,  0,  1 }, // top (red)
    {  0.5f, -0.5f,  0.0f,    0,  1,  0,  1 }, // right (green)
    { -0.5f, -0.5f,  0.0f,    0,  0,  1,  1 }, // left (blue)
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
	ID3D11Texture2D* backbuffer = 0;
	_renderer->swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);	
	
	_renderer->device->CreateRenderTargetView(backbuffer, 0, &_renderer->target_view);
	backbuffer->Release();
	
	_renderer->context->OMSetRenderTargets(1, &_renderer->target_view, 0);
	
	
	
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
	
	HRESULT hr_vertex_buffer = g_renderer.device->CreateBuffer(&vertex_buffer_description, &vertex_buffer_data, &g_renderer.triangle_vertex_buffer);
	
	
	ID3DBlob *vs_blob = 0;
	ID3DBlob *ps_blob = 0;
	
	D3DCompileFromFile(L"triangle_vs.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vs_blob, 0);
	D3DCompileFromFile(L"triangle_ps.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &ps_blob, 0);

	
	g_renderer.device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), 0, &g_renderer.vertex_shader);	
	g_renderer.device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), 0, &g_renderer.pixel_shader);
	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
		
	g_renderer.device->CreateInputLayout(
										 layout, 
										 2, 
										 vs_blob->GetBufferPointer(),
										 vs_blob->GetBufferSize(),
										 &g_renderer.input_layout
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
BeginFrame(renderer *_rederer)
{
	float clear_color[4] = { 0.1f, 0.2f, 0.4f, 1.0f };
	_rederer->context->ClearRenderTargetView(_rederer->target_view, clear_color);
	
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	
	g_renderer.context->IASetVertexBuffers(0, 1, &g_renderer.triangle_vertex_buffer, &stride, &offset);
	g_renderer.context->IASetInputLayout(g_renderer.input_layout);
	g_renderer.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_renderer.context->VSSetShader(g_renderer.vertex_shader, 0, 0);
	g_renderer.context->PSSetShader(g_renderer.pixel_shader, 0, 0);
	
	g_renderer.context->Draw(3, 0);
}


internal_f void
EndFrame(renderer *_rederer)
{
	_rederer->swap_chain->Present(1, 0);
}


global_f void
RendererUpdate()
{
	BeginFrame(&g_renderer);
	
	EndFrame(&g_renderer);
}