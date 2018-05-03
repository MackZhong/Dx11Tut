//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved


//-----------------------------------------------------------------------------
// File: Cube.cpp
//
// Renders a spinning, colorful cube.
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#include <string>
#include <memory>
#include <ppltasks.h>

#include "Renderer.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
Renderer::Renderer(std::shared_ptr<DeviceResources> deviceResources)
	:
	m_frameCount(0),
	m_deviceResources(deviceResources)
{
	m_frameCount = 0; // init frame count
}

//-----------------------------------------------------------------------------
// Create Direct3D shader resources by loading the .cso files.
//-----------------------------------------------------------------------------
HRESULT Renderer::CreateShaders()
{
	HRESULT hr = S_OK;

	// Use the Direct3D device to load resources into graphics memory.
	Microsoft::WRL::ComPtr<ID3D11Device> device = m_deviceResources->GetDevice();

	// You'll need to use a file loader to load the shader bytecode. In this
	// example, we just use the standard library.
	int fh;
	if (_sopen_s(&fh, "CubeVertexShader.cso", _O_RDWR | _O_CREAT, _SH_DENYNO,
		_S_IREAD | _S_IWRITE) != 0) {
		return E_FAIL;
	}
	unsigned int destSize = _filelength(fh);
	auto bytes = std::make_unique<BYTE[]>(destSize);

	size_t bytesRead = _read(fh, bytes.get(), destSize);
	_close(fh);

	hr = device->CreateVertexShader(
		bytes.get(),
		bytesRead,
		nullptr,
		&m_pVertexShader
	);
	if (FAILED(hr)) return hr;
	SetName<ID3D11VertexShader>(m_pVertexShader.Get(), "ID3D11VertexShader* m_pVertexShader");

	D3D11_INPUT_ELEMENT_DESC iaDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
		iaDesc,
		ARRAYSIZE(iaDesc),
		bytes.get(),
		bytesRead,
		&m_pInputLayout
	);
	if (FAILED(hr)) return hr;
	SetName<ID3D11InputLayout>(m_pInputLayout.Get(), "ID3D11VertexShader* m_pInputLayout");

	if (_sopen_s(&fh, "CubePixelShader.cso", _O_RDWR | _O_CREAT, _SH_DENYNO,
		_S_IREAD | _S_IWRITE) != 0) {
		return E_FAIL;
	}
	destSize = _filelength(fh);
	bytes = std::make_unique<BYTE[]>(destSize);

	bytesRead = _read(fh, bytes.get(), destSize);
	_close(fh);

	hr = device->CreatePixelShader(
		bytes.get(),
		bytesRead,
		nullptr,
		m_pPixelShader.GetAddressOf()
	);
	if (FAILED(hr)) return hr;
	SetName<ID3D11PixelShader>(m_pPixelShader.Get(), "ID3D11PixelShader* m_pPixelShader");

	CD3D11_BUFFER_DESC cbDesc(
		sizeof(ConstantBufferStruct),
		D3D11_BIND_CONSTANT_BUFFER
	);

	hr = device->CreateBuffer(
		&cbDesc,
		nullptr,
		m_pConstantBuffer.GetAddressOf()
	);
	if (FAILED(hr)) return hr;
	SetName<ID3D11Buffer>(m_pConstantBuffer.Get(), "ID3D11Buffer* m_pConstantBuffer");


	// Load the extended shaders with lighting calculations.
	/*
	bytes = new BYTE[destSize];
	bytesRead = 0;
	fopen_s(&vShader, "CubeVertexShaderLighting.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, 4096, vShader);
	hr = device->CreateVertexShader(
		bytes,
		bytesRead,
		nullptr,
		&m_pVertexShader
		);

	D3D11_INPUT_ELEMENT_DESC iaDescExtended[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = device->CreateInputLayout(
		iaDesc,
		ARRAYSIZE(iaDesc),
		bytes,
		bytesRead,
		&m_pInputLayoutExtended
		);

	delete bytes;


	bytes = new BYTE[destSize];
	bytesRead = 0;
	fopen_s(&pShader, "CubePixelShaderPhongLighting.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, 4096, pShader);
	hr = device->CreatePixelShader(
		bytes,
		bytesRead,
		nullptr,
		m_pPixelShader.GetAddressOf()
		);

	delete bytes;

	fclose(vShader);
	fclose(pShader);


	bytes = new BYTE[destSize];
	bytesRead = 0;
	fopen_s(&pShader, "CubePixelShaderTexelLighting.cso", "rb");
	bytesRead = fread_s(bytes, destSize, 1, 4096, pShader);
	hr = device->CreatePixelShader(
	bytes,
	bytesRead,
	nullptr,
	m_pPixelShader.GetAddressOf()
	);

	delete bytes;

	fclose(pShader);
	*/

	return hr;
}

