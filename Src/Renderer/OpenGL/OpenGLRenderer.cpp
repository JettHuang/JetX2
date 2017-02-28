//\brief
//		OpenGL Render implementation: commands.
//

#include <cassert>
#include <algorithm>

#include "OpenGLRenderer.h"
#include "OpenGLState.h"
#include "OpenGLViewport.h"
#include "OpenGLWindows.h"


static FPlatformOpenGLContext s_OpenGLContext;

//Init
void FOpenGLRenderer::Init(FOutputDevice *LogOutputDevice)
{
	Logger = LogOutputDevice;

	OpenGLContext = &s_OpenGLContext;
	PlatformInitializeOpenGLContext(*OpenGLContext);
	ViewportDrawing = nullptr;

	glGetIntegerv(GL_MAJOR_VERSION, &cap_MajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &cap_MinorVersion);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &cap_GL_MAX_TEXTURE_IMAGE_UNITS);
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &cap_GL_MAX_DRAW_BUFFERS);
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &cap_GL_MAX_ELEMENTS_VERTICES);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &cap_GL_MAX_ELEMENTS_INDICES);

	assert(cap_GL_MAX_DRAW_BUFFERS >= MaxSimultaneousRenderTargets);
	assert(cap_GL_MAX_TEXTURE_IMAGE_UNITS >= MaxTextureUnits);

	// Init States
	RenderContext.ScissorRect = FIntRect();
	RenderContext.bEnableScissorTest = GL_FALSE;
	RenderContext.ViewportBox = FViewportBox();

	glScissor(RenderContext.ScissorRect.x, RenderContext.ScissorRect.y, RenderContext.ScissorRect.width, RenderContext.ScissorRect.height);
	glDisable(GL_SCISSOR_TEST);

	glViewport(RenderContext.ViewportBox.x, RenderContext.ViewportBox.y, RenderContext.ViewportBox.width, RenderContext.ViewportBox.height);
	glDepthRange(RenderContext.ViewportBox.zMin, RenderContext.ViewportBox.zMax);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	FRasterizerStateInitializerRHI RasterizerInitializer(FM_Solid);
	FRHIRasterizerStateRef RasterizerState = RHICreateRasterizerState(RasterizerInitializer);
	assert(RasterizerState.IsValidRef());
	PendingStatesSet.RasterizerState = dynamic_cast<FRHIOpenGLRasterizerState*>(RasterizerState.DeRef());

	FDepthStencilStateInitializerRHI DepthStencilInitializer(true);
	FRHIDepthStencilStateRef DepthStencilState = RHICreateDepthStencilState(DepthStencilInitializer);
	assert(DepthStencilState.IsValidRef());
	PendingStatesSet.DepthStencilState = dynamic_cast<FRHIOpenGLDepthStencilState*>(DepthStencilState.DeRef());
	PendingStatesSet.StencilRef = 0;

	FBlendStateInitializerRHI::FRenderTargetBlendState TargetBlend;
	FBlendStateInitializerRHI BlendStateInitializer(TargetBlend);
	FRHIBlendStateRef BlendState = RHICreateBlendState(BlendStateInitializer);
	assert(BlendState.IsValidRef());
	PendingStatesSet.BlendState = dynamic_cast<FRHIOpenGLBlendState*>(BlendState.DeRef());
	PendingStatesSet.BlendColor = FLinearColor(0.f, 0.f, 0.f, 0.f);
	//Setting BlendState Cache(we use one more cache to manage blend-state switch)
	for (uint32_t k = 0; k < MaxSimultaneousRenderTargets; k++)
	{
		const FOpenGLBlendStateData::FRenderTagertBlendState &InitTargetState = RenderContext.BlendStateCache.RenderTargetBlendStates[k];
		if (InitTargetState.bEnableAlphaBlend)
		{
			glEnablei(GL_BLEND, k);
		}
		else
		{
			glDisablei(GL_BLEND, k);
		}

		glBlendEquationSeparatei(k, InitTargetState.ColorBlendOp, InitTargetState.AlphaBlendOp);
		glBlendFuncSeparatei(k, InitTargetState.ColorSrcFactor, InitTargetState.ColorDstFactor, InitTargetState.AlphaSrcFactor, InitTargetState.AlphaDstFactor);
		glColorMaski(k, InitTargetState.bEnableWriteR, InitTargetState.bEnableWriteG, InitTargetState.bEnableWriteB, InitTargetState.bEnableWriteA);

	} // end for k


	FSamplerStateInitializerRHI SamplerInitializer(SF_Bilinear);
	FRHISamplerStateRef SamplerState = RHICreateSamplerState(SamplerInitializer);
	assert(SamplerState.IsValidRef());
	PendingStatesSet.TextureSamplers[0] = dynamic_cast<FRHIOpenGLSamplerState*>(SamplerState.DeRef());
	for (uint32_t k = 1; k < MaxTextureUnits; k++)
	{
		PendingStatesSet.TextureSamplers[k] = PendingStatesSet.TextureSamplers[0];
	} // end for k

	UpdatePendingRasterizerState(true);
	UpdatePendingSamplers(true);
	UpdatePendingDepthStencilState(true);
	UpdatePendingBlendState(true);
	CheckError(__FILE__, __LINE__);

	// create vertex array object and active it.
	glGenVertexArrays(1, &RenderContext.SharedVAO);
	glBindVertexArray(RenderContext.SharedVAO);

	for (uint32_t k = 0; k < MaxBufferBinds; k++)
	{
		RenderContext.BufferBinds[k] = 0;
	}

	// Vertex Inputs
	PendingStatesSet.VertexDeclDirty = true;
	PendingStatesSet.VertexStreamsDirty = true;
}

