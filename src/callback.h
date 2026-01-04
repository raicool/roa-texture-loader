#pragma once

#include <d3d11.h>

void handle_wndproc(
	const HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
);

void dx_present(
	ID3D11RenderTargetView* render_target,
	IDXGISwapChain* swapchain,
	ID3D11Device* d3d_device,
	ID3D11DeviceContext* d3d_device_context
);