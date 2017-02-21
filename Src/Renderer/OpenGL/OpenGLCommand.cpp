//\brief
//		OpenGL Render implementation: commands.
//

#include <cassert>

#include "OpenGLRenderer.h"
#include "OpenGLState.h"


void FOpenGLRenderer::RHISetSamplerState(uint32_t InTexIndex, const FRHISamplerStateRef &InSamplerState)
{
	if (InSamplerState.IsValidRef())
	{
		assert(InTexIndex < MaxTextureUnits);

		FRHIOpenGLSamplerState *OpenGLSampler = dynamic_cast<FRHIOpenGLSamplerState*>(InSamplerState.DeRef());
		assert(OpenGLSampler != nullptr);

		PendingStatesSet.TextureSamplers[InTexIndex] = OpenGLSampler;
	}
}

void FOpenGLRenderer::RHISetRasterizerState(const FRHIRasterizerStateRef &InRasterizerState)
{
	if (InRasterizerState.IsValidRef())
	{
		FRHIOpenGLRasterizerState *OpenGLRasterizerState = dynamic_cast<FRHIOpenGLRasterizerState*>(InRasterizerState.DeRef());
		assert(OpenGLRasterizerState != nullptr);

		PendingStatesSet.RasterizerState = OpenGLRasterizerState;
	}
}

void FOpenGLRenderer::RHISetDepthStencilState(const FRHIDepthStencilStateRef &InDepthStencilState, int32_t InStencilRef)
{
	if (InDepthStencilState.IsValidRef())
	{
		FRHIOpenGLDepthStencilState *OpenGLDepthStencilState = dynamic_cast<FRHIOpenGLDepthStencilState*>(InDepthStencilState.DeRef());
		assert(OpenGLDepthStencilState != nullptr);

		PendingStatesSet.DepthStencilState = OpenGLDepthStencilState;
	}

	PendingStatesSet.StencilRef = InStencilRef;
}

void FOpenGLRenderer::RHISetBlendState(const FRHIBlendStateRef &InBlendState, const FLinearColor &InBlendColor)
{
	if (InBlendState.IsValidRef())
	{
		FRHIOpenGLBlendState *OpenGLBlendState = dynamic_cast<FRHIOpenGLBlendState*>(InBlendState.DeRef());
		assert(OpenGLBlendState != nullptr);

		PendingStatesSet.BlendState = OpenGLBlendState;
	}

	PendingStatesSet.BlendColor = InBlendColor;
}

void FOpenGLRenderer::RHISetViewport(int32_t InX, int32_t InY, int32_t InWidth, int32_t InHeight, float InMinZ, float InMaxZ)
{
	if (InX != RenderContext.ViewportBox.x
		|| InY != RenderContext.ViewportBox.y
		|| InWidth != RenderContext.ViewportBox.width
		|| InHeight != RenderContext.ViewportBox.height
		)
	{
		glViewport(InX, InY, InWidth, InHeight);
		RenderContext.ViewportBox.x = InX;
		RenderContext.ViewportBox.y = InY;
		RenderContext.ViewportBox.width = InWidth;
		RenderContext.ViewportBox.height = InHeight;
	}

	if (InMinZ != RenderContext.ViewportBox.zMin
		|| InMaxZ != RenderContext.ViewportBox.zMax
		)
	{
		glDepthRangef(InMinZ, InMaxZ);
		RenderContext.ViewportBox.zMin = InMinZ;
		RenderContext.ViewportBox.zMax = InMaxZ;
	}
}

void FOpenGLRenderer::RHISetScissorRect(int32_t InX, int32_t InY, int32_t InWidth, int32_t InHeight)
{
	if (InX != RenderContext.ScissorRect.x
		|| InY != RenderContext.ScissorRect.y
		|| InWidth != RenderContext.ScissorRect.width
		|| InHeight != RenderContext.ScissorRect.height
		)
	{
		glScissor(InX, InY, InWidth, InHeight);
		RenderContext.ScissorRect.x = InX;
		RenderContext.ScissorRect.y = InY;
		RenderContext.ScissorRect.width = InWidth;
		RenderContext.ScissorRect.height = InHeight;
	}
}