void FOpenGLRenderer::Shutdown()
{
	if (RenderContext.SharedVAO)
	{
		glDeleteVertexArrays(1, &RenderContext.SharedVAO);
	}

	// release resource
	RenderContext.RasterizerState.SafeRelease();
	RenderContext.DepthStencilState.SafeRelease();
	RenderContext.BlendState.SafeRelease();
	PendingStatesSet.RasterizerState.SafeRelease();
	PendingStatesSet.DepthStencilState.SafeRelease();
	PendingStatesSet.BlendState.SafeRelease();
	for (uint32_t Index = 0; Index < MaxTextureUnits; Index++)
	{
		RenderContext.TextureSamplers[Index].SafeRelease();
		PendingStatesSet.TextureSamplers[Index].SafeRelease();
	}

	for (uint32_t Index = 0; Index < MaxVertexStreamSources; Index++)
	{
		PendingStatesSet.VertexStreams[Index].SafeRelease();
	}
	PendingStatesSet.VertexDecl.SafeRelease();

	PlatformShutdownOpenGLContext(*OpenGLContext);
	ViewportDrawing = nullptr;
}

//Capabilities
void FOpenGLRenderer::DumpCapabilities()
{
	if (Logger)
	{
		Logger->Log(Log_Info, "GL %d.%d Capabilities:", cap_MajorVersion, cap_MinorVersion);
		Logger->Log(Log_Info, "cap_GL_MAX_TEXTURE_IMAGE_UNITS: %d", cap_GL_MAX_TEXTURE_IMAGE_UNITS);
		Logger->Log(Log_Info, "cap_GL_MAX_DRAW_BUFFERS: %d", cap_GL_MAX_DRAW_BUFFERS);
		Logger->Log(Log_Info, "cap_GL_MAX_ELEMENTS_VERTICES: %d", cap_GL_MAX_ELEMENTS_VERTICES);
		Logger->Log(Log_Info, "cap_GL_MAX_ELEMENTS_INDICES: %d", cap_GL_MAX_ELEMENTS_INDICES);
	}
}


//Others
void FOpenGLRenderer::AddViewport(class FRHIOpenGLViewport *InViewport)
{
	Viewports.push_back(InViewport);
}

void FOpenGLRenderer::RemoveViewport(class FRHIOpenGLViewport *InViewport)
{
	std::vector<class FRHIOpenGLViewport*>::iterator Itr;
	for (Itr = Viewports.begin(); Itr != Viewports.end(); Itr++)
	{
		if (*Itr == InViewport)
		{
			Viewports.erase(Itr);
			break;
		}
	} // end for
}

//render viewport
FRHIViewportRef FOpenGLRenderer::RHICreateViewport(void* InWindowHandle, uint32_t SizeX, uint32_t SizeY, bool bIsFullscreen)
{
	return new FRHIOpenGLViewport(this, InWindowHandle, SizeX, SizeY, bIsFullscreen);
}

void FOpenGLRenderer::RHIResizeViewport(FRHIViewportRef InViewport, uint32_t SizeX, uint32_t SizeY, bool bIsFullscreen)
{
	if (InViewport.IsValidRef())
	{
		FRHIOpenGLViewport *Viewport = dynamic_cast<FRHIOpenGLViewport *>(InViewport.DeRef());
		assert(Viewport);

		Viewport->Resize(SizeX, SizeY, bIsFullscreen);
	}
}

bool FOpenGLRenderer::RHIGetAvailableResolutions(FScreenResolutionArray& Resolutions, bool bIgnoreRefreshRate)
{
	const bool Result = PlatformGetAvailableResolutions(Resolutions, bIgnoreRefreshRate);
	if (Result)
	{
		std::sort(Resolutions.begin(), Resolutions.end());
	}

	return Result;
}

void FOpenGLRenderer::RHIGetSupportedResolution(uint32_t& Width, uint32_t& Height)
{
	PlatformGetSupportedResolution(Width, Height);
}


void FOpenGLRenderer::RHIBeginDrawingViewport(FRHIViewportRef Viewport)
{
	FRHIOpenGLViewport *GLViewport = dynamic_cast<FRHIOpenGLViewport*>(Viewport.DeRef());
	assert(GLViewport);

	if (GLViewport != ViewportDrawing)
	{
		PlatformActiveViewportContext(OpenGLContext, GLViewport->GetViewportContext());
		ViewportDrawing = GLViewport;
	}
}

void FOpenGLRenderer::RHIEndDrawingViewport(FRHIViewportRef Viewport, bool bPresent, bool bLockToVsync)
{
	CheckError(__FILE__, __LINE__);

	FRHIOpenGLViewport *GLViewport = dynamic_cast<FRHIOpenGLViewport*>(Viewport.DeRef());
	assert(GLViewport);
	assert(ViewportDrawing == GLViewport);

	glFlush();
	PlatformSwapBuffers(OpenGLContext, GLViewport->GetViewportContext());
}

