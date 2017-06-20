//\brief
//		OpenGL for Mac OS.
//

#ifndef __JETX_OPENGL_MAC_H__
#define __JETX_OPENGL_MAC_H__

#include "glew.h"
#include "RendererDefs.h"


#if defined(__OBJC__)
#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>
#else
#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>
typedef void* id;
#endif


#define	OPENGL_MAJOR_VERSION	4
#define OPENGL_MINOR_VERSION	1


class FPlatformOpenGLContext
{
public:
    id  PixelFormat;
    id	OpenGLContext;
    
    FPlatformOpenGLContext()
    : PixelFormat(nil)
    , OpenGLContext(nil)
    {}
};

class FPlatformViewportContext
{
public:
    id	cocoaWindow;

    FPlatformViewportContext()
    : cocoaWindow(nil)
    {}
};

// init the GL context
bool PlatformInitializeOpenGLContext(FPlatformOpenGLContext &InGLConext);
void PlatformShutdownOpenGLContext(FPlatformOpenGLContext &InGLContext);

// create & release viewport context
FPlatformViewportContext* PlatformCreateViewportContext(FPlatformOpenGLContext &InGLContext, void* InWindowHandle);
void PlatformReleaseViewportContext(FPlatformOpenGLContext &InGLContext, FPlatformViewportContext* InContext);

// fullscreen operations
void PlatformRestoreDesktopDisplayMode();
void PlatformResizeGLContext(FPlatformOpenGLContext &InGLContext, FPlatformViewportContext* InContext, uint32_t SizeX, uint32_t SizeY, bool bFullscreen, bool bWasFullscreen);

void PlatformGetSupportedResolution(uint32_t &Width, uint32_t &Height);
bool PlatformGetAvailableResolutions(FScreenResolutionArray& Resolutions, bool bIgnoreRefreshRate);

// active viewport context
void PlatformActiveViewportContext(FPlatformOpenGLContext &InOpenGLContext, FPlatformViewportContext *InViewportCtx);

// platform swapbuffers
void PlatformSwapBuffers(FPlatformOpenGLContext &InOpenGLContext, FPlatformViewportContext *InViewportCtx);

#endif //__JETX_OPENGL_MAC_H__
