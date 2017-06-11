// \brief
//		Windows Platform OpenGL Context Created.
//

#include <cstdlib>
#include <cassert>
#include "OpenGLWindows.h"


/**
* A dummy wndproc.
*/
static LRESULT CALLBACK PlatformDummyGLWndproc(HWND hWnd, uint32_t Message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, Message, wParam, lParam);
}

/**
* Initialize a pixel format descriptor for the given window handle.
*/
static void PlatformInitPixelFormatForDevice(HDC DeviceContext)
{
	// Pixel format descriptor for the context.
	PIXELFORMATDESCRIPTOR PixelFormatDesc;
	memset(&PixelFormatDesc, 0, sizeof(PixelFormatDesc));
	PixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	PixelFormatDesc.nVersion = 1;
	PixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	PixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
	PixelFormatDesc.cColorBits = 32;
	PixelFormatDesc.cDepthBits = 24;
	PixelFormatDesc.cStencilBits = 8;
	PixelFormatDesc.iLayerType = PFD_MAIN_PLANE;


	// Set the pixel format and create the context.
	int32_t PixelFormat = ::ChoosePixelFormat(DeviceContext, &PixelFormatDesc);
	if (!PixelFormat || !::SetPixelFormat(DeviceContext, PixelFormat, &PixelFormatDesc))
	{
		assert(0);
	}
}

static void PlatformCreateDummyGLWindow(FPlatformOpenGLContext &InGLContext)
{
	const TCHAR* WindowsClassName = TEXT("DummyGLHelperWindow");

	static bool bInitializedWindowClass = false;
	if (!bInitializedWindowClass)
	{
		WNDCLASS wc;

		bInitializedWindowClass = true;
		memset(&wc, 0, sizeof(wc));
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = PlatformDummyGLWndproc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = NULL;
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = (HBRUSH)(COLOR_MENUTEXT);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = WindowsClassName;
		ATOM ClassAtom = ::RegisterClass(&wc);
		assert(ClassAtom);
	}

	InGLContext.WindowHandle = ::CreateWindowEx(WS_EX_WINDOWEDGE,
		WindowsClassName,
		NULL,
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, NULL, NULL);
	assert(InGLContext.WindowHandle);

	// get device context
	InGLContext.DeviceContext = ::GetDC(InGLContext.WindowHandle);
	assert(InGLContext.DeviceContext);

	PlatformInitPixelFormatForDevice(InGLContext.DeviceContext);
}

// create a core profile OpenGL Context
static void PlatformCreateOpenGLContextCore(FPlatformOpenGLContext &InGLContext, int32_t MajorVersion, int32_t MinorVersion, HGLRC InParentContext)
{
	assert(wglCreateContextAttribsARB);
	assert(InGLContext.DeviceContext);

	int32_t DebugFlag = 0;
	if (0)
	{
		DebugFlag = WGL_CONTEXT_DEBUG_BIT_ARB;
	}

	int AttribList[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, MajorVersion,
		WGL_CONTEXT_MINOR_VERSION_ARB, MinorVersion,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | DebugFlag,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	InGLContext.OpenGLContext = wglCreateContextAttribsARB(InGLContext.DeviceContext, InParentContext, AttribList);
}

bool PlatformInitializeOpenGLContext(FPlatformOpenGLContext &InGLConext)
{
	bool bOpenGLSupported = false;

	PlatformCreateDummyGLWindow(InGLConext);
	
	InGLConext.OpenGLContext = wglCreateContext(InGLConext.DeviceContext);
	assert(InGLConext.OpenGLContext);
	wglMakeCurrent(InGLConext.DeviceContext, InGLConext.OpenGLContext);

	/* initialize GLEW */
	glewExperimental = GL_TRUE;
	GLenum Result = glewInit();
	assert(GLEW_OK == Result);

	if (wglCreateContextAttribsARB)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(InGLConext.OpenGLContext);

		// create core profile
		PlatformCreateOpenGLContextCore(InGLConext, OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION, NULL);
		if (InGLConext.OpenGLContext)
		{
			bOpenGLSupported = true;
			wglMakeCurrent(InGLConext.DeviceContext, InGLConext.OpenGLContext);
			wglSwapIntervalEXT(0); // turn off v-sync
		}
	}

	return bOpenGLSupported;
}

void PlatformShutdownOpenGLContext(FPlatformOpenGLContext &InGLContext)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(InGLContext.OpenGLContext);
	::ReleaseDC(InGLContext.WindowHandle, InGLContext.DeviceContext);
	::DestroyWindow(InGLContext.WindowHandle);
}

// create & release viewport context
FPlatformViewportContext* PlatformCreateViewportContext(class FOpenGLRenderer* InRenderer, void* InWindowHandle)
{
	assert(InRenderer);

	FPlatformViewportContext *Context = new FPlatformViewportContext();
	Context->WindowHandle = (HWND)InWindowHandle;
	Context->DeviceContext = ::GetDC(Context->WindowHandle);
	assert(Context->DeviceContext);

	PlatformInitPixelFormatForDevice(Context->DeviceContext);

	return Context;
}