void FOpenGLRenderer::RHIBeginFrame()
{

}

void FOpenGLRenderer::RHIEndFrame()
{

}

void FOpenGLRenderer::UpdatePendingRasterizerState(bool bForce)
{
	FRHIOpenGLRasterizerState *Pending = PendingStatesSet.RasterizerState.DeRef();
	FRHIOpenGLRasterizerState *Current = RenderContext.RasterizerState.DeRef();

	if (!Pending)
	{
		return;
	}
	if (Pending == Current)
	{
		PendingStatesSet.RasterizerState = nullptr;
		return;
	}

	if (bForce || !Current)
	{
		glPolygonMode(GL_FRONT_AND_BACK, Pending->Data.FillMode);
		glCullFace(Pending->Data.CullMode);
		if (Pending->Data.bEnableDepthOffset)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
		}
		else
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
		glPolygonOffset(Pending->Data.DepthOffsetFactor, Pending->Data.DepthOffsetUnits);
	}
	else
	{
		assert(Current);
		if (Current->Data.FillMode != Pending->Data.FillMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, Pending->Data.FillMode);
		}
		if (Current->Data.CullMode != Pending->Data.CullMode)
		{
			glCullFace(Pending->Data.CullMode);
		}
		if (Current->Data.bEnableDepthOffset != Pending->Data.bEnableDepthOffset)
		{
			if (Pending->Data.bEnableDepthOffset)
			{
				glEnable(GL_POLYGON_OFFSET_FILL);
			}
			else
			{
				glDisable(GL_POLYGON_OFFSET_FILL);
			}
		}
		if (Current->Data.DepthOffsetFactor != Pending->Data.DepthOffsetFactor
			|| Current->Data.DepthOffsetUnits != Pending->Data.DepthOffsetUnits)
		{
			glPolygonOffset(Pending->Data.DepthOffsetFactor, Pending->Data.DepthOffsetUnits);
		}
	}

	RenderContext.RasterizerState = PendingStatesSet.RasterizerState;
	PendingStatesSet.RasterizerState = nullptr;
}

void FOpenGLRenderer::UpdatePendingSamplers(bool bForce)
{
	for (uint32_t k = 0; k < MaxTextureUnits; k++)
	{
		FRHIOpenGLSamplerState *Pending = PendingStatesSet.TextureSamplers[k].DeRef();
		FRHIOpenGLSamplerState *Current = RenderContext.TextureSamplers[k].DeRef();

		if (!Pending)
		{
			continue;
		}
		if (Pending == Current)
		{
			PendingStatesSet.TextureSamplers[k] = nullptr;
			continue;
		}

		if (bForce || !Current)
		{
			glBindSampler(k, Pending->Resource);
			CheckError(__FILE__, __LINE__);
		}
		else
		{
			assert(Current);
			if (Current->Resource != Pending->Resource)
			{
				glBindSampler(k, Pending->Resource);
			}
		}

		RenderContext.TextureSamplers[k] = Pending;
		PendingStatesSet.TextureSamplers[k] = nullptr;
	} // end for k
}

