#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include "Timer.h"
#include "imgui\imgui_impl_dx11.h"
#include "imgui\imgui_impl_win32.h"
#include "ImGuiManager.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

ID3D11Device* pDevice = nullptr;
IDXGISwapChain* pSwapChain = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* pTarget = nullptr;

ImguiManager imgui;


enum class IDXGISwapChainVMT {
	QueryInterface,
	AddRef,
	Release,
	SetPrivateData,
	SetPrivateDataInterface,
	GetPrivateData,
	GetParent,
	GetDevice,
	Present,
	GetBuffer,
	SetFullscreenState,
	GetFullscreenState,
	GetDesc,
	ResizeBuffers,
	ResizeTarget,
	GetContainingOutput,
	GetFrameStatistics,
	GetLastPresentCount,
};

#define BYTE_SIZE 5
using fnTemplate = HRESULT(__stdcall*)(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags);
void* ogFunction;
fnTemplate ogTrampline;

Timer timer;

void Draw(float angle)
{
	struct Vertex {
		float x, y, z;
	};

	const Vertex vertices[] = {
		{ -1.0f, -1.0f, -1.0f },
		{  1.0f, -1.0f, -1.0f },
		{ -1.0f,  1.0f, -1.0f },
		{  1.0f,  1.0f, -1.0f },
		{ -1.0f, -1.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f },
		{ -1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f }
	};
	ID3D11Buffer* pVertexBuffer = nullptr;
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0;
	vbd.ByteWidth = sizeof(vertices);
	vbd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA vsd = { 0 };
	vsd.pSysMem = vertices;
	pDevice->CreateBuffer(&vbd, &vsd, &pVertexBuffer);
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);



	const unsigned short indicies[] = {
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};
	ID3D11Buffer* pIndexBuffer = nullptr;
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.ByteWidth = sizeof(indicies);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = { 0 };
	isd.pSysMem = indicies;
	pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);
	pContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);



	struct ConstantBuffer
	{
		DirectX::XMMATRIX matrix;
	};
	const ConstantBuffer cb = {
		{
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixRotationY(angle) * 
				//DirectX::XMMatrixRotationX(-0.9f) *
				DirectX::XMMatrixTranslation(0.0f, 0.0f, 4.0f) *
				DirectX::XMMatrixPerspectiveLH(2.0f, 2.0f, 0.5f, 10.0f))
		}
	};
	ID3D11Buffer* pConstantBuffer = nullptr;
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA csd = { 0 };
	csd.pSysMem = &cb;
	pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);
	pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);



	struct ConstantBuffer2
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_color[6];
	};
	const ConstantBuffer2 cb2 = {
		{
			{ 0.0f, 0.0f, 0.0f },
			{ 0.2f, 0.0f, 0.0f },
			{ 0.4f, 0.0f, 0.0f },
			{ 0.6f, 0.0f, 0.0f },
			{ 0.8f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f }
		}
	};
	ID3D11Buffer* pConstantBuffer2 = nullptr;
	D3D11_BUFFER_DESC cbd2 = { 0 };
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DEFAULT;
	cbd2.CPUAccessFlags = 0;
	cbd2.MiscFlags = 0;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA csd2 = { 0 };
	csd2.pSysMem = &cb2;
	pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2);
	pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer2);



	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	pContext->DrawIndexed((UINT)std::size(indicies), 0, 0);
}

bool InitD3D(IDXGISwapChain* pSwapChain)
{
	HRESULT hr = pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
	if (FAILED(hr))
		return false;

	pDevice->GetImmediateContext(&pContext);
	pContext->OMGetRenderTargets(1, &pTarget, nullptr); // we could also get stencil view

	// Shaders
	ID3D11PixelShader* pPixelShader = nullptr;
	ID3DBlob* pBlob = nullptr;
	D3DReadFileToBlob(L"PS.cso", &pBlob);
	pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
	pContext->PSSetShader(pPixelShader, nullptr, 0);

	ID3D11VertexShader* pVertexShader = nullptr;
	//ComPtr<ID3DBlob> pBlob = nullptr;
	D3DReadFileToBlob(L"VS.cso", &pBlob);
	pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
	pContext->VSSetShader(pVertexShader, nullptr, 0);


	// Input Layout
	ID3D11InputLayout* pInputLayout = nullptr;
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		//{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);
	pContext->IASetInputLayout(pInputLayout);

	
	// Set Viewport
	D3D11_VIEWPORT pViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{ 0 };
	UINT numViewports = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	float fWidth = 0;
	float fHeight = 0;
	pContext->RSGetViewports(&numViewports, pViewports);
	if (!numViewports)
	{
		return false;
	}
	else
	{
		fWidth = (float)pViewports[0].Width;
		fHeight = (float)pViewports[0].Height;
	}


	return true;
}

float speed_factor = 0.5f;
char ogBytes[BYTE_SIZE];

using fnWinProc = LRESULT(__stdcall*)(HWND, UINT, WPARAM, LPARAM);
fnWinProc ogWndProc;

LRESULT __stdcall WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	return CallWindowProc(ogWndProc, hWnd, msg, wParam, lParam);
}

