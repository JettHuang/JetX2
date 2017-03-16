// \brief
//		Windows Platform OpenGL Context Created.
//

#ifndef __JETX_OPENGL_WINDOWS_H__
#define __JETX_OPENGL_WINDOWS_H__

#include "glew.h"
#include "wglew.h"
#include "Foundation/JetX.h"
#include "Renderer/RendererDefs.h"


#define	OPENGL_MAJOR_VERSION	4
#define OPENGL_MINOR_VERSION	0


class FPlatformOpenGLContext
{
public:
	HWND	WindowHandle;
	HDC		DeviceContext;
	HGLRC	OpenGLContext;

	FPlatformOpenGLContext()
		: WindowHandle(0)
		, DeviceContext(0)
		, OpenGLContext(0)
	{}
};

class FPlatformViewportContext
{
public:
	HWND	WindowHandle;
	HDC		DeviceContext;
};

// init the GL context
bool PlatformInitializeOpenGLContext(FPlatformOpenGLContext &InGLConext);
void PlatformShutdownOpenGLContext(FPlatformOpenGLContext &InGLContext);

// create & release viewport context
FPlatformViewportContext* PlatformCreateViewportContext(class FOpenGLRenderer* InRenderer, void* InWindowHandle);
void PlatformReleaseViewportContext(class FOpenGLRenderer* InRenderer, FPlatformViewportContext* InContext);

// fullscreen operations
void PlatformRestoreDesktopDisplayMode();
void PlatformResizeGLContext(class FOpenGLRenderer* InRenderer, FPlatformViewportContext* InContext, uint32_t SizeX, uint32_t SizeY, bool bFullscreen, bool bWasFullscreen);

void PlatformGetSupportedResolution(uint32_t &Width, uint32_t &Height);
bool PlatformGetAvailableResolutions(FScreenResolutionArray& Resolutions, bool bIgnoreRefreshRate);

// active viewport context
void PlatformActiveViewportContext(FPlatformOpenGLContext *InOpenGLContext, FPlatformViewportContext *InViewportCtx);

// platform swapbuffers
void PlatformSwapBuffers(FPlatformOpenGLContext *InOpenGLContext, FPlatformViewportContext *InViewportCtx);

#endif // __JETX_OPENGL_WINDOWS_H__