void FOpenGLRenderer::UpdatePendingDepthStencilState(bool bForce)
{
	FRHIOpenGLDepthStencilState *Pending = PendingStatesSet.DepthStencilState.DeRef();
	FRHIOpenGLDepthStencilState *Current = RenderContext.DepthStencilState.DeRef();

	if (!Pending)
	{
		return;
	}

	if (bForce || !Current)
	{
		if (Pending->Data.bEnableZTest)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		glDepthFunc(Pending->Data.ZFunc);
		glDepthMask(Pending->Data.bEnableZWrite);

		if (Pending->Data.bEnableStencilTest)
		{
			glEnable(GL_STENCIL_TEST);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}

		glStencilFuncSeparate(GL_FRONT, Pending->Data.FrontFaceStencilCompFunc, PendingStatesSet.StencilRef, Pending->Data.StencilReadMask);
		glStencilOpSeparate(GL_FRONT, Pending->Data.FrontFaceStencilFailOp, Pending->Data.FrontFaceDepthFailOp, Pending->Data.FrontFaceDepthPassOp);
		glStencilFuncSeparate(GL_BACK, Pending->Data.BackFaceStencilCompFunc, PendingStatesSet.StencilRef, Pending->Data.StencilReadMask);
		glStencilOpSeparate(GL_BACK, Pending->Data.BackFaceStencilFailOp, Pending->Data.BackFaceDepthFailOp, Pending->Data.BackFaceDepthPassOp);

		glStencilMask(Pending->Data.StencilWriteMask);
	}
	else
	{
		assert(Current);
		if (Pending != Current)
		{
			if (Current->Data.bEnableZTest != Pending->Data.bEnableZTest)
			{
				if (Pending->Data.bEnableZTest)
				{
					glEnable(GL_DEPTH_TEST);
				}
				else
				{
					glDisable(GL_DEPTH_TEST);
				}
			}

			if (Current->Data.ZFunc != Pending->Data.ZFunc)
			{
				glDepthFunc(Pending->Data.ZFunc);
			}
			if (Current->Data.bEnableZWrite != Pending->Data.bEnableZWrite)
			{
				glDepthMask(Pending->Data.bEnableZWrite);
			}

			if (Current->Data.bEnableStencilTest != Pending->Data.bEnableStencilTest)
			{
				if (Pending->Data.bEnableStencilTest)
				{
					glEnable(GL_STENCIL_TEST);
				}
				else
				{
					glDisable(GL_STENCIL_TEST);
				}
			}

			if (Current->Data.FrontFaceStencilCompFunc != Pending->Data.FrontFaceStencilCompFunc
				|| RenderContext.StencilRef != PendingStatesSet.StencilRef
				|| Current->Data.StencilReadMask != Pending->Data.StencilReadMask)
			{
				glStencilFuncSeparate(GL_FRONT, Pending->Data.FrontFaceStencilCompFunc, PendingStatesSet.StencilRef, Pending->Data.StencilReadMask);
			}
			if (Current->Data.FrontFaceStencilFailOp != Pending->Data.FrontFaceStencilFailOp
				|| Current->Data.FrontFaceDepthFailOp != Pending->Data.FrontFaceDepthFailOp
				|| Current->Data.FrontFaceDepthPassOp != Pending->Data.FrontFaceDepthPassOp)
			{
				glStencilOpSeparate(GL_FRONT, Pending->Data.FrontFaceStencilFailOp, Pending->Data.FrontFaceDepthFailOp, Pending->Data.FrontFaceDepthPassOp);
			}

			if (Current->Data.BackFaceStencilCompFunc != Pending->Data.BackFaceStencilCompFunc
				|| RenderContext.StencilRef != PendingStatesSet.StencilRef
				|| Current->Data.StencilReadMask != Pending->Data.StencilReadMask)
			{
				glStencilFuncSeparate(GL_BACK, Pending->Data.BackFaceStencilCompFunc, PendingStatesSet.StencilRef, Pending->Data.StencilReadMask);
			}
			if (Current->Data.BackFaceStencilFailOp != Pending->Data.BackFaceStencilFailOp
				|| Current->Data.BackFaceDepthFailOp != Pending->Data.BackFaceDepthFailOp
				|| Current->Data.BackFaceDepthPassOp != Pending->Data.BackFaceDepthPassOp)
			{
				glStencilOpSeparate(GL_BACK, Pending->Data.BackFaceStencilFailOp, Pending->Data.BackFaceDepthFailOp, Pending->Data.BackFaceDepthPassOp);
			}

			if (Current->Data.StencilWriteMask != Pending->Data.StencilWriteMask)
			{
				glStencilMask(Pending->Data.StencilWriteMask);
			}
		}
		else
		{
			if (RenderContext.StencilRef != PendingStatesSet.StencilRef)
			{
				glStencilFuncSeparate(GL_FRONT, Pending->Data.FrontFaceStencilCompFunc, PendingStatesSet.StencilRef, Pending->Data.StencilReadMask);
				glStencilFuncSeparate(GL_BACK, Pending->Data.BackFaceStencilCompFunc, PendingStatesSet.StencilRef, Pending->Data.StencilReadMask);
			}
		}
	}

	RenderContext.DepthStencilState = PendingStatesSet.DepthStencilState;
	RenderContext.StencilRef = PendingStatesSet.StencilRef;
	PendingStatesSet.DepthStencilState = nullptr;
}

