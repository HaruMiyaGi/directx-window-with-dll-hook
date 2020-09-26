#include <Windows.h>
#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")



ID3D11Device* pDevice = nullptr;
IDXGISwapChain* pSwapChain = nullptr;
ID3D11DeviceContext* pContex = nullptr;



bool HookD3D();

void MainThread(void* pHandle)
{
	if (HookD3D())
		MessageBoxA(nullptr, "Hooked!", "YAY!!", MB_OK);
	else
		MessageBoxA(nullptr, "FAILED!", "SADGE!!", MB_OK);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hinstDLL, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

bool HookD3D()
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
	scd.OutputWindow = GetForegroundWindow();
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, 0, nullptr, 0,
		D3D11_SDK_VERSION,
		&scd, &pSwapChain, &pDevice, nullptr, &pContex
	);

	if (FAILED(hr))
		return false;

	return true;
}