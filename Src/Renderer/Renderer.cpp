// \brief
//		Base FRenderer
//

#include "Renderer.h"
#include "Renderer/OpenGL/OpenGLRenderer.h"


FRenderer* FRenderer::CreateRender(ERendererType InRenderType)
{
	switch (InRenderType)
	{
	case RT_OpenGL:
		return new FOpenGLRenderer();
		break;
	default:
		break;
	}

	return nullptr;
}