void FOpenGLRenderer::UpdatePendingBlendState(bool bForce)
{
	FRHIOpenGLBlendState *Pending = PendingStatesSet.BlendState;
	FRHIOpenGLBlendState *Current = RenderContext.BlendState;

	if (!Pending)
	{
		return;
	}
	if (bForce || !Current)
	{
		const uint32_t kMaxRenderTargets = (std::min)(Pending->TargetsNum, (uint32_t)MaxSimultaneousRenderTargets);
		for (uint32_t k = 0; k < kMaxRenderTargets; k++)
		{
			const FOpenGLBlendStateData::FRenderTagertBlendState &PendingTargetState = Pending->Data.RenderTargetBlendStates[k];
			if (PendingTargetState.bEnableAlphaBlend)
			{
				glEnablei(GL_BLEND, k);
			}
			else
			{
				glDisablei(GL_BLEND, k);
			}

			glBlendEquationSeparatei(k, PendingTargetState.ColorBlendOp, PendingTargetState.AlphaBlendOp);
			glBlendFuncSeparatei(k, PendingTargetState.ColorSrcFactor, PendingTargetState.ColorDstFactor, PendingTargetState.AlphaSrcFactor, PendingTargetState.AlphaDstFactor);
			glColorMaski(k, PendingTargetState.bEnableWriteR, PendingTargetState.bEnableWriteG, PendingTargetState.bEnableWriteB, PendingTargetState.bEnableWriteA);
			
		} // end for k
		const FLinearColor &BlendColor = PendingStatesSet.BlendColor;
		glBlendColor(BlendColor.R, BlendColor.G, BlendColor.B, BlendColor.A);
	}
	else
	{
		if (Current != Pending)
		{
			const uint32_t kMatchedTargets = (std::min)(Pending->TargetsNum, Current->TargetsNum);
			for (uint32_t k = 0; k < kMatchedTargets; k++)
			{
				const FOpenGLBlendStateData::FRenderTagertBlendState &PendingTargetState = Pending->Data.RenderTargetBlendStates[k];
				const FOpenGLBlendStateData::FRenderTagertBlendState &CurrentTargetState = Current->Data.RenderTargetBlendStates[k];
				if (CurrentTargetState.bEnableAlphaBlend != PendingTargetState.bEnableAlphaBlend)
				{
					if (PendingTargetState.bEnableAlphaBlend)
					{
						glEnablei(GL_BLEND, k);
					}
					else
					{
						glDisablei(GL_BLEND, k);
					}
				}

				if (CurrentTargetState.ColorBlendOp != PendingTargetState.ColorBlendOp
					|| CurrentTargetState.AlphaBlendOp != PendingTargetState.AlphaBlendOp)
				{
					glBlendEquationSeparatei(k, PendingTargetState.ColorBlendOp, PendingTargetState.AlphaBlendOp);
				}
				if (CurrentTargetState.ColorSrcFactor != PendingTargetState.ColorSrcFactor
					|| CurrentTargetState.ColorDstFactor != PendingTargetState.ColorDstFactor
					|| CurrentTargetState.AlphaSrcFactor != PendingTargetState.AlphaSrcFactor
					|| CurrentTargetState.AlphaDstFactor != PendingTargetState.AlphaDstFactor
					)
				{
					glBlendFuncSeparatei(k, PendingTargetState.ColorSrcFactor, PendingTargetState.ColorDstFactor, PendingTargetState.AlphaSrcFactor, PendingTargetState.AlphaDstFactor);
				}
				if (CurrentTargetState.bEnableWriteR != PendingTargetState.bEnableWriteR
					|| CurrentTargetState.bEnableWriteG != PendingTargetState.bEnableWriteG
					|| CurrentTargetState.bEnableWriteB != PendingTargetState.bEnableWriteB
					|| CurrentTargetState.bEnableWriteA != PendingTargetState.bEnableWriteA
					)
				{
					glColorMaski(k, PendingTargetState.bEnableWriteR, PendingTargetState.bEnableWriteG, PendingTargetState.bEnableWriteB, PendingTargetState.bEnableWriteA);
				}
			} // end for k
		
			// compare with cache
			for (uint32_t k = kMatchedTargets; k < Pending->TargetsNum; k++)
			{
				const FOpenGLBlendStateData::FRenderTagertBlendState &PendingTargetState = Pending->Data.RenderTargetBlendStates[k];
				const FOpenGLBlendStateData::FRenderTagertBlendState &CurrentTargetState = RenderContext.BlendStateCache.RenderTargetBlendStates[k];
				if (CurrentTargetState.bEnableAlphaBlend != PendingTargetState.bEnableAlphaBlend)
				{
					if (PendingTargetState.bEnableAlphaBlend)
					{
						glEnablei(GL_BLEND, k);
					}
					else
					{
						glDisablei(GL_BLEND, k);
					}
				}

				if (CurrentTargetState.ColorBlendOp != PendingTargetState.ColorBlendOp
					|| CurrentTargetState.AlphaBlendOp != PendingTargetState.AlphaBlendOp)
				{
					glBlendEquationSeparatei(k, PendingTargetState.ColorBlendOp, PendingTargetState.AlphaBlendOp);
				}
				if (CurrentTargetState.ColorSrcFactor != PendingTargetState.ColorSrcFactor
					|| CurrentTargetState.ColorDstFactor != PendingTargetState.ColorDstFactor
					|| CurrentTargetState.AlphaSrcFactor != PendingTargetState.AlphaSrcFactor
					|| CurrentTargetState.AlphaDstFactor != PendingTargetState.AlphaDstFactor
					)
				{
					glBlendFuncSeparatei(k, PendingTargetState.ColorSrcFactor, PendingTargetState.ColorDstFactor, PendingTargetState.AlphaSrcFactor, PendingTargetState.AlphaDstFactor);
				}
				if (CurrentTargetState.bEnableWriteR != PendingTargetState.bEnableWriteR
					|| CurrentTargetState.bEnableWriteG != PendingTargetState.bEnableWriteG
					|| CurrentTargetState.bEnableWriteB != PendingTargetState.bEnableWriteB
					|| CurrentTargetState.bEnableWriteA != PendingTargetState.bEnableWriteA
					)
				{
					glColorMaski(k, PendingTargetState.bEnableWriteR, PendingTargetState.bEnableWriteG, PendingTargetState.bEnableWriteB, PendingTargetState.bEnableWriteA);
				}
			} // end for k

			// synchronize to cache
			for (uint32_t k = kMatchedTargets; k < Current->TargetsNum; k++)
			{
				const FOpenGLBlendStateData::FRenderTagertBlendState &SrcTargetState = Current->Data.RenderTargetBlendStates[k];
				FOpenGLBlendStateData::FRenderTagertBlendState &DstTargetState = RenderContext.BlendStateCache.RenderTargetBlendStates[k];

				DstTargetState = SrcTargetState;
			} // end for k
		}

		if (RenderContext.BlendColor != PendingStatesSet.BlendColor)
		{
			glBlendColor(PendingStatesSet.BlendColor.R, PendingStatesSet.BlendColor.G, PendingStatesSet.BlendColor.B, PendingStatesSet.BlendColor.A);
		}
	}

	RenderContext.BlendState = PendingStatesSet.BlendState;
	RenderContext.BlendColor = PendingStatesSet.BlendColor;
	PendingStatesSet.BlendState = nullptr;
}

