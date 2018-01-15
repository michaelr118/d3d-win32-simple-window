#include "Window.h"
#define MESSAGE_HANDLED 0

// Because why not?
#define MEMBER static

// Handle to our awesome window
MEMBER HWND m_Window = NULL;
// This can also be passed from WinMain, but we'll save it here for convenience
MEMBER const HINSTANCE m_Application = GetModuleHandle(NULL);

// While true, the mouse will not be able to leave our window (good for 3D)
MEMBER bool m_ClipMouse = true;

// This will be called by the OS/WinMain whenever we have a message waiting 
MEMBER LRESULT CALLBACK OnMessage(HWND Wnd, UINT Message, WPARAM W, LPARAM L);

namespace Window
{
	/* init a new Win32 window with 'Name' in titlebar, 'W' canvas width, and 'H' canvas height.
	Must be called prior to calling other procedures */
	void init(const wchar_t *Name, int W, int H)
	{
		/* Define our new window class. CS_HREDRAW and CS_VREDRAW are used here for 3D rendering.
		'D3DWindow' will be used later to create the window */
		const WNDCLASS wc = { (CS_HREDRAW | CS_VREDRAW), OnMessage,
			0, 0, m_Application, LoadIcon(NULL, IDI_APPLICATION),
			LoadCursor(NULL, IDC_ARROW), NULL, NULL, L"D3DWindow" };

		// Window border style. In this case: an icon (sysmenu), a caption for the window name, and a minimize box.
		const DWORD ws = (WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX);

		/* W and H passed include window border. We need to adjust these so that W and H are describing the drawable area.
		We also do some math here to center the window on the screen */
		RECT wr = { 0, 0, W, H };
		AdjustWindowRect(&wr, ws, FALSE);

		const int w = (wr.right - wr.left);
		const int h = (wr.bottom - wr.top);
		const int x = ((GetSystemMetrics(SM_CXSCREEN) - w) / 2);
		const int y = ((GetSystemMetrics(SM_CYSCREEN) - h) / 2);

		// Finally, we register our new window class with the OS and create the window, m_Window
		RegisterClass(&wc);
		m_Window = CreateWindow(L"D3DWindow", Name, ws, x, y, w, h,
			NULL, NULL, m_Application, NULL);

		// This bit makes our window accept raw input from the mouse
		const RAWINPUTDEVICE Mouse = { 0x01, 0x02, NULL, m_Window };
		RegisterRawInputDevices(&Mouse, 1, sizeof(RAWINPUTDEVICE));
		ShowCursor(FALSE);
	}

	// For when you need the window but you don't really /need/ the window
	HWND GetHandle()
	{
		return m_Window;
	}

	// By default, the window will be hidden to allow for other setup/loading.
	void Show()
	{
		ShowWindow(m_Window, SW_SHOW);
	}

	// Call this before cleaning everything up so the user thinks our program is fast
	void Hide()
	{
		ShowWindow(m_Window, SW_HIDE);
	}

	// When you're done with the window, free up its class/memory for other uses
	void End()
	{
		DestroyWindow(m_Window);
		UnregisterClass(L"D3DWindow", m_Application);
	}
}

/* Should return 'MESSAGE_HANDLED' if we process the message so that the OS doesn't process it.
Otherwise, we let the OS decide what's best */
MEMBER LRESULT CALLBACK OnMessage(HWND Wnd, UINT Message, WPARAM W, LPARAM L)
{
	switch (Message)
	{
	// Called when the user presses the 'X' button. We can totally ignore this, but that wouldn't be very nice
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return MESSAGE_HANDLED;
	}
	// Called when Raw Input has data for us. Good for real-time keyboard and mouse input
	case WM_INPUT:
	{
		RAWINPUT Input;
		UINT s = sizeof(RAWINPUT);
		GetRawInputData((HRAWINPUT)L, RID_INPUT, &Input, &s,
			sizeof(RAWINPUTHEADER));

		if (Input.header.dwType == RIM_TYPEMOUSE && m_ClipMouse)
		{
			/* Pass Input.data.mouse.lLastX and Input.data.mouse.lLastY to your input manager of choice. */
			/* GSM::OnMouseMove(Input.data.mouse.lLastX,
				Input.data.mouse.lLastY); */
		}
		return MESSAGE_HANDLED;
	}
	// Called when a keyboard key is pressed
	case WM_KEYDOWN:
	{
		// Esc key will free the mouse, otherwise we process the key
		if (W == VK_ESCAPE && m_ClipMouse)
		{
			ShowCursor(TRUE);
			ClipCursor(nullptr);
			m_ClipMouse = false;
		}
		else if (m_ClipMouse)
		{
			/* Pass W to your input manager of choice. */
			/* GSM::OnKeyLimited(W); */
		}
		return MESSAGE_HANDLED;
	}
	/* Called when the window loses focus.
	If something causes the window to lose focus, we don't want to clip the mouse anymore. */
	case WM_KILLFOCUS:
	{
		if (m_ClipMouse)
		{
			ShowCursor(TRUE);
			ClipCursor(nullptr);
			m_ClipMouse = false;
		}
		return MESSAGE_HANDLED;
	}
	// Called when the L mouse button is clicked
	case WM_LBUTTONDOWN:
	{
		if (!m_ClipMouse)
		{
			ShowCursor(FALSE);
			m_ClipMouse = true;
		}
		else
		{
			/* Pass MOUSE_LBUTTON to your input manager of choice. */
			/* GSM::OnMouseButton(MOUSE_LBUTTON); */
		}
		return MESSAGE_HANDLED;
	}
	// Called when the R mouse button is clicked
	case WM_RBUTTONDOWN:
	{
		if (m_ClipMouse)
		{
			/* Pass MOUSE_RBUTTON to your input manager of choice. */
			/* GSM::OnMouseButton(MOUSE_RBUTTON); */
		}
		return MESSAGE_HANDLED;
	}
	}
	// We didn't have a handler, so we pass the message back to the OS
	return DefWindowProc(Wnd, Message, W, L);
}