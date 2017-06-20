// \brief
//		Mac OSX Platform OpenGL Context Created.
//
#include <cassert>

#include "OpenGLMac.h"
#include <OpenGL/gl3.h>


static void MakeContextCurrentNSGL(id InGLContext)
{
	if(InGLContext)
	{
		[InGLContext makeCurrentContext];
	}
	else
	{
		[NSOpenGLContext clearCurrentContext];
	}
}

static void SwapBufferNSGL(id InGLContext)
{
	assert(InGLContext);

	// ARP appears to be unnecessary, but this is future-proof
	[InGLContext flushBuffer];
}

static void SwapIntervalNSGL(id InGLContext, int interval)
{
	assert(InGLContext);

	GLint sync = interval;
	[InGLContext setValues:&sync forParameter:NSOpenGLCPSwapInterval];
}

// Initialize OpenGL support
static bool CheckOpenGLSupport()
{
	CFBundleRef framework =
		CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));

	return framework != NULL;
}

// create a core profile OpenGL Context
static bool PlatformCreateOpenGLContextCore(FPlatformOpenGLContext &InGLContext, int MajorVersion, int MinorVersion, NSOpenGLContext* InSharedContext)
{
	unsigned int attributeCount = 0;
	// Arbitrary array size here
	NSOpenGLPixelFormatAttribute attributes[40];

#define ADD_ATTR(x) { attributes[attributeCount++] = x; }
#define ADD_ATTR2(x, y)  { ADD_ATTR(x); ADD_ATTR(y); }

	ADD_ATTR(NSOpenGLPFAAccelerated);
	ADD_ATTR(NSOpenGLPFAClosestPolicy);
	ADD_ATTR(NSOpenGLPFAAllowOfflineRenderers);
	ADD_ATTR(kCGLPFASupportsAutomaticGraphicsSwitching);
	
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 101000
	if(MajorVersion >= 4)
	{
		ADD_ATTR2(NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core);
	}
	else
#endif
	if(MajorVersion >= 3)
	{
		ADD_ATTR2(NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core);		
	}
	
	ADD_ATTR2(NSOpenGLPFAColorSize, 24);
	ADD_ATTR2(NSOpenGLPFAAlphaSize, 8);
	ADD_ATTR2(NSOpenGLPFADepthSize, 24);
	ADD_ATTR2(NSOpenGLPFAStencilSize, 8);

	// double buffer
	ADD_ATTR(NSOpenGLPFADoubleBuffer);
	ADD_ATTR2(NSOpenGLPFASampleBuffers, 0);

	// end.
    ADD_ATTR(0);

#undef ADD_ATTR
#undef ADD_ATTR2
    InGLContext.PixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
    if(InGLContext.PixelFormat == nil)
    {
        return false;
    }
	
    InGLContext.OpenGLContext = [[NSOpenGLContext alloc] initWithFormat:InGLContext.PixelFormat shareContext:InSharedContext];
    if(InGLContext.OpenGLContext == nil)
    {
        [InGLContext.PixelFormat release];
        InGLContext.PixelFormat = nil;
        return false;
    }
    
    return true;
}

bool PlatformInitializeOpenGLContext(FPlatformOpenGLContext &InGLConext)
{
    bool bOpenGLSupported = CheckOpenGLSupport();
    if(!bOpenGLSupported)
    {
        return false;
    }
    
    // create core profile
    bOpenGLSupported = PlatformCreateOpenGLContextCore(InGLConext, OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION, NULL);
    if(!bOpenGLSupported)
    {
        return false;
    }
    
    MakeContextCurrentNSGL(InGLConext.OpenGLContext);
    SwapIntervalNSGL(InGLConext.OpenGLContext, 0); // turn off v-sync
    
    /* initialize GLEW */
    glewExperimental = GL_TRUE;
    GLenum Result = glewInit();
    assert(GLEW_OK == Result);
    
    return bOpenGLSupported;
}

void PlatformShutdownOpenGLContext(FPlatformOpenGLContext &InGLContext)
{
    MakeContextCurrentNSGL(nil);
    if(InGLContext.OpenGLContext != nil)
    {
        [InGLContext.OpenGLContext release];
        InGLContext.OpenGLContext = nil;
    }
    if(InGLContext.PixelFormat != nil)
    {
        [InGLContext.PixelFormat release];
        InGLContext.PixelFormat = nil;
    }
}

// create & release viewport context
FPlatformViewportContext* PlatformCreateViewportContext(FPlatformOpenGLContext &InGLContext, void* InWindowHandle)
{
    FPlatformViewportContext *Context = new FPlatformViewportContext();
    Context->cocoaWindow = (id)InWindowHandle;
    assert(Context->cocoaWindow);

    return Context;
}

void PlatformReleaseViewportContext(FPlatformOpenGLContext &InGLContext, FPlatformViewportContext* InViewportCtx)
{
    assert(InViewportCtx);

    id cocoaView = [(NSWindow*)InViewportCtx->cocoaWindow contentView];
    bool bActived = [InGLContext.OpenGLContext view] == cocoaView;
    if(bActived)
    {
        [InGLContext.OpenGLContext setView:nil];
    }

    delete InViewportCtx;
}


// fullscreen operation
void PlatformRestoreDesktopDisplayMode()
{
    // todo:
}

/**
 * Resize the GL context.
 */
void PlatformResizeGLContext(FPlatformOpenGLContext &InGLContext, FPlatformViewportContext* InContext, uint32_t SizeX, uint32_t SizeY, bool bFullscreen, bool bWasFullscreen)
{
}

void PlatformGetSupportedResolution(uint32_t &Width, uint32_t &Height)
{
    // todo:
}

bool PlatformGetAvailableResolutions(FScreenResolutionArray& Resolutions, bool bIgnoreRefreshRate)
{
    // todo:
    return true;
}


// active viewport context
void PlatformActiveViewportContext(FPlatformOpenGLContext &InGLContext, FPlatformViewportContext *InViewportCtx)
{
    assert(InViewportCtx);
    
    id cocoaView = [(NSWindow*)InViewportCtx->cocoaWindow contentView];
    bool bActived = [InGLContext.OpenGLContext view] == cocoaView;
    if(!bActived)
    {
        [InGLContext.OpenGLContext setView:cocoaView];
    }
}

void PlatformSwapBuffers(FPlatformOpenGLContext &InOpenGLContext, FPlatformViewportContext *InViewportCtx)
{
    assert(InViewportCtx);
    SwapBufferNSGL(InOpenGLContext.OpenGLContext);
}