void FOpenGLRenderer::UpdatePendingVertexInputLayout(bool bForce)
{
	bool bNeedUpdate = PendingStatesSet.VertexDeclDirty || PendingStatesSet.VertexStreamsDirty || bForce;
	if (!bNeedUpdate)
	{
		return;
	}

	assert(PendingStatesSet.VertexDecl.IsValidRef());

	GLboolean VertexAttrisEnables[MaxVertexAttributes] = { GL_FALSE };
	const FOpenGLVertexElementsList &VertexElementList = PendingStatesSet.VertexDecl->VertexInputLayout;
	for (size_t Index = 0; Index < VertexElementList.size(); Index++)
	{
		const FOpenGLVertexElement &Element = VertexElementList[Index];
		const GLuint kStreamIndex = Element.StreamIndex;
		const GLuint kAttriIndex = Element.AttributeIndex;

		assert(kAttriIndex < MaxVertexAttributes);
		assert(kStreamIndex < MaxVertexStreamSources);

		FRHIOpenGLVertexBuffer *SourceStream = PendingStatesSet.VertexStreams[kStreamIndex];
		assert(SourceStream);

		CachedEnableVertexAttributePointer(SourceStream->NativeResource(), Element);
		VertexAttrisEnables[kAttriIndex] = GL_TRUE;
	} // end for

	for (GLuint Index = 0; Index < MaxVertexAttributes; Index++)
	{
		if (VertexAttrisEnables[Index] == GL_FALSE && RenderContext.VAOState.VertexInputAttris[Index].Enabled)
		{
			glDisableVertexAttribArray(Index);
			CheckError(__FILE__, __LINE__);
			RenderContext.VAOState.VertexInputAttris[Index].Enabled = GL_FALSE;
		}
	} // end for
}

//////////////////////////////////////////////////////////////////////////
// Others

void FOpenGLRenderer::CachedEnableVertexAttributePointer(GLuint InBuffer, const FOpenGLVertexElement &InVertexElement)
{
	const GLuint kAttriIndex = InVertexElement.AttributeIndex;
	FVertexInputAttribute &CurrentInputAttribute = RenderContext.VAOState.VertexInputAttris[kAttriIndex];

	if (CurrentInputAttribute.Buffer != InBuffer ||
		CurrentInputAttribute.Type != InVertexElement.Type ||
		CurrentInputAttribute.Size != InVertexElement.Size ||
		CurrentInputAttribute.Stride != InVertexElement.Stride ||
		CurrentInputAttribute.Offset != InVertexElement.Offset ||
		CurrentInputAttribute.ShouldConvertToFloat != InVertexElement.ShouldConvertToFloat ||
		CurrentInputAttribute.Normalized != InVertexElement.Normalized)
	{
		CachedBindBuffer(Array_Buffer, InBuffer);
		if (InVertexElement.ShouldConvertToFloat)
		{
			glVertexAttribPointer(kAttriIndex, InVertexElement.Size, InVertexElement.Type, InVertexElement.Normalized, InVertexElement.Stride, (GLvoid*)InVertexElement.Offset);
		}
		else
		{
			glVertexAttribIPointer(kAttriIndex, InVertexElement.Size, InVertexElement.Type, InVertexElement.Stride, (GLvoid*)InVertexElement.Offset);
		}
		CheckError(__FILE__, __LINE__);

		CurrentInputAttribute.Buffer = InBuffer;
		CurrentInputAttribute.Type = InVertexElement.Type;
		CurrentInputAttribute.Size = InVertexElement.Size;
		CurrentInputAttribute.Stride = InVertexElement.Stride;
		CurrentInputAttribute.Offset = InVertexElement.Offset;
		CurrentInputAttribute.Normalized = InVertexElement.Normalized;
	}

	if (CurrentInputAttribute.Divisor != InVertexElement.Divisor)
	{
		glVertexAttribDivisor(kAttriIndex, InVertexElement.Divisor);
		CurrentInputAttribute.Divisor = InVertexElement.Divisor;
	}

	if (!CurrentInputAttribute.Enabled)
	{
		glEnableVertexAttribArray(kAttriIndex);
		CheckError(__FILE__, __LINE__);

		CurrentInputAttribute.Enabled = GL_TRUE;
	}
}



void FOpenGLRenderer::CachedBindBuffer(EBufferBindTarget InBindPoint, GLuint InBuffer)
{
	if (RenderContext.BufferBinds[InBindPoint] != InBuffer)
	{
		glBindBuffer(FOpenGLBuffer::TranslateBindTarget(InBindPoint), InBuffer);
		RenderContext.BufferBinds[InBindPoint] = InBuffer;
	}
}

void FOpenGLRenderer::OnBufferDeleted(EBufferBindTarget InBindPoint, GLuint InBuffer)
{
	if (RenderContext.BufferBinds[InBindPoint] == InBuffer)
	{
		RenderContext.BufferBinds[InBindPoint] = 0;
	}
	
	switch (InBindPoint)
	{
	case Array_Buffer:
	{
		for (uint32_t Index = 0; Index < MaxVertexAttributes; Index++)
		{
			FVertexInputAttribute &Entry = RenderContext.VAOState.VertexInputAttris[Index];
			if (Entry.Buffer == InBuffer)
			{
				Entry.Buffer = 0;
			}
		}
	}
		break;
	default:
		break;
	}
	
}


