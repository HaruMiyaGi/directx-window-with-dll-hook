#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "Math.h"

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
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		pContex->RSSetViewports(1, &vp);


		ImGui_ImplDX11_Init(pDevice.Get(), pContex.Get());
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
	void Cube(float angle, float owo)
	{
		// Data for Vertex Buffer
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


		// Vertex Buffer
		ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;
		D3D11_BUFFER_DESC vbd = { 0 };
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.CPUAccessFlags = 0;
		vbd.ByteWidth = sizeof(vertices);
		vbd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA vsd = { 0 };
		vsd.pSysMem = vertices;
		pDevice->CreateBuffer(&vbd, &vsd, pVertexBuffer.GetAddressOf());
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0;
		pContex->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);


		// Data for Index Buffer
		const unsigned short indicies[] = {
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4
		};


		// Index Buffer
		ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
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
		pContex->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);


		// Constant Buffer - vertex
		struct ConstantBuffer
		{
			//float element[4][4];
			DirectX::XMMATRIX matrix;
		};

		/*
		float one = 1.0f;
		float zer = 0.0f;
		float sin = std::sin(angle);
		float cos = std::cos(angle);
		float tan = std::tan(angle);

		const ConstantBuffer cb = {
		{
				//	 x		 y		 z		 w
					cos,	sin,	zer,	zer,
					-sin,	cos,	zer,	zer,
					zer,	zer,	one,	zer,
					zer,	zer,	zer,	one,
				}
		};
		*/

		const ConstantBuffer cb = {
			{
				DirectX::XMMatrixTranspose(
					DirectX::XMMatrixRotationY(angle) *
					DirectX::XMMatrixRotationX(-0.5f) *
					DirectX::XMMatrixTranslation(0.0f, owo, 4.0f) *
					DirectX::XMMatrixPerspectiveLH(1.0f, 1.0f, 0.5f, 10.0f)
				)
				//DirectX::XMMatrixScaling((3.0f/4.0f), 1.0f, 1.0f)
			}
		};

		ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;
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
		pContex->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());


		// Constant Buffer - pixel
		struct ConstantBuffer2
		{
			//float element[4][4];
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
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 1.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 0.0f, 1.0f },
			}
		};
		ComPtr<ID3D11Buffer> pConstantBuffer2 = nullptr;
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
		pContex->PSSetConstantBuffers(0, 1, pConstantBuffer2.GetAddressOf());


		// Pixel Shader
		ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
		ComPtr<ID3DBlob> pBlob = nullptr;
		D3DReadFileToBlob(L"PS.cso", &pBlob);
		pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
		pContex->PSSetShader(pPixelShader.Get(), nullptr, 0);


		// Vertex Shader
		ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
		//ComPtr<ID3DBlob> pBlob = nullptr;
		D3DReadFileToBlob(L"VS.cso", &pBlob);
		pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
		pContex->VSSetShader(pVertexShader.Get(), nullptr, 0);


		//	Render Target - maybe?
		//	vp? - not really


		// Topology
		pContex->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		// Input Layout
		ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
		const D3D11_INPUT_ELEMENT_DESC ied[]{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);
		pContex->IASetInputLayout(pInputLayout.Get());


		// Draw Call
		pContex->DrawIndexed((UINT)std::size(indicies), 0, 0);
	}

	void Line(Vec2 start, Vec2 end)
	{
		// Data for Vertex Buffer
		struct Vertex {
			float x;
			float y;
			float z;
		};
		const Vertex vertices[] = {
			{ view_x(start.x), view_y(start.y), 1.0f },
			{ view_x(end.x), view_y(end.y), 1.0f }
		};
		// Vertex Buffer
		ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;
		D3D11_BUFFER_DESC vbd = { 0 };
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.CPUAccessFlags = 0;
		vbd.ByteWidth = sizeof(vertices);
		vbd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA vsd = { 0 };
		vsd.pSysMem = vertices;
		pDevice->CreateBuffer(&vbd, &vsd, pVertexBuffer.GetAddressOf());
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0;
		pContex->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);


		// Constant Buffer - vertex
		struct ConstantBuffer
		{
			DirectX::XMMATRIX matrix;
		};
		const ConstantBuffer cb = {
			{
				DirectX::XMMatrixTranspose(
					DirectX::XMMatrixRotationX(0.0f)
					//DirectX::XMMatrixTranslation(.1f, .1f, .1f)
					//DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f) *
					//DirectX::XMMatrixPerspectiveLH(1.0f, 1.0f, 1.0f, 10.0f)
				)
			}
		};
		ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;
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
		pContex->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());


		// Pixel Shader
		ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
		ComPtr<ID3DBlob> pBlob = nullptr;
		D3DReadFileToBlob(L"PS.cso", &pBlob);
		pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
		pContex->PSSetShader(pPixelShader.Get(), nullptr, 0);


		// Vertex Shader
		ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
		//ComPtr<ID3DBlob> pBlob = nullptr;
		D3DReadFileToBlob(L"VS.cso", &pBlob);
		pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
		pContex->VSSetShader(pVertexShader.Get(), nullptr, 0);


		// Topology
		pContex->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


		// Input Layout
		ComPtr<ID3D11InputLayout> pInputLayout = nullptr;
		const D3D11_INPUT_ELEMENT_DESC ied[]{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout);
		pContex->IASetInputLayout(pInputLayout.Get());


		// Draw Call
		pContex->Draw(2, 0);
	}

private:
	ComPtr<ID3D11Device> pDevice = nullptr;
	ComPtr<IDXGISwapChain> pSwapChain = nullptr;
	ComPtr<ID3D11DeviceContext> pContex = nullptr;

	ComPtr<ID3D11RenderTargetView> pTarget = nullptr;
};