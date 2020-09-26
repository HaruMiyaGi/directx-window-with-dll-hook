#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;

class Graphics
{
public:
	Graphics(HWND hWnd, int width, int height)
	{
		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferDesc.Width = 0;
		scd.BufferDesc.Height = 0;
		scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hWnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = 0;

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE,
			nullptr, 0, nullptr, 0,
			D3D11_SDK_VERSION,
			&scd, &pSwapChain, &pDevice, nullptr, &pContex
		);


		ComPtr<ID3D11Resource> pBackBuffer = nullptr;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget);
		pContex->OMSetRenderTargets(1, pTarget.GetAddressOf(), nullptr);


		D3D11_VIEWPORT vp = { 0 };
		vp.Width = 640;
		vp.Height = 480;
		/*vp.Width = width;
		vp.Height = height;*/
		pContex->RSSetViewports(1, &vp);
	}
	~Graphics() = default;
	void StartFrame(float r, float g, float b)
	{
		const float color[] = { r, g, b, 1.0f };
		pContex->ClearRenderTargetView(pTarget.Get(), color);
	}
	void EndFrame()
	{
		pSwapChain->Present(1, 0);
	}
private:
	ComPtr<ID3D11Device> pDevice = nullptr;
	ComPtr<IDXGISwapChain> pSwapChain = nullptr;
	ComPtr<ID3D11DeviceContext> pContex = nullptr;

	ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
};