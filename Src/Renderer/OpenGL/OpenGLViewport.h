// \brief
//		OpenGL Viewport
//

#ifndef __JETX_OPENGL_VIEWPORT_H__
#define __JETX_OPENGL_VIEWPORT_H__

#include "Foundation/JetX.h"
#include "Foundation/RefCounting.h"
#include "Renderer/RendererDefs.h"
#include "Renderer/RendererState.h"
#include "Renderer/RHIResource.h"
#include "PlatformOpenGL.h"


// OpenGL Viewport
class FRHIOpenGLViewport : public FRHIViewport
{
public:
	FRHIOpenGLViewport(class FOpenGLRenderer *InRenderer, void *InWindowHandle, int32_t InSizeX, int32_t InSizeY, bool InbIsFullscreen);
	virtual ~FRHIOpenGLViewport();

	void Resize(uint32_t InSizeX, uint32_t InSizeY, bool InbIsFullscreen);
	FIntPoint GetSize() const { return FIntPoint(SizeX, SizeY); }
	bool IsFullscreen() const { return bIsFullscreen; }

	class FPlatformViewportContext* GetViewportContext() const { return ViewportContext; }
protected:
	class FOpenGLRenderer	*Renderer;
	class FPlatformViewportContext	*ViewportContext;

	int32_t		SizeX;
	int32_t		SizeY;
	bool		bIsFullscreen;
};


#endif // __JETX_OPENGL_VIEWPORT_H__
