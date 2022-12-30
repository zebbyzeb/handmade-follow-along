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
		case WM_SIZE: {
			OutputDebugStringA(("Event: WM_SIZE: " + std::to_string(message) + "\n").c_str());
			break;
		}

		case WM_DESTROY: {
			OutputDebugStringA(("Event: WM_DESTROY: " + std::to_string(message) + "\n").c_str());
			break;
		}

		case WM_CLOSE: {
			OutputDebugStringA(("Event: WM_CLOSE: " + std::to_string(message) + "\n").c_str());
			break;
		}

		case WM_ACTIVATEAPP: {
			OutputDebugStringA(("Event: WM_ACTIVATEAPP: " + std::to_string(message) + "\n").c_str());
			break;
		}

		case WM_PAINT: {
			OutputDebugStringA(("Event: WM_PAINT: " + std::to_string(message) + "\n").c_str());

			PAINTSTRUCT paintStruct;
			LPPAINTSTRUCT pPaintStruct = &paintStruct;

			HDC deviceContext = BeginPaint(window, pPaintStruct);
			
			int width = paintStruct.rcPaint.right - paintStruct.rcPaint.left;
			int height = paintStruct.rcPaint.bottom - paintStruct.rcPaint.top;

			static DWORD rasterOperation = BLACKNESS;
			rasterOperation = rasterOperation == WHITENESS ? BLACKNESS : WHITENESS;

			PatBlt(deviceContext,
				paintStruct.rcPaint.left,
				paintStruct.rcPaint.top,
				width,
				height,
				rasterOperation);

			EndPaint(window, pPaintStruct);

			break;
		}

		default: {
			OutputDebugStringA(("Event: " + std::to_string(message) + "\n").c_str());

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
			bool Running = true;
			while (Running)
			{
				MSG message;
				MSG* pMessage = &message;
				BOOL messageResult = GetMessage(pMessage,
												0,
												0,
												0);
				if (messageResult > 0) {
					TranslateMessage(pMessage);
					DispatchMessageW(pMessage);
				}
			}
		}
		else {

		}
	}
	else {

	}

	return(0);
}