//////////////////////////////////////////////////////////////////////////
// helpers

bool FOpenGLRenderer::CheckError(const char* FILE, int LINE)
{
	GLenum Error = glGetError();

	if (Error != GL_NO_ERROR && Logger)
	{
		Logger->Log(Log_Error, "OpenGL Error at %s:%d, error code=%0Xd, reason:%s", FILE, LINE, Error, LookupErrorCode(Error));
	}

	return (Error != GL_NO_ERROR);
}

struct FTypeNamePair
{
	GLenum	Type;
	const char *Name;
};
static const char *kUnknown = "unknown";

#define DEF_TYPENAME_PAIR(Type)		{ Type, #Type }
#define DEF_ARRAYCOUNT(Array)		(sizeof(Array)/sizeof(Array[0]))

const char* FOpenGLRenderer::LookupShaderAttributeTypeName(GLenum InType)
{
	static const FTypeNamePair kTypeNames[] =
	{
		DEF_TYPENAME_PAIR(GL_FLOAT),
		DEF_TYPENAME_PAIR(GL_FLOAT_VEC2),
		DEF_TYPENAME_PAIR(GL_FLOAT_VEC3),
		DEF_TYPENAME_PAIR(GL_FLOAT_VEC4),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT2),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT3),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT4),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT2x3),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT2x4),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT3x2),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT3x4),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT4x2),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT4x3),
		DEF_TYPENAME_PAIR(GL_INT),
		DEF_TYPENAME_PAIR(GL_INT_VEC2),
		DEF_TYPENAME_PAIR(GL_INT_VEC3),
		DEF_TYPENAME_PAIR(GL_INT_VEC4),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_VEC2),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_VEC3),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_VEC4),
		DEF_TYPENAME_PAIR(GL_DOUBLE),
		DEF_TYPENAME_PAIR(GL_DOUBLE_VEC2),
		DEF_TYPENAME_PAIR(GL_DOUBLE_VEC3),
		DEF_TYPENAME_PAIR(GL_DOUBLE_VEC4),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT2),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT3),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT4),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT2x3),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT2x4),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT3x2),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT3x4),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT4x2),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT4x3)
	};

	for (GLint Index = 0; Index < DEF_ARRAYCOUNT(kTypeNames); Index++)
	{
		const FTypeNamePair &Element = kTypeNames[Index];
		if (Element.Type == InType)
		{
			return Element.Name;
		}
	} // end for

	return kUnknown;
}

