//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved


//-----------------------------------------------------------------------------
// File: Cube.cpp
//
// Desktop app that renders a spinning, colorful cube.
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#include <string>
#include <memory>

#include "DeviceResources.h"
#include "Renderer.h"
#include "MainClass.h"

//-----------------------------------------------------------------------------
// Main function: Creates window, calls initialization functions, and hosts
// the render loop.
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	HRESULT hr = S_OK;

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	{
		// Begin initialization.

		// Instantiate the window manager class.
		std::shared_ptr<MainClass> winMain = std::make_shared<MainClass>();
		// Create a window.
		hr = winMain->CreateDesktopWindow();

		if (FAILED(hr))
		{
			return -1;
		}
		// Instantiate the device manager class.
		std::shared_ptr<DeviceResources> deviceResources = std::make_shared<DeviceResources>();
		// Create device resources.
		hr = deviceResources->CreateDeviceResources();
		if (FAILED(hr)) {
			OutputDebugStringW(L"Failed CreateDeviceResources");
		}

		// Instantiate the renderer.
		std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>(deviceResources);
		hr = renderer->CreateDeviceDependentResources();
		if (FAILED(hr)) {
			OutputDebugStringW(L"Failed CreateDeviceDependentResources");
		}

		// We have a window, so initialize window size-dependent resources.
		hr = deviceResources->CreateWindowResources(winMain->GetWindowHandle());
		if (FAILED(hr)) {
			OutputDebugStringW(L"Failed CreateWindowResources");
		}
		hr = renderer->CreateWindowSizeDependentResources();
		if (FAILED(hr)) {
			OutputDebugStringW(L"Failed CreateWindowSizeDependentResources");
		}

		// Go full-screen.
		//deviceResources->GoFullScreen();

		// Whoops! We resized the "window" when we went full-screen. Better
		// tell the renderer.
		hr = renderer->CreateWindowSizeDependentResources();
		if (FAILED(hr)) {
			OutputDebugStringW(L"Failed CreateWindowSizeDependentResources");
		}

		// Run the program.
		hr = winMain->Run(deviceResources, renderer);
	}

	//ComPtr<ID3D11Debug> d3dDebug;
	//if (SUCCEEDED(d3dDevice.As(&d3dDebug)))
	//{
	//	d3dDebug->ReportLiveDeviceObjects( D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL );
	//}
	// Cleanup is handled in destructors.
	return hr;
}
