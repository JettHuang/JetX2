//\brief
//		OpenGL pipeline state data-structure.
//

#ifndef __JETX_OPENGL_STATE_H__
#define __JETX_OPENGL_STATE_H__

#include "Renderer/RendererDefs.h"
#include "Renderer/RendererState.h"
#include "Renderer/RHIResource.h"
#include "PlatformOpenGL.h"


// GL sampler state
struct FOpenGLSamplerStateData
{
	GLint	MinFilter;
	GLint	MagFilter;
	GLint	Address_s;
	GLint	Address_t;
	GLint	Address_r;

	GLfloat	MinMipLevel;
	GLfloat	MaxMipLevel;
	GLfloat	MipBias;
	GLfloat	BorderColor[4];
};

class FRHIOpenGLSamplerState : public FRHISamplerState
{
public:
	FRHIOpenGLSamplerState(class FOpenGLRenderer *InRenderer)
		: Renderer(InRenderer)
		, Resource(0)
	{
	}

	virtual ~FRHIOpenGLSamplerState();
	bool Initialize();

	class FOpenGLRenderer	*Renderer;

	GLuint					 Resource;
	FOpenGLSamplerStateData  Data;
};

// GL Rasterizer state
struct FOpenGLRasterizerStateData
{
	GLenum 					FillMode;
	GLenum					CullMode;
	GLfloat					DepthOffsetFactor;
	GLfloat					DepthOffsetUnits;
	GLboolean				bEnableDepthOffset;
	GLboolean				bAllowMSAA;
	GLboolean				bEnableLineAA;
};

class FRHIOpenGLRasterizerState : public FRHIRasterizerState
{
public:
	FRHIOpenGLRasterizerState(class FOpenGLRenderer *InRenderer)
		: Renderer(InRenderer)
	{
	}

	class FOpenGLRenderer	*Renderer;
	FOpenGLRasterizerStateData	Data;
};

// GL Depth-Stencil state
struct FOpenGLDepthStencilStateData
{
	GLboolean	bEnableZTest;
	GLboolean	bEnableZWrite;
	GLenum		ZFunc;

	GLboolean	bEnableStencilTest;
	GLenum		FrontFaceStencilCompFunc;
	GLenum		FrontFaceStencilFailOp;
	GLenum		FrontFaceDepthFailOp;
	GLenum		FrontFaceDepthPassOp;

	GLenum		BackFaceStencilCompFunc;
	GLenum		BackFaceStencilFailOp;
	GLenum		BackFaceDepthFailOp;
	GLenum		BackFaceDepthPassOp;

	GLuint		StencilReadMask;
	GLuint		StencilWriteMask;
};

class FRHIOpenGLDepthStencilState : public FRHIDepthStencilState
{
public:
	FRHIOpenGLDepthStencilState(class FOpenGLRenderer *InRenderer)
		: Renderer(InRenderer)
	{
	}

	class FOpenGLRenderer	*Renderer;
	FOpenGLDepthStencilStateData	Data;
};

// GL blend state
struct FOpenGLBlendStateData
{
	struct FRenderTagertBlendState
	{
		FRenderTagertBlendState()
			: bEnableAlphaBlend(GL_FALSE)
			, ColorBlendOp(GL_FUNC_ADD)
			, ColorSrcFactor(GL_ONE)
			, ColorDstFactor(GL_ZERO)
			, AlphaBlendOp(GL_FUNC_ADD)
			, AlphaSrcFactor(GL_ONE)
			, AlphaDstFactor(GL_ZERO)
			, bEnableWriteR(GL_TRUE)
			, bEnableWriteG(GL_TRUE)
			, bEnableWriteB(GL_TRUE)
			, bEnableWriteA(GL_TRUE)
		{}

		GLboolean	bEnableAlphaBlend;
		GLenum		ColorBlendOp;
		GLenum		ColorSrcFactor;
		GLenum		ColorDstFactor;
		GLenum		AlphaBlendOp;
		GLenum		AlphaSrcFactor;
		GLenum		AlphaDstFactor;
		GLboolean	bEnableWriteR;
		GLboolean	bEnableWriteG;
		GLboolean	bEnableWriteB;
		GLboolean	bEnableWriteA;
	};

	FRenderTagertBlendState		RenderTargetBlendStates[MaxSimultaneousRenderTargets];
};

class FRHIOpenGLBlendState : public FRHIBlendState
{
public:
	FRHIOpenGLBlendState(class FOpenGLRenderer *InRenderer)
		: Renderer(InRenderer)
		, TargetsNum(0)
	{
	}

	class FOpenGLRenderer	*Renderer;
	FOpenGLBlendStateData	Data;
	uint32_t				TargetsNum;
};


typedef TRefCountPtr<FRHIOpenGLSamplerState> FRHIOpenGLSamplerStateRef;
typedef TRefCountPtr<FRHIOpenGLRasterizerState>	FRHIOpenGLRasterizerStateRef;
typedef TRefCountPtr<FRHIOpenGLDepthStencilState> FRHIOpenGLDepthStencilStateRef;
typedef TRefCountPtr<FRHIOpenGLBlendState> FRHIOpenGLBlendStateRef;

#endif // __JETX_OPENGL_STATE_H__