const char* FOpenGLRenderer::LookupShaderUniformTypeName(GLenum InType)
{
	static const FTypeNamePair kTypeNames[] =
	{
		DEF_TYPENAME_PAIR(GL_FLOAT),
		DEF_TYPENAME_PAIR(GL_FLOAT_VEC2),
		DEF_TYPENAME_PAIR(GL_FLOAT_VEC3),
		DEF_TYPENAME_PAIR(GL_FLOAT_VEC4),
		DEF_TYPENAME_PAIR(GL_DOUBLE),
		DEF_TYPENAME_PAIR(GL_DOUBLE_VEC2),
		DEF_TYPENAME_PAIR(GL_DOUBLE_VEC3),
		DEF_TYPENAME_PAIR(GL_DOUBLE_VEC4),
		DEF_TYPENAME_PAIR(GL_INT),
		DEF_TYPENAME_PAIR(GL_INT_VEC2),
		DEF_TYPENAME_PAIR(GL_INT_VEC3),
		DEF_TYPENAME_PAIR(GL_INT_VEC4),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_VEC2),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_VEC3),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_VEC4),
		DEF_TYPENAME_PAIR(GL_BOOL),
		DEF_TYPENAME_PAIR(GL_BOOL_VEC2),
		DEF_TYPENAME_PAIR(GL_BOOL_VEC3),
		DEF_TYPENAME_PAIR(GL_BOOL_VEC4),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT2),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT3),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT4),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT2x3),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT2x4),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT3x2),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT3x4),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT4x2),
		DEF_TYPENAME_PAIR(GL_FLOAT_MAT4x3),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT2),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT3),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT4),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT2x3),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT2x4),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT3x2),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT3x4),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT4x2),
		DEF_TYPENAME_PAIR(GL_DOUBLE_MAT4x3),
		DEF_TYPENAME_PAIR(GL_SAMPLER_1D),
		DEF_TYPENAME_PAIR(GL_SAMPLER_2D),
		DEF_TYPENAME_PAIR(GL_SAMPLER_3D),
		DEF_TYPENAME_PAIR(GL_SAMPLER_CUBE),
		DEF_TYPENAME_PAIR(GL_SAMPLER_1D_SHADOW),
		DEF_TYPENAME_PAIR(GL_SAMPLER_2D_SHADOW),
		DEF_TYPENAME_PAIR(GL_SAMPLER_1D_ARRAY),
		DEF_TYPENAME_PAIR(GL_SAMPLER_2D_ARRAY),
		DEF_TYPENAME_PAIR(GL_SAMPLER_1D_ARRAY_SHADOW),
		DEF_TYPENAME_PAIR(GL_SAMPLER_2D_ARRAY_SHADOW),
		DEF_TYPENAME_PAIR(GL_SAMPLER_2D_MULTISAMPLE),
		DEF_TYPENAME_PAIR(GL_SAMPLER_2D_MULTISAMPLE_ARRAY),
		DEF_TYPENAME_PAIR(GL_SAMPLER_CUBE_SHADOW),
		DEF_TYPENAME_PAIR(GL_SAMPLER_BUFFER),
		DEF_TYPENAME_PAIR(GL_SAMPLER_2D_RECT),
		DEF_TYPENAME_PAIR(GL_SAMPLER_2D_RECT_SHADOW),
		DEF_TYPENAME_PAIR(GL_INT_SAMPLER_1D),
		DEF_TYPENAME_PAIR(GL_INT_SAMPLER_2D),
		DEF_TYPENAME_PAIR(GL_INT_SAMPLER_3D),
		DEF_TYPENAME_PAIR(GL_INT_SAMPLER_CUBE),
		DEF_TYPENAME_PAIR(GL_INT_SAMPLER_1D_ARRAY),
		DEF_TYPENAME_PAIR(GL_INT_SAMPLER_2D_ARRAY),
		DEF_TYPENAME_PAIR(GL_INT_SAMPLER_2D_MULTISAMPLE),
		DEF_TYPENAME_PAIR(GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY),
		DEF_TYPENAME_PAIR(GL_INT_SAMPLER_BUFFER),
		DEF_TYPENAME_PAIR(GL_INT_SAMPLER_2D_RECT),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_SAMPLER_1D),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_SAMPLER_2D),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_SAMPLER_3D),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_SAMPLER_CUBE),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_SAMPLER_1D_ARRAY),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_SAMPLER_BUFFER),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_SAMPLER_2D_RECT),
		DEF_TYPENAME_PAIR(GL_IMAGE_1D),
		DEF_TYPENAME_PAIR(GL_IMAGE_2D),
		DEF_TYPENAME_PAIR(GL_IMAGE_3D),
		DEF_TYPENAME_PAIR(GL_IMAGE_2D_RECT),
		DEF_TYPENAME_PAIR(GL_IMAGE_CUBE),
		DEF_TYPENAME_PAIR(GL_IMAGE_BUFFER),
		DEF_TYPENAME_PAIR(GL_IMAGE_1D_ARRAY),
		DEF_TYPENAME_PAIR(GL_IMAGE_2D_ARRAY),
		DEF_TYPENAME_PAIR(GL_IMAGE_2D_MULTISAMPLE),
		DEF_TYPENAME_PAIR(GL_IMAGE_2D_MULTISAMPLE_ARRAY),
		DEF_TYPENAME_PAIR(GL_INT_IMAGE_1D),
		DEF_TYPENAME_PAIR(GL_INT_IMAGE_2D),
		DEF_TYPENAME_PAIR(GL_INT_IMAGE_3D),
		DEF_TYPENAME_PAIR(GL_INT_IMAGE_2D_RECT),
		DEF_TYPENAME_PAIR(GL_INT_IMAGE_CUBE),
		DEF_TYPENAME_PAIR(GL_INT_IMAGE_BUFFER),
		DEF_TYPENAME_PAIR(GL_INT_IMAGE_1D_ARRAY),
		DEF_TYPENAME_PAIR(GL_INT_IMAGE_2D_ARRAY),
		DEF_TYPENAME_PAIR(GL_INT_IMAGE_2D_MULTISAMPLE),
		DEF_TYPENAME_PAIR(GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_IMAGE_1D),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_IMAGE_2D),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_IMAGE_3D),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_IMAGE_2D_RECT),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_IMAGE_CUBE),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_IMAGE_BUFFER),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_IMAGE_1D_ARRAY),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_IMAGE_2D_ARRAY),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY),
		DEF_TYPENAME_PAIR(GL_UNSIGNED_INT_ATOMIC_COUNTER)
	};

	for (GLint Index = 0; Index < DEF_ARRAYCOUNT(kTypeNames); Index++)
	{
		const FTypeNamePair &Element = kTypeNames[Index];
		if (Element.Type == InType)
		{
			return Element.Name;
		}
	} // end for

	return kUnknown;
}

const char* FOpenGLRenderer::LookupErrorCode(GLenum InError)
{
	static const FTypeNamePair kTypeNames[] =
	{
		DEF_TYPENAME_PAIR(GL_NO_ERROR),
		DEF_TYPENAME_PAIR(GL_INVALID_ENUM),
		DEF_TYPENAME_PAIR(GL_INVALID_VALUE),
		DEF_TYPENAME_PAIR(GL_INVALID_OPERATION),
		DEF_TYPENAME_PAIR(GL_INVALID_FRAMEBUFFER_OPERATION),
		DEF_TYPENAME_PAIR(GL_OUT_OF_MEMORY),
		DEF_TYPENAME_PAIR(GL_STACK_UNDERFLOW),
		DEF_TYPENAME_PAIR(GL_STACK_OVERFLOW)
	};

	for (GLint Index = 0; Index < DEF_ARRAYCOUNT(kTypeNames); Index++)
	{
		const FTypeNamePair &Element = kTypeNames[Index];
		if (Element.Type == InError)
		{
			return Element.Name;
		}
	} // end for

	return kUnknown;
}
