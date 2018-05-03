//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved


//-----------------------------------------------------------------------------
// File: MainClass.cpp
//
// Creates and owns a desktop window resource.
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#include <string>
#include <memory>

#include "MainClass.h"

HINSTANCE MainClass::m_hInstance = NULL;
std::wstring MainClass::m_windowClassName = L"Direct3DWindowClass";
std::wstring MainClass::m_windowTitle = L"Direct3D Startup";

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
MainClass::MainClass()
{
}

//-----------------------------------------------------------------------------
// Create a window for our Direct3D viewport.
//-----------------------------------------------------------------------------
HRESULT MainClass::CreateDesktopWindow()
{
	// Window resources are dealt with here.

	if (m_hInstance == NULL)
		m_hInstance = (HINSTANCE)GetModuleHandle(NULL);

	HICON hIcon = NULL;
	WCHAR szExePath[MAX_PATH];
	GetModuleFileName(NULL, szExePath, MAX_PATH);

	// If the icon is NULL, then use the first one found in the exe
	if (hIcon == NULL)
		hIcon = ::ExtractIcon(m_hInstance, szExePath, 0);
	if (NULL == hIcon) {
		hIcon = (HICON)LoadImage(NULL, L"app.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_SHARED);
	}

	// Register the windows class
	WNDCLASS wndClass;
	wndClass.style = CS_DBLCLKS;
	wndClass.lpfnWndProc = MainClass::StaticWindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = hIcon;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = m_windowClassName.c_str();

	if (!RegisterClass(&wndClass))
	{
		DWORD dwError = GetLastError();
		if (dwError != ERROR_CLASS_ALREADY_EXISTS)
			return HRESULT_FROM_WIN32(dwError);
	}

	// This example uses a non-resizable 640 by 480 viewport for simplicity.
	int nDefaultWidth = 800;
	int nDefaultHeight = 600;
	int x = (GetSystemMetrics(SM_CXFULLSCREEN) - nDefaultWidth) / 2;
	int y = (GetSystemMetrics(SM_CYFULLSCREEN) - nDefaultHeight) / 2;
	m_rc;
	SetRect(&m_rc, 0, 0, nDefaultWidth, nDefaultHeight);
	AdjustWindowRect(
		&m_rc,
		WS_OVERLAPPEDWINDOW,
		false
	);

	// Create the window for our viewport.
	m_hWnd = CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE,
		m_windowClassName.c_str(),
		m_windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		x, y,
		(m_rc.right - m_rc.left), (m_rc.bottom - m_rc.top),
		0,
		NULL,
		m_hInstance,
		0
	);

	if (m_hWnd == NULL)
	{
		DWORD dwError = GetLastError();
		return HRESULT_FROM_WIN32(dwError);
	}

	return S_OK;
}

int MainClass::Run(
	std::shared_ptr<DeviceResources> deviceResources,
	std::shared_ptr<Renderer> renderer
)
{
	if (!IsWindowVisible(m_hWnd))
		ShowWindow(m_hWnd, SW_SHOW);

	// The render loop is controlled here.
	bool bGotMsg;
	MSG  msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		// Process window events.
		// Use PeekMessage() so we can use idle time to render the scene. 
		bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

		if (bGotMsg)
		{
			// Translate and dispatch the message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Update the scene.
			renderer->Update();

			// Render frames during idle time (when no messages are waiting).
			renderer->Render();

			// Present the frame to the screen.
			deviceResources->Present();
		}
	}
		GoWindowed();

	return (int)msg.wParam;
}


//-----------------------------------------------------------------------------
// Destructor.
//-----------------------------------------------------------------------------
MainClass::~MainClass()
{

}

//-----------------------------------------------------------------------------
// Process windows messages. This looks for window close events, letting us
// exit out of the sample.
//-----------------------------------------------------------------------------
LRESULT CALLBACK MainClass::StaticWindowProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		return 0;
	case WM_CLOSE:
	{
		HMENU hMenu;
		hMenu = GetMenu(hWnd);
		if (hMenu != NULL)
		{
			DestroyMenu(hMenu);
		}
		DestroyWindow(hWnd);
		UnregisterClass(
			m_windowClassName.c_str(),
			m_hInstance
		);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
