// \brief
//		renderer context states
//

#ifndef __JETX_RENDERER_STATES_H__
#define __JETX_RENDERER_STATES_H__

#include "Foundation/JetX.h"
#include "RendererDefs.h"


// sampler state initializer
struct FSamplerStateInitializerRHI
{
	FSamplerStateInitializerRHI() {}
	FSamplerStateInitializerRHI(
		ESamplerFilter	InFilter,
		ESamplerAddressMode InAddressS = AM_Wrap,
		ESamplerAddressMode InAddressT = AM_Wrap,
		ESamplerAddressMode InAddressR = AM_Wrap,
		float InMipBias = 0.f,
		float InMinMipLevel = -1000.f,
		float InMaxMipLevel = 1000.f,
		float InBorderR = 0.f, float InBorderG = 0.f, float InBorderB = 0.f, float InBorderA = 0.f
		)
		: Filter(InFilter)
		, AddressS(InAddressS)
		, AddressT(InAddressT)
		, AddressR(InAddressR)
		, MinMipLevel(InMinMipLevel)
		, MaxMipLevel(InMaxMipLevel)
		, MipBias(InMipBias)
	{
		BorderColor[0] = InBorderR;
		BorderColor[1] = InBorderG;
		BorderColor[2] = InBorderB;
		BorderColor[3] = InBorderA;
	}

	bool operator <(const FSamplerStateInitializerRHI &rhs) const
	{
		return (Filter < rhs.Filter)
			|| (AddressS < rhs.AddressS)
			|| (AddressT < rhs.AddressT)
			|| (AddressR < rhs.AddressR)
			|| (MinMipLevel < rhs.MinMipLevel)
			|| (MaxMipLevel < rhs.MaxMipLevel);
	}

	ESamplerFilter		Filter;
	ESamplerAddressMode	AddressS;
	ESamplerAddressMode AddressT;
	ESamplerAddressMode AddressR;

	float				MinMipLevel;
	float				MaxMipLevel;
	float				MipBias;
	float				BorderColor[4];
};

// Rasterizer state initializer
struct FRasterizerStateInitializerRHI
{
	FRasterizerStateInitializerRHI() {}
	FRasterizerStateInitializerRHI(
		ERasterizerFillMode InFillMode,
		ERasterizerCullMode InCullMode = CM_CW,
		float InDepthFactor = 0.f,
		float InDepthUnits = 0.f,
		bool InbAllowMSAA = false,
		bool InbEnableLineAA = false
		)
		: FillMode(InFillMode)
		, CullMode(InCullMode)
		, DepthOffsetFactor(InDepthFactor)
		, DepthOffsetUnits(InDepthUnits)
		, bAllowMSAA(InbAllowMSAA)
		, bEnableLineAA(InbEnableLineAA)
	{
	}

	bool operator <(const FRasterizerStateInitializerRHI &rhs) const
	{
		return (FillMode < rhs.FillMode)
			|| (CullMode < rhs.CullMode)
			|| (DepthOffsetUnits < rhs.DepthOffsetUnits)
			|| (DepthOffsetFactor < rhs.DepthOffsetFactor);
	}

	ERasterizerFillMode		FillMode;
	ERasterizerCullMode		CullMode;
	float					DepthOffsetFactor;
	float					DepthOffsetUnits;
	bool					bAllowMSAA;
	bool					bEnableLineAA;
};

// depth-stencil state initializer
struct FDepthStencilStateInitializerRHI
{
	FDepthStencilStateInitializerRHI() {}
	FDepthStencilStateInitializerRHI(
		bool InbEnableDepthWrite,
		ECompareFunction InDepthTestFunc = CF_Less,
		bool InbEnableStencilTest = false,
		ECompareFunction InFrontFaceStencilTest = CF_Always,
		EStencilOp InFrontFaceStencilFailOp = SO_Keep,
		EStencilOp InFrontFaceDepthFailOp = SO_Keep,
		EStencilOp InFrontFaceDepthPassOp = SO_Keep,
		ECompareFunction InBackFaceStencilTest = CF_Always,
		EStencilOp InBackFaceStencilFailOp = SO_Keep,
		EStencilOp InBackFaceDepthFailOp = SO_Keep,
		EStencilOp InBackFaceDepthPassOp = SO_Keep,
		uint32_t InStencilReadMask = 0xFFFFFFFF,
		uint32_t InStencilWriteMask= 0xFFFFFFFF
		)
		: bEnableDepthWrite(InbEnableDepthWrite)
		, DepthTestFunc(InDepthTestFunc)
		, bEnableStencilTest(InbEnableStencilTest)
		, FrontFaceStencilTest(InFrontFaceStencilTest)
		, FrontFaceStencilFailOp(InFrontFaceStencilFailOp)
		, FrontFaceDepthFailOp(InFrontFaceDepthFailOp)
		, FrontFaceDepthPassOp(InFrontFaceDepthPassOp)
		, BackFaceStencilTest(InBackFaceStencilTest)
		, BackFaceStencilFailOp(InBackFaceStencilFailOp)
		, BackFaceDepthFailOp(InBackFaceDepthFailOp)
		, BackFaceDepthPassOp(InBackFaceDepthPassOp)
		, StencilReadMask(InStencilReadMask)
		, StencilWriteMask(InStencilWriteMask)
	{}

