/* date = January 23rd 2026 1:35 pm */


#include "d3d11.h"

struct renderer
{
	ID3D11Device* device;
	ID3D11DeviceContext *context;
	IDXGISwapChain *swap_chain;
	ID3D11RenderTargetView *target_view;
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