void PlatformReleaseViewportContext(class FOpenGLRenderer* InRenderer, FPlatformViewportContext* InContext)
{
	assert(InRenderer && InContext);

	HDC CurrentDC = wglGetCurrentDC();
	bool bActived = (CurrentDC == InContext->DeviceContext);

	if (bActived)
	{
		wglMakeCurrent(NULL, NULL);
	}

	::ReleaseDC(InContext->WindowHandle, InContext->DeviceContext);
	InContext->WindowHandle = NULL;
	delete InContext;
}


// fullscreen operation
void PlatformRestoreDesktopDisplayMode()
{
	ChangeDisplaySettings(NULL, 0);
}

/**
* Resize the GL context.
*/
void PlatformResizeGLContext(class FOpenGLRenderer* InRenderer, FPlatformViewportContext* InContext, uint32_t SizeX, uint32_t SizeY, bool bFullscreen, bool bWasFullscreen)
{
}

void PlatformGetSupportedResolution(uint32_t &Width, uint32_t &Height)
{
	uint32_t InitializedMode = false;
	uint32_t BestWidth = 0;
	uint32_t BestHeight = 0;
	uint32_t ModeIndex = 0;
	DEVMODE DisplayMode;
	memset(&DisplayMode, 0, sizeof(DEVMODE));

	while (EnumDisplaySettings(NULL, ModeIndex++, &DisplayMode))
	{
		bool IsEqualOrBetterWidth = abs((int32_t)DisplayMode.dmPelsWidth - (int32_t)Width) <= abs((int32_t)BestWidth - (int32_t)Width);
		bool IsEqualOrBetterHeight = abs((int32_t)DisplayMode.dmPelsHeight - (int32_t)Height) <= abs((int32_t)BestHeight - (int32_t)Height);
		if (!InitializedMode || (IsEqualOrBetterWidth && IsEqualOrBetterHeight))
		{
			BestWidth = DisplayMode.dmPelsWidth;
			BestHeight = DisplayMode.dmPelsHeight;
			InitializedMode = true;
		}
	}
	assert(InitializedMode);
	Width = BestWidth;
	Height = BestHeight;
}

bool PlatformGetAvailableResolutions(FScreenResolutionArray& Resolutions, bool bIgnoreRefreshRate)
{
	int32_t MinAllowableResolutionX = 0;
	int32_t MinAllowableResolutionY = 0;
	int32_t MaxAllowableResolutionX = 10480;
	int32_t MaxAllowableResolutionY = 10480;
	int32_t MinAllowableRefreshRate = 0;
	int32_t MaxAllowableRefreshRate = 10480;

	if (MaxAllowableResolutionX == 0)
	{
		MaxAllowableResolutionX = 10480;
	}
	if (MaxAllowableResolutionY == 0)
	{
		MaxAllowableResolutionY = 10480;
	}
	if (MaxAllowableRefreshRate == 0)
	{
		MaxAllowableRefreshRate = 10480;
	}

	uint32_t ModeIndex = 0;
	DEVMODE DisplayMode;
	memset(&DisplayMode, 0, sizeof(DEVMODE));

	while (EnumDisplaySettings(NULL, ModeIndex++, &DisplayMode))
	{
		if (((int32_t)DisplayMode.dmPelsWidth >= MinAllowableResolutionX) &&
			((int32_t)DisplayMode.dmPelsWidth <= MaxAllowableResolutionX) &&
			((int32_t)DisplayMode.dmPelsHeight >= MinAllowableResolutionY) &&
			((int32_t)DisplayMode.dmPelsHeight <= MaxAllowableResolutionY)
			)
		{
			bool bAddIt = true;
			if (bIgnoreRefreshRate == false)
			{
				if (((int32_t)DisplayMode.dmDisplayFrequency < MinAllowableRefreshRate) ||
					((int32_t)DisplayMode.dmDisplayFrequency > MaxAllowableRefreshRate)
					)
				{
					continue;
				}
			}
			else
			{
				// See if it is in the list already
				for (uint32_t CheckIndex = 0; CheckIndex < Resolutions.size(); CheckIndex++)
				{
					FScreenResolution& CheckResolution = Resolutions[CheckIndex];
					if ((CheckResolution.Width == DisplayMode.dmPelsWidth) &&
						(CheckResolution.Height == DisplayMode.dmPelsHeight))
					{
						// Already in the list...
						bAddIt = false;
						break;
					}
				}
			}

			if (bAddIt)
			{
				// Add the mode to the list
				FScreenResolution ScreenResolution;

				ScreenResolution.Width = DisplayMode.dmPelsWidth;
				ScreenResolution.Height = DisplayMode.dmPelsHeight;
				ScreenResolution.RefreshRate = DisplayMode.dmDisplayFrequency;

				Resolutions.push_back(ScreenResolution);
			}
		}
	}

	return true;
}


// active viewport context
void PlatformActiveViewportContext(FPlatformOpenGLContext *InOpenGLContext, FPlatformViewportContext *InViewportCtx)
{
	assert(InOpenGLContext);
	assert(InViewportCtx);
	
	wglMakeCurrent(InViewportCtx->DeviceContext, InOpenGLContext->OpenGLContext);
}

void PlatformSwapBuffers(FPlatformOpenGLContext *InOpenGLContext, FPlatformViewportContext *InViewportCtx)
{
	assert(InViewportCtx);
	::SwapBuffers(InViewportCtx->DeviceContext);
}
