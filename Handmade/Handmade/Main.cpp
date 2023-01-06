#include <windows.h>
#include <stdint.h>
#include <string>
#include "Constants.h"

static bool Running;
static BITMAPINFO bitmapInfo;
static BITMAPINFO* pBitmapInfo = &bitmapInfo;
static void* pBitmapMemory;

static int bitmapWidth;
static int bitmapHeight;

static int bytesPerPixel = 4;

static void FillBitmapMemory(int xOffset, int yOffset) {

	uint8_t* row = (uint8_t*)pBitmapMemory;
	int pitch = bitmapWidth * bytesPerPixel;

	for (int y = 0; y < bitmapHeight; ++y) {

		//uint8_t* pixel = (uint8_t*)row;
		uint32_t* pixel = (uint32_t*)row;

		for (int x = 0; x < bitmapWidth; ++x) {

			/* Little Endian Architecture :
			0x 00 00 00 00
			0x 00 GG BB RR

			Windows devs made it so that they can visualise RGB when they looked into the Registers:
			0x 00 00 00 00
			0x 00 RR GG BB
			*/
			uint8_t blue = x + xOffset;
			uint8_t green = y + yOffset;

			*pixel = green << 8 | blue; // Filling 32 bits at a time instead of 8 bits.
			++pixel;

			/*
			Loading In Memory:		BB GG RR xx
			Loading In Register:	xx RR GG BB				Because Little Endian Architecture.
			*/

			/*
			*pixel = x + xOffset;
			++pixel;

			*pixel = y + yOffset;
			++pixel;

			*pixel = 0;
			++pixel;

			*pixel = 0;
			++pixel;*/
		}

		row += pitch;
	}	
}

// TODO: Research why for some reason we dont need to CreateCompatibleDC and CreateDIBSection.
//static HBITMAP bitmapHandle;
//static HDC bitmapDeviceContext;

// Create Bitmap buffer
static void ResizeDIBSection(int width, int height) {

	//if (bitmapHandle) {
	//	DeleteObject(bitmapHandle);
	//}
	//if (!bitmapDeviceContext) {
	//	bitmapDeviceContext = CreateCompatibleDC(0);
	//}
	

	// Make sure the memory is freed before we reallocate memory on resize.
	// TODO: Read about VirtualProtect to catch 'memory write after free' bugs.
	if (pBitmapMemory) {
		VirtualFree(pBitmapMemory, 0, MEM_RELEASE);
	}

	bitmapWidth = width;
	bitmapHeight = height;
	
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = bitmapWidth;
	bitmapInfo.bmiHeader.biHeight = -bitmapHeight; // negative so that the bitmap on screen goes top-down
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32; // Only need 24 bits: 8 bit for R, G, B. Specifying 32 for DWORD alignment.
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	void** ppBitmapMemory = &pBitmapMemory;
	/*bitmapHandle =  CreateDIBSection(bitmapDeviceContext,
									pBitmapInfo,
									DIB_RGB_COLORS,
									ppBitmapMemory,
									0,
									0);*/

	int bitmapMemorySize = bitmapWidth * bitmapHeight * bytesPerPixel;
	// What is a MemoryPage?

	pBitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

	FillBitmapMemory(0, 0);
}

static void UpdateWindow(HDC deviceContext, RECT* windowRect, int x_coordinate, int y_coordinate, int width, int height) {
	int windowWidth = windowRect->right - windowRect->left;
	int windowHeight = windowRect->bottom - windowRect->top;
	StretchDIBits(deviceContext,
		/*
						x, y, width, height, // destination
						x, y, width, height, // source
						*/
		0,0,bitmapWidth, bitmapHeight,
		0,0,windowWidth, windowHeight,
						pBitmapMemory,
						pBitmapInfo,
						DIB_RGB_COLORS,
						SRCCOPY
	);
}


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

			RECT clientRect;
			RECT* pClientRect = &clientRect;
			// Drawable rectangle.
			GetClientRect(window, pClientRect);
			long width = clientRect.right - clientRect.left;
			long height = clientRect.bottom - clientRect.top;

			// Re-render bitmap everytime the window is resized.
			ResizeDIBSection(width, height);

			break;
		}

		case WM_DESTROY: {
			OutputDebugStringA(("Event: WM_DESTROY: " + std::to_string(message) + "\n").c_str());

			DestroyWindow(window); // Destroys the window altogether.
			break;
		}

		case WM_CLOSE: {
			OutputDebugStringA(("Event: WM_CLOSE: " + std::to_string(message) + "\n").c_str());

			// Post a Quit message to the queue which really is the int we pass with PostQuitMessage.
			//PostQuitMessage(0);

			// Can display a message before toggling the flag.
			Running = false;

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
			
			int x = paintStruct.rcPaint.left;
			int y = paintStruct.rcPaint.top;
			int width = paintStruct.rcPaint.right - paintStruct.rcPaint.left;
			int height = paintStruct.rcPaint.bottom - paintStruct.rcPaint.top;

			//static DWORD rasterOperation = BLACKNESS; // What is a rater operation ?
			//rasterOperation = rasterOperation == WHITENESS ? BLACKNESS : WHITENESS;

			//PatBlt(deviceContext,
			//	paintStruct.rcPaint.left,
			//	paintStruct.rcPaint.top,
			//	width,
			//	height,
			//	rasterOperation);

			RECT clientRect;
			RECT* pClientRect = &clientRect;
			GetClientRect(window, pClientRect);

			UpdateWindow(deviceContext, pClientRect, x, y, width, height);

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
			Running = true;
			int xOffset = 0;
			int yOffset = 0;

			while (Running)
			{
				MSG message;
				MSG* pMessage = &message;

				while (PeekMessageA(pMessage, 0, 0, 0, PM_REMOVE)) {

					if (pMessage->message == WM_QUIT) {
						Running = false;
					}

					TranslateMessage(pMessage); // If this is commented out we cant resize the window or capture window Close events.
					DispatchMessageW(pMessage); // If this is commented out we still see callback procedure getting invoked.
				}

				FillBitmapMemory(xOffset, yOffset);
				xOffset++;
				yOffset++;

				PAINTSTRUCT paintStruct;
				LPPAINTSTRUCT pPaintStruct = &paintStruct;

				RECT clientRect;
				RECT* pClientRect = &clientRect;
				GetClientRect(windowHandle, pClientRect);

				//HDC deviceContext = BeginPaint(windowHandle, pPaintStruct);
				HDC deviceContext = GetDC(windowHandle);
				UpdateWindow(deviceContext,
							pClientRect,					
							clientRect.left,
							clientRect.top,
							clientRect.right - clientRect.left,
							clientRect.bottom - clientRect.top);

				ReleaseDC(windowHandle, deviceContext);

				/*
				
				// GetMessage is blocking. Use PeekMessage to get thread back to do something.
				BOOL messageResult = GetMessage(pMessage,
												0,
												0,
												0);
				if (messageResult > 0) {
					TranslateMessage(pMessage); // If this is commented out we cant resize the window or capture window Close events.
					DispatchMessageW(pMessage); // If this is commented out we still see callback procedure getting invoked.
				}
				else {
					// PostQuitMessage(0) posted a 0 message to the queue which we can handle by toggling the Running flag.
					Running = false;
				}
				*/
			}
		}
		else {

		}
	}
	else {

	}

	return(0);
}