//-----------------------------------------------------------------------------
// Create the cube:
// Creates the vertex buffer and index buffer.
//-----------------------------------------------------------------------------
HRESULT Renderer::CreateCube()
{
	HRESULT hr = S_OK;

	// Use the Direct3D device to load resources into graphics memory.
	Microsoft::WRL::ComPtr<ID3D11Device> device = m_deviceResources->GetDevice();

	// Create cube geometry.
	VertexPositionColor CubeVertices[] =
	{
		{DirectX::XMFLOAT3(-0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(0,   0,   0),},
		{DirectX::XMFLOAT3(-0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(0,   0,   1),},
		{DirectX::XMFLOAT3(-0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(0,   1,   0),},
		{DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0,   1,   1),},

		{DirectX::XMFLOAT3(0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(1,   0,   0),},
		{DirectX::XMFLOAT3(0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(1,   0,   1),},
		{DirectX::XMFLOAT3(0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(1,   1,   0),},
		{DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(1,   1,   1),},
	};

	// Create vertex buffer:

	CD3D11_BUFFER_DESC vDesc(
		sizeof(CubeVertices),
		D3D11_BIND_VERTEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA vData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	vData.pSysMem = CubeVertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&vDesc,
		&vData,
		&m_pVertexBuffer
	);
	if (FAILED(hr)) return hr;
	SetName<ID3D11Buffer>(m_pVertexBuffer.Get(), "ID3D11Buffer* m_pVertexBuffer");

	// Create index buffer:
	unsigned short CubeIndices[] =
	{
		0,2,1, // -x
		1,2,3,

		4,5,6, // +x
		5,7,6,

		0,1,5, // -y
		0,5,4,

		2,6,7, // +y
		2,7,3,

		0,4,6, // -z
		0,6,2,

		1,3,7, // +z
		1,7,5,
	};

	m_indexCount = ARRAYSIZE(CubeIndices);

	CD3D11_BUFFER_DESC iDesc(
		sizeof(CubeIndices),
		D3D11_BIND_INDEX_BUFFER
	);

	D3D11_SUBRESOURCE_DATA iData;
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	iData.pSysMem = CubeIndices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(
		&iDesc,
		&iData,
		&m_pIndexBuffer
	);
	if (FAILED(hr)) return hr;
	SetName<ID3D11Buffer>(m_pIndexBuffer.Get(), "ID3D11Buffer* m_pIndexBuffer");

	return hr;
}

//-----------------------------------------------------------------------------
// Create the view matrix and create the perspective matrix.
//-----------------------------------------------------------------------------
HRESULT Renderer::CreateViewAndPerspective()
{
	// Use DirectXMath to create view and perspective matrices.

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.7f, 1.5f, 0.f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, -0.1f, 0.0f, 0.f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);

	DirectX::XMStoreFloat4x4(
		&m_constantBufferData.view,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixLookAtRH(
				eye,
				at,
				up
			)
		)
	);

	float aspectRatio = m_deviceResources->GetAspectRatio();

	DirectX::XMStoreFloat4x4(
		&m_constantBufferData.projection,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixPerspectiveFovRH(
				DirectX::XMConvertToRadians(70),
				aspectRatio,
				0.01f,
				100.0f
			)
		)
	);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Create device-dependent resources for rendering.
//-----------------------------------------------------------------------------
HRESULT Renderer::CreateDeviceDependentResources()
{
	// Compile shaders using the Effects library.
	auto CreateShadersTask = Concurrency::create_task(
		[this]()
	{
		CreateShaders();
	}
	);

	// Load the geometry for the spinning cube.
	auto CreateCubeTask = CreateShadersTask.then(
		[this]()
	{
		CreateCube();
	}
	);
	return S_OK;
}

HRESULT Renderer::CreateWindowSizeDependentResources()
{
	// Create the view matrix and the perspective matrix.
	return CreateViewAndPerspective();
}


//-----------------------------------------------------------------------------
// Update the scene.
//-----------------------------------------------------------------------------
void Renderer::Update()
{
	// Rotate the cube 1 degree per frame.
	DirectX::XMStoreFloat4x4(
		&m_constantBufferData.world,
		DirectX::XMMatrixTranspose(
			DirectX::XMMatrixRotationY(
				DirectX::XMConvertToRadians(
				(float)m_frameCount++
				)
			)
		)
	);

	if (m_frameCount == MAXUINT)  m_frameCount = 0;
}

//-----------------------------------------------------------------------------
// Render the cube.
//-----------------------------------------------------------------------------
void Renderer::Render()
{
	// Use the Direct3D device context to draw.
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = m_deviceResources->GetDeviceContext();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTarget = m_deviceResources->GetRenderTarget();
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencil = m_deviceResources->GetDepthStencil();

	context->UpdateSubresource(
		m_pConstantBuffer.Get(),
		0,
		nullptr,
		&m_constantBufferData,
		0,
		0
	);

	// Clear the render target and the z-buffer.
	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };
	context->ClearRenderTargetView(
		renderTarget.Get(),
		teal
	);
	context->ClearDepthStencilView(
		depthStencil.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Set the render target.
	context->OMSetRenderTargets(
		1,
		renderTarget.GetAddressOf(),
		depthStencil.Get()
	);

	// Set up the IA stage by setting the input topology and layout.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;

	context->IASetVertexBuffers(
		0,
		1,
		m_pVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_pIndexBuffer.Get(),
		DXGI_FORMAT_R16_UINT,
		0
	);

	context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	context->IASetInputLayout(m_pInputLayout.Get());

	// Set up the vertex shader stage.
	context->VSSetShader(
		m_pVertexShader.Get(),
		nullptr,
		0
	);

	context->VSSetConstantBuffers(
		0,
		1,
		m_pConstantBuffer.GetAddressOf()
	);

	// Set up the pixel shader stage.
	context->PSSetShader(
		m_pPixelShader.Get(),
		nullptr,
		0
	);

	// Calling Draw tells Direct3D to start sending commands to the graphics device.
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}

//-----------------------------------------------------------------------------
// Clean up cube resources when the Direct3D device is lost or destroyed.
//-----------------------------------------------------------------------------
Renderer::~Renderer()
{
	// ComPtr will clean up references for us. But be careful to release
	// references to anything you don't need whenever you call Flush or Trim.
	// As always, clean up your system (CPU) memory resources before exit.
}
