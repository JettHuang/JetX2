// \brief
//		OpenGL Viewport implementation
//

#include "OpenGLViewport.h"
#include "OpenGLRenderer.h"


FRHIOpenGLViewport::FRHIOpenGLViewport(class FOpenGLRenderer *InRenderer, void *InWindowHandle, int32_t InSizeX, int32_t InSizeY, bool InbIsFullscreen)
	: Renderer(InRenderer)
	, ViewportContext(nullptr)
	, SizeX(0)
	, SizeY(0)
	, bIsFullscreen(false)
{
	Renderer->AddViewport(this);
	ViewportContext = InRenderer->CreateViewportContext(InWindowHandle);
	Resize(InSizeX, InSizeY, InbIsFullscreen);
}

FRHIOpenGLViewport::~FRHIOpenGLViewport()
{
	if (bIsFullscreen)
	{
		PlatformRestoreDesktopDisplayMode();
	}

	Renderer->ReleaseViewportContext(ViewportContext);
	ViewportContext = nullptr;
	Renderer->RemoveViewport(this);
}

void FRHIOpenGLViewport::Resize(uint32_t InSizeX, uint32_t InSizeY, bool InbIsFullscreen)
{
	if ((InSizeX == SizeX) && (InSizeY == SizeY) && (InbIsFullscreen == bIsFullscreen))
	{
		return;
	}

	Renderer->ResizeViewportContext(ViewportContext, InSizeX, InSizeY, InbIsFullscreen, bIsFullscreen);
	SizeX = InSizeX;
	SizeY = InSizeY;
	bIsFullscreen = InbIsFullscreen;
}