// Commands
void FOpenGLRenderer::RHIClear(bool bClearColor, const FLinearColor &InColor, bool bClearDepth, float InDepth, bool bClearStencil, int32_t InStencil)
{
	RHIClearMRT(bClearColor, &InColor, 1, bClearDepth, InDepth, bClearStencil, InStencil);
}

void FOpenGLRenderer::RHIClearMRT(bool bClearColor, const FLinearColor *InColors, uint32_t InColorsNum, bool bClearDepth, float InDepth, bool bClearStencil, int32_t InStencil)
{
	if (bClearColor)
	{
		assert(InColorsNum <= MaxSimultaneousRenderTargets);
		assert(RenderContext.BlendState.IsValidRef());

		for (uint32_t k = 0; k < InColorsNum; k++)
		{
			const FOpenGLBlendStateData::FRenderTagertBlendState &BlendState = RenderContext.BlendState->Data.RenderTargetBlendStates[k];
			bool bMaskDirty = false;

			if (!BlendState.bEnableWriteR
				|| !BlendState.bEnableWriteG
				|| !BlendState.bEnableWriteB
				|| !BlendState.bEnableWriteA
				)
			{
				glColorMaski(k, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				bMaskDirty = true;
			}

			glClearBufferfv(GL_COLOR, k, InColors[k].RGBA);
			if (bMaskDirty)
			{
				glColorMaski(k, BlendState.bEnableWriteR, BlendState.bEnableWriteG, BlendState.bEnableWriteB, BlendState.bEnableWriteA);
			}
		} // end for k
	}

	uint32_t ClearTypes = 0;
	bool bDirtyDepthMask = false, bDirtyStencilMask = false;

	if (bClearDepth)
	{
		assert(RenderContext.DepthStencilState.IsValidRef());

		ClearTypes |= CT_Depth;
		if (!RenderContext.DepthStencilState->Data.bEnableZWrite)
		{
			glDepthMask(GL_TRUE);
			bDirtyDepthMask = true;
		}
	}
	if (bClearStencil)
	{
		assert(RenderContext.DepthStencilState.IsValidRef());

		ClearTypes |= CT_Stencil;
		if (RenderContext.DepthStencilState->Data.StencilWriteMask != 0xFFFFFFFF)
		{
			glStencilMask(0xFFFFFFFF);
			bDirtyStencilMask = true;
		}
	}

	switch (ClearTypes)
	{
	case CT_Depth:
		glClearBufferfv(GL_DEPTH, 0, &InDepth);
		break;
	case CT_Stencil:
		glClearBufferiv(GL_STENCIL, 0, &InStencil);
		break;
	case CT_DepthStencil:
		glClearBufferfi(GL_DEPTH_STENCIL, 0, InDepth, InStencil);
		break;
	default:
		break;
	}

	// restore the states
	if (bDirtyDepthMask)
	{
		glDepthMask(RenderContext.DepthStencilState->Data.bEnableZWrite);
	}
	if (bDirtyStencilMask)
	{
		glStencilMask(RenderContext.DepthStencilState->Data.StencilWriteMask);
	}
}

// draw primitives
void FOpenGLRenderer::DrawIndexedPrimitive(const FRHIIndexBufferRef &InIndexBuffer, EPrimitiveType InMode, uint32_t InStart, uint32_t InCount)
{

}

void FOpenGLRenderer::DrawIndexedPrimitiveInstanced(const FRHIIndexBufferRef &InIndexBuffer, EPrimitiveType InMode, uint32_t InStart, uint32_t InCount, uint32_t InInstances)
{

}

void FOpenGLRenderer::DrawArrayedPrimitive(EPrimitiveType InMode, uint32_t InStart, uint32_t InCount)
{

}

void FOpenGLRenderer::DrawArrayedPrimitiveInstanced(EPrimitiveType InMode, uint32_t InStart, uint32_t InCount, uint32_t InInstances)
{

}
