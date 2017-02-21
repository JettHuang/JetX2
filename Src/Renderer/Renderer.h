//\brief
//		base renderer class & definition
//

#ifndef __JETX_RENDERER_H__
#define __JETX_RENDERER_H__

#include "Foundation/JetX.h"
#include "Foundation/RefCounting.h"
#include "Foundation/OutputDevice.h"
#include "RendererDefs.h"
#include "RendererState.h"
#include "RHIResource.h"
#include "glm/glm.hpp"


enum ERendererType
{
	RT_None,
	RT_OpenGL,
	RT_Max
};

// FRenderer
class FRenderer
{
public:
	static FRenderer* CreateRender(ERendererType InRenderType);

	FRenderer() {}
	virtual ~FRenderer() {}

	//Init
	virtual void Init(FOutputDevice *LogOutputDevice) {}
	virtual void Shutdown() {}

	//Capabilities
	virtual void DumpCapabilities() = 0;

//render viewport
	virtual FRHIViewportRef RHICreateViewport(void* InWindowHandle, uint32_t SizeX, uint32_t SizeY, bool bIsFullscreen) = 0;
	virtual void RHIResizeViewport(FRHIViewportRef InViewport, uint32_t SizeX, uint32_t SizeY, bool bIsFullscreen) = 0;

	virtual bool RHIGetAvailableResolutions(FScreenResolutionArray& Resolutions, bool bIgnoreRefreshRate) = 0;
	virtual void RHIGetSupportedResolution(uint32_t& Width, uint32_t& Height) = 0;

//Resource Creating
	// states
	virtual FRHISamplerStateRef RHICreateSamplerState(const FSamplerStateInitializerRHI &SamplerStateInitializer) = 0;
	virtual FRHIRasterizerStateRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI &RasterizerStateInitializer) = 0;
	virtual FRHIDepthStencilStateRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI &DepthStencilStateInitializer) = 0;
	virtual FRHIBlendStateRef RHICreateBlendState(const FBlendStateInitializerRHI &BlendStateInitializer) = 0;

	// data buffers
	virtual FRHIVertexBufferRef RHICreateVertexBuffer(uint32_t InBytes, const void *InData, EBufferAccess InAccess, EBufferUsage InUsage) = 0;
	virtual FRHIIndexBufferRef RHICreateIndexBuffer(uint32_t InBytes, const void *InData, uint16_t InStride, EBufferAccess InAccess, EBufferUsage InUsage) = 0;
	
	virtual void FillDataBuffer(FRHIDataBufferRef InBuffer, uint32_t InOffset, uint32_t InBytes, const void *InData) = 0;
	virtual void* LockDataBuffer(FRHIDataBufferRef InBuffer, uint32_t InOffset, uint32_t InBytes, EBufferLockMode InMode) = 0;
	virtual void UnLockDataBuffer(FRHIDataBufferRef InBuffer) = 0;

	// textures
	//virtual FRHITexture2DRef RHICreateTexture2D() = 0;

//State Setting
	virtual void RHISetSamplerState(uint32_t InTexIndex, const FRHISamplerStateRef &InSamplerState) = 0;
	virtual void RHISetRasterizerState(const FRHIRasterizerStateRef &InRasterizerState) = 0;
	virtual void RHISetDepthStencilState(const FRHIDepthStencilStateRef &InDepthStencilState, int32_t InStencilRef) = 0;
	virtual void RHISetBlendState(const FRHIBlendStateRef &InBlendState, const FLinearColor &InBlendColor) = 0;

	virtual void RHISetViewport(int32_t InX, int32_t InY, int32_t InWidth, int32_t InHeight, float InMinZ, float InMaxZ) = 0;
	virtual void RHISetScissorRect(int32_t InX, int32_t InY, int32_t InWidth, int32_t InHeight) = 0;

//Draw Commands
	virtual void RHIBeginDrawingViewport(FRHIViewportRef Viewport) = 0;
	virtual void RHIEndDrawingViewport(FRHIViewportRef Viewport, bool bPresent, bool bLockToVsync) = 0;
	virtual void RHIBeginFrame() = 0;
	virtual void RHIEndFrame() = 0;

	virtual void RHIClear(bool bClearColor, const FLinearColor &InColor, bool bClearDepth, float InDepth, bool bClearStencil, int32_t InStencil) = 0;
	// clear multi-render-targets
	virtual void RHIClearMRT(bool bClearColor, const FLinearColor *InColors, uint32_t InColorsNum, bool bClearDepth, float InDepth, bool bClearStencil, int32_t InStencil) = 0;

	// draw primitives
	virtual void DrawIndexedPrimitive(const FRHIIndexBufferRef &InIndexBuffer, EPrimitiveType InMode, uint32_t InStart, uint32_t InCount) = 0;
	virtual void DrawIndexedPrimitiveInstanced(const FRHIIndexBufferRef &InIndexBuffer, EPrimitiveType InMode, uint32_t InStart, uint32_t InCount, uint32_t InInstances) = 0;
	virtual void DrawArrayedPrimitive(EPrimitiveType InMode, uint32_t InStart, uint32_t InCount) = 0;
	virtual void DrawArrayedPrimitiveInstanced(EPrimitiveType InMode, uint32_t InStart, uint32_t InCount, uint32_t InInstances) = 0;
};


#endif //__JETX_RENDERER_H__
