#include <windows.h>
#include <string>
#include "Constants.h"

// Callback method that windows calls for our application to handle messages sent to the WNDCLASS window.
LRESULT MainWindowCallback(HWND window,
							UINT message,
							WPARAM wParam,
							LPARAM lParam)
{
	LRESULT result = 0;

	switch (message)
	{
		OutputDebugStringA(("Event:" + std::to_string(message)).c_str());

		case WM_SIZE: {
			break;
		}

		case WM_DESTROY: {
			break;
		}

		case WM_CLOSE: {
			break;
		}

		case WM_ACTIVATEAPP: {
			break;
		}

		default: {
			// Default window procedure to let windows handle the events that we dont want to handle.
			result = DefWindowProc(window, message, wParam, lParam);
			break;
		}

		return result;
	}
}

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
	// Open a simple message box.
	/*MessageBoxA(0, "This is a message box.", "Handmade Hero", MB_OK | MB_ICONINFORMATION);*/

	WNDCLASSA windowClass = {};
	windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = MainWindowCallback;

	// Can call GetModuleHandle to get the hInstance of the currently running code.
	windowClass.hInstance = instance;

	// So that when we create the window, it can be done so by passing in the classname.
	windowClass.lpszClassName = Constants::WindowClassName.c_str();

	WNDCLASSA* pWindowClass = &windowClass;
	// Register Window class.
	// Returns an ATOM type.
	// TODO: find more about ATOM type.
	if (RegisterClassA(pWindowClass)) {
		
		HWND windowHandle = CreateWindowExA(0,
											windowClass.lpszClassName,
											Constants::WindowName.c_str(),
											WS_OVERLAPPEDWINDOW|WS_VISIBLE,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											0,
											0,
											instance,
											0	// This value (if not null) is sent back to the callback function
												// as WM_CREATE message before CreateWindow returns.
											);
		if (windowHandle) {
			// Need to start a message queue that Windows pushes messages to for our window
			// which we then need to pull.
			MSG message;
			MSG* pMessage = &message;
			GetMessage(pMessage,
					   0,
					   0,
					   0);
		}
		else {

		}
	}
	else {

	}

	return(0);
}