	bool operator <(const FDepthStencilStateInitializerRHI &rhs) const
	{
		return (DepthTestFunc < rhs.DepthTestFunc)
			|| (FrontFaceStencilTest < rhs.FrontFaceStencilTest)
			|| (BackFaceStencilTest < rhs.BackFaceStencilTest);
	}

	//depth
	bool	bEnableDepthWrite;
	ECompareFunction	DepthTestFunc;
	
	//stencil
	bool	bEnableStencilTest;
	ECompareFunction	FrontFaceStencilTest;
	EStencilOp			FrontFaceStencilFailOp;	// operation when stencil test fail.
	EStencilOp			FrontFaceDepthFailOp;	// operation when stencil test pass but depth test fail.
	EStencilOp			FrontFaceDepthPassOp;	// operation when stencil test pass but depth test pass.
	ECompareFunction	BackFaceStencilTest;
	EStencilOp			BackFaceStencilFailOp;
	EStencilOp			BackFaceDepthFailOp;
	EStencilOp			BackFaceDepthPassOp;

	uint32_t			StencilReadMask;
	uint32_t			StencilWriteMask;
};

// blend state initializer
struct FBlendStateInitializerRHI
{
	struct FRenderTargetBlendState
	{
		EBlendOperation		ColorBlendOp;
		EBlendFactor		ColorSrcFactor;
		EBlendFactor		ColorDstFactor;
		EBlendOperation		AlphaBlendOp;
		EBlendFactor		AlphaSrcFactor;
		EBlendFactor		AlphaDstFactor;
		EColorWriteMask		ColorWriteMask;

		FRenderTargetBlendState()
			: ColorBlendOp(BO_Add)
			, ColorSrcFactor(BF_One)
			, ColorDstFactor(BF_Zero)
			, AlphaBlendOp(BO_Add)
			, AlphaSrcFactor(BF_One)
			, AlphaDstFactor(BF_Zero)
			, ColorWriteMask(CW_RGBA)
		{}
	};

	FBlendStateInitializerRHI() {}
	FBlendStateInitializerRHI(const FRenderTargetBlendState& InBlendState)
		: TargetsNum(1)
	{
		RenderTargetBlendStates[0] = InBlendState;
	}

	FBlendStateInitializerRHI(const FRenderTargetBlendState* InBlendStates, uint32_t InNum)
		: TargetsNum(InNum)
	{
		for (uint32_t k = 0; k < InNum; k++)
		{
			RenderTargetBlendStates[k] = InBlendStates[k];
		}
	}

	bool operator <(const FBlendStateInitializerRHI &rhs) const
	{
		return (RenderTargetBlendStates[0].ColorBlendOp < rhs.RenderTargetBlendStates[0].ColorBlendOp)
			|| (RenderTargetBlendStates[0].ColorSrcFactor < rhs.RenderTargetBlendStates[0].ColorSrcFactor)
			|| (RenderTargetBlendStates[0].ColorDstFactor < rhs.RenderTargetBlendStates[0].ColorDstFactor)
			|| (RenderTargetBlendStates[0].AlphaBlendOp < rhs.RenderTargetBlendStates[0].AlphaBlendOp)
			|| (RenderTargetBlendStates[0].AlphaSrcFactor < rhs.RenderTargetBlendStates[0].AlphaSrcFactor)
			|| (RenderTargetBlendStates[0].AlphaDstFactor < rhs.RenderTargetBlendStates[0].AlphaDstFactor)
			|| TargetsNum < rhs.TargetsNum;
	}

	FRenderTargetBlendState RenderTargetBlendStates[MaxSimultaneousRenderTargets];
	uint32_t TargetsNum;
};

#endif // __JETX_RENDERER_STATES_H__