HRESULT __stdcall OurHook(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags)
{
	pSwapChain = pThis; // do we need swap chain?

	if (!pTarget)
	{
		if (!InitD3D(pSwapChain))
		{
			MessageBoxA(nullptr, "InitD3D() failed", "nuu :c", MB_OK);
		}
		else
		{
			ImGui_ImplWin32_Init(GetForegroundWindow());
			ImGui_ImplDX11_Init(pDevice, pContext);
		}
	}


	//auto dt = sin(timer.Peek()) / 2.0f + 0.5f;
	auto dt = timer.Peek();
	Draw(dt);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static char buffer[1024];

	if (ImGui::Begin("owo"))
	{
		ImGui::Text("uwu");
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return ogTrampline(pThis, SyncInterval, Flags);
}



bool Hook(void* pOrg, void* pDst, size_t size);
bool HookD3D();

bool WriteMem(void* pDst, char* pBytes, size_t size)
{
	DWORD dwOld;
	if (!VirtualProtect(pDst, size, PAGE_EXECUTE_READWRITE, &dwOld))
		return false;

	memcpy(pDst, pBytes, BYTE_SIZE);

	VirtualProtect(pDst, size, dwOld, &dwOld);
	return true;
}

void CleanD3D()
{
	if (pDevice) { pDevice->Release(); pDevice = nullptr; }
	if (pSwapChain) { pSwapChain->Release(); pSwapChain = nullptr; }
	if (pContext) { pContext->Release(); pContext = nullptr; }
	if (pTarget) { pTarget->Release(); pTarget = nullptr; }
}


void MainThread(HMODULE pHandle)
{
	ogWndProc = (WNDPROC)SetWindowLongPtr(GetForegroundWindow(), GWL_WNDPROC, (LONG_PTR)WndProcHook);
	//ogWndProc = (WNDPROC)SetWindowLongPtr(GetForegroundWindow(), GWLP_WNDPROC, (LONG_PTR)WndProcHook);
	

	if (!HookD3D())
		MessageBoxA(nullptr, "HookD3D() failed!", "sad", MB_OK);

	
	
	while (!GetAsyncKeyState(VK_END));
	MessageBoxA(nullptr, "CleanUp!", "uwu", MB_OK);
	CleanD3D();
	WriteMem(ogFunction, ogBytes, BYTE_SIZE);
	VirtualFree((void*)ogTrampline, 0, MEM_RELEASE);

	//GetModuleHandleEx(0, L"Hook.dll", &pHandle);
	//FreeLibraryAndExitThread(pHandle, 0);
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
		//ExitThread(0);
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
		&scd, &pSwapChain, &pDevice, nullptr, &pContext
	);

	if (FAILED(hr))
		return false;

	void** pVTM = *(void***)pSwapChain;
	ogFunction = (fnTemplate)(pVTM[(UINT)IDXGISwapChainVMT::Present]);

	if (pDevice) { pDevice->Release(); pDevice = nullptr; }
	if (pSwapChain) { pSwapChain->Release(); pSwapChain = nullptr; }
	if (pContext) { pContext->Release(); pContext = nullptr; }

	//

	void* new_mem = nullptr;
	void* pLoc = (void*)((uintptr_t)ogFunction - 0x2000);

	while (!new_mem)
	{
		new_mem = VirtualAlloc(pLoc, 10, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		pLoc = (void*)((uintptr_t)pLoc + 0x200);
	}
	
	ogTrampline = (fnTemplate)new_mem;

	//char ogBytes[BYTE_SIZE];
	// cant we just do -> memcpy(new_mem, ogFunction, BYTE_SIZE);??? instead of these 2 calls
	memcpy(ogBytes, ogFunction, BYTE_SIZE);
	memcpy(new_mem, ogBytes, BYTE_SIZE);
	new_mem = (void*)((uintptr_t)new_mem + BYTE_SIZE);

	*(char*)new_mem = (char)0xE9;
	new_mem = (void*)((uintptr_t)new_mem + 1);
	
	uintptr_t fnAddress = (uintptr_t)ogFunction + BYTE_SIZE;
	*(int*)new_mem = (int)(fnAddress - (uintptr_t)new_mem - 4);
	void* pDst = new_mem = (void*)((uintptr_t)new_mem + 4);

	*(char*)new_mem = (char)0xE9;
	new_mem = (void*)((uintptr_t)new_mem + 1);

	*(int*)new_mem = (uintptr_t)OurHook - (uintptr_t)new_mem - 4;

	return Hook(ogFunction, pDst, BYTE_SIZE); // this will update ogFunction binary
}

bool Hook(void* pOrg, void* pDst, size_t size)
{
	DWORD old;
	uintptr_t src = (uintptr_t)pOrg;
	uintptr_t dst = (uintptr_t)pDst;

	if (!VirtualProtect(pOrg, size, PAGE_EXECUTE_READWRITE, &old))
		return false;

	*(char*)src = (char)0xE9;
	*(int*)(src + 1) = (int)(dst - src - 5);

	if (size > 5)
	{
		src += 5;
		for (size_t i = 0; i < size - 5; i++)
		{
			*(char*)src = (char)0x90;
			src += 1;
		}
	}	

	VirtualProtect(pOrg, size, old, &old);

	return true;
}