// \brief
//		methods for GL-state creating
//

#include "OpenGLState.h"
#include "OpenGLRenderer.h"


static GLenum TranslateAddressMode(ESamplerAddressMode AddressMode)
{
	switch (AddressMode)
	{
	case AM_Clamp:
		return GL_CLAMP_TO_EDGE;
	case AM_Mirror:
		return GL_MIRRORED_REPEAT;
	case AM_Border:
		return GL_CLAMP_TO_BORDER;
	default:
		return GL_REPEAT;
	}
}

static GLenum TranslateFillMode(ERasterizerFillMode InFillMode)
{
	switch (InFillMode)
	{
	case FM_Point:
		return GL_POINT;
	case FM_Wireframe:
		return GL_LINE;
	case FM_Solid:
	default:
		return GL_FILL;
	}
}

static GLenum TranslateCullMode(ERasterizerCullMode InCullMode)
{
	switch (InCullMode)
	{
	case CM_CW:
		return GL_BACK;
	case CM_CCW:
		return GL_FRONT;
	default:
		return GL_NONE;
	}
}

static GLenum TranslateCompareFunction(ECompareFunction InCompFunc)
{
	switch (InCompFunc)
	{
	case CF_Less:
		return GL_LESS;
	case CF_LessEqual:
		return GL_LEQUAL;
	case CF_Greater:
		return GL_GREATER;
	case CF_GreaterEqual:
		return GL_GEQUAL;
	case CF_Equal:
		return GL_EQUAL;
	case CF_NotEqual:
		return GL_NOTEQUAL;
	case CF_Never:
		return GL_NEVER;
	case CF_Always:
	default:
		return GL_ALWAYS;
	}
}

static GLenum TranslateStencilOp(EStencilOp InStencilOp)
{
	switch (InStencilOp)
	{
	case SO_Keep:
		return GL_KEEP;
	case SO_Zero:
		return GL_ZERO;
	case SO_Replace:
		return GL_REPLACE;
	case SO_Increment:
		return GL_INCR;
	case SO_Decrement:
		return GL_DECR;
	case SO_IncrementWrap:
		return GL_INCR_WRAP;
	case SO_DecrementWrap:
		return GL_DECR_WRAP;
	case SO_Invert:
		return GL_INVERT;
	default:
		return GL_KEEP;
	}
}

static GLenum TranslateBlendOp(EBlendOperation InBlendOp)
{
	switch (InBlendOp)
	{
	case BO_Add:
		return GL_FUNC_ADD;
	case BO_Subtract:
		return GL_FUNC_SUBTRACT;
	case BO_Min:
		return GL_MIN;
	case BO_Max:
		return GL_MAX;
	case BO_ReverseSubstract:
		return GL_FUNC_REVERSE_SUBTRACT;
	default:
		return GL_FUNC_ADD;
	}
}

static GLenum TranslateBlendFactor(EBlendFactor InBlendFactor)
{
	switch (InBlendFactor)
	{
	case BF_Zero:
		return GL_ZERO;
	case BF_One:
		return GL_ONE;
	case BF_SourceColor:
		return GL_SRC_COLOR;
	case BF_InverseSourceColor:
		return GL_ONE_MINUS_SRC_COLOR;
	case BF_SourceAlpha:
		return GL_SRC_ALPHA;
	case BF_InverseSourceAlpha:
		return GL_ONE_MINUS_SRC_ALPHA;
	case BF_DestAlpha:
		return GL_DST_ALPHA;
	case BF_InverseDestAlpha:
		return GL_ONE_MINUS_DST_ALPHA;
	case BF_DestColor:
		return GL_DST_COLOR;
	case BF_InverseDestColor:
		return GL_ONE_MINUS_DST_COLOR;
	case BF_ConstantBlendFactor:
		return GL_CONSTANT_COLOR;
	case BF_InverseConstantBlendFactor:
		return GL_ONE_MINUS_CONSTANT_COLOR;
	default:
		return GL_ZERO;
	}
}

FRHIOpenGLSamplerState::~FRHIOpenGLSamplerState()
{
	glDeleteSamplers(1, &Resource);
	Resource = 0;
}

bool FRHIOpenGLSamplerState::Initialize()
{
	glGenSamplers(1, &Resource);
	assert(Resource);
	glSamplerParameteri(Resource, GL_TEXTURE_WRAP_S, Data.Address_s);
	glSamplerParameteri(Resource, GL_TEXTURE_WRAP_T, Data.Address_t);
	glSamplerParameteri(Resource, GL_TEXTURE_WRAP_R, Data.Address_r);
	glSamplerParameteri(Resource, GL_TEXTURE_MIN_FILTER, Data.MinFilter);
	glSamplerParameteri(Resource, GL_TEXTURE_MAG_FILTER, Data.MagFilter);
	glSamplerParameterf(Resource, GL_TEXTURE_MIN_LOD, Data.MinMipLevel);
	glSamplerParameterf(Resource, GL_TEXTURE_MAX_LOD, Data.MaxMipLevel);
	glSamplerParameterf(Resource, GL_TEXTURE_LOD_BIAS, Data.MipBias);
	glSamplerParameterfv(Resource, GL_TEXTURE_BORDER_COLOR, Data.BorderColor);

	return true;
}

//State Resource Creating
FRHISamplerStateRef FOpenGLRenderer::RHICreateSamplerState(const FSamplerStateInitializerRHI &SamplerStateInitializer)
{
	std::map<FSamplerStateInitializerRHI, FRHISamplerStateRef>::iterator Target = SamplerStateCache.find(SamplerStateInitializer);
	if (Target != SamplerStateCache.end())
	{
		return Target->second;
	}

	FRHIOpenGLSamplerState *SamplerState = new FRHIOpenGLSamplerState(this);
	if (SamplerState == nullptr)
	{
		return FRHISamplerStateRef();
	}

	SamplerState->Data.Address_s = TranslateAddressMode(SamplerStateInitializer.AddressS);
	SamplerState->Data.Address_t = TranslateAddressMode(SamplerStateInitializer.AddressT);
	SamplerState->Data.Address_r = TranslateAddressMode(SamplerStateInitializer.AddressR);
	SamplerState->Data.MinMipLevel = SamplerStateInitializer.MinMipLevel;
	SamplerState->Data.MaxMipLevel = SamplerStateInitializer.MaxMipLevel;
	SamplerState->Data.MipBias = SamplerStateInitializer.MipBias;
	SamplerState->Data.BorderColor[0] = SamplerStateInitializer.BorderColor[0];
	SamplerState->Data.BorderColor[1] = SamplerStateInitializer.BorderColor[1];
	SamplerState->Data.BorderColor[2] = SamplerStateInitializer.BorderColor[2];
	SamplerState->Data.BorderColor[3] = SamplerStateInitializer.BorderColor[3];
	switch (SamplerStateInitializer.Filter)
	{
	case SF_Bilinear:
		SamplerState->Data.MagFilter = GL_LINEAR;
		SamplerState->Data.MinFilter = GL_LINEAR_MIPMAP_NEAREST;
		break;
	case SF_Trilinear:
		SamplerState->Data.MagFilter = GL_LINEAR;
		SamplerState->Data.MinFilter = GL_LINEAR_MIPMAP_LINEAR;
	case SF_Point:
	default:
		SamplerState->Data.MagFilter = GL_NEAREST;
		SamplerState->Data.MinFilter = GL_NEAREST_MIPMAP_NEAREST;
		break;
	}

	// add to cache
	SamplerStateCache.insert(std::make_pair(SamplerStateInitializer, SamplerState));
	return SamplerState;
}

FRHIRasterizerStateRef FOpenGLRenderer::RHICreateRasterizerState(const FRasterizerStateInitializerRHI &RasterizerStateInitializer)
{
	std::map<FRasterizerStateInitializerRHI, FRHIRasterizerStateRef>::iterator Target = RasterizerStateCache.find(RasterizerStateInitializer);
	if (Target != RasterizerStateCache.end())
	{
		return Target->second;
	}

	FRHIOpenGLRasterizerState *RasterizerState = new FRHIOpenGLRasterizerState(this);
	if (RasterizerState == nullptr)
	{
		return FRHIRasterizerStateRef();
	}

	RasterizerState->Data.FillMode = TranslateFillMode(RasterizerStateInitializer.FillMode);
	RasterizerState->Data.CullMode = TranslateCullMode(RasterizerStateInitializer.CullMode);
	RasterizerState->Data.bAllowMSAA = RasterizerStateInitializer.bAllowMSAA;
	RasterizerState->Data.bEnableLineAA = RasterizerStateInitializer.bEnableLineAA;
	RasterizerState->Data.DepthOffsetFactor = RasterizerStateInitializer.DepthOffsetFactor;
	RasterizerState->Data.DepthOffsetUnits = RasterizerStateInitializer.DepthOffsetUnits;
	RasterizerState->Data.bEnableDepthOffset = (RasterizerStateInitializer.DepthOffsetFactor != 0.f && RasterizerStateInitializer.DepthOffsetUnits != 0.f);

	// add to cache
	RasterizerStateCache.insert(std::make_pair(RasterizerStateInitializer, RasterizerState));
	return RasterizerState;
}

FRHIDepthStencilStateRef FOpenGLRenderer::RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI &DepthStencilStateInitializer)
{
	std::map<FDepthStencilStateInitializerRHI, FRHIDepthStencilStateRef>::iterator Target = DepthStencilStateCache.find(DepthStencilStateInitializer);
	if (Target != DepthStencilStateCache.end())
	{
		return Target->second;
	}

	FRHIOpenGLDepthStencilState *DepthStencilState = new FRHIOpenGLDepthStencilState(this);
	if (DepthStencilState == nullptr)
	{
		return FRHIDepthStencilStateRef();
	}

	DepthStencilState->Data.bEnableZTest = (DepthStencilStateInitializer.DepthTestFunc != CF_Always) || DepthStencilStateInitializer.bEnableDepthWrite;
	DepthStencilState->Data.bEnableZWrite = DepthStencilStateInitializer.bEnableDepthWrite;
	DepthStencilState->Data.ZFunc = TranslateCompareFunction(DepthStencilStateInitializer.DepthTestFunc);

	DepthStencilState->Data.bEnableStencilTest = DepthStencilStateInitializer.bEnableStencilTest;
	DepthStencilState->Data.FrontFaceStencilCompFunc = TranslateCompareFunction(DepthStencilStateInitializer.FrontFaceStencilTest);
	DepthStencilState->Data.FrontFaceStencilFailOp = TranslateStencilOp(DepthStencilStateInitializer.FrontFaceStencilFailOp);
	DepthStencilState->Data.FrontFaceDepthFailOp = TranslateStencilOp(DepthStencilStateInitializer.FrontFaceDepthFailOp);
	DepthStencilState->Data.FrontFaceDepthPassOp = TranslateStencilOp(DepthStencilStateInitializer.FrontFaceDepthPassOp);
	
	DepthStencilState->Data.BackFaceStencilCompFunc = TranslateCompareFunction(DepthStencilStateInitializer.BackFaceStencilTest);
	DepthStencilState->Data.BackFaceStencilFailOp = TranslateStencilOp(DepthStencilStateInitializer.BackFaceStencilFailOp);
	DepthStencilState->Data.BackFaceDepthFailOp = TranslateStencilOp(DepthStencilStateInitializer.BackFaceDepthFailOp);
	DepthStencilState->Data.BackFaceDepthPassOp = TranslateStencilOp(DepthStencilStateInitializer.BackFaceDepthPassOp);

	DepthStencilState->Data.StencilReadMask = DepthStencilStateInitializer.StencilReadMask;
	DepthStencilState->Data.StencilWriteMask = DepthStencilStateInitializer.StencilWriteMask;

	// add to cache
	DepthStencilStateCache.insert(std::make_pair(DepthStencilStateInitializer, DepthStencilState));
	return DepthStencilState;
}

FRHIBlendStateRef FOpenGLRenderer::RHICreateBlendState(const FBlendStateInitializerRHI &BlendStateInitializer)
{
	std::map<FBlendStateInitializerRHI, FRHIBlendStateRef>::iterator Target = BlendStateCache.find(BlendStateInitializer);
	if (Target != BlendStateCache.end())
	{
		return Target->second;
	}

	FRHIOpenGLBlendState *BlendState = new FRHIOpenGLBlendState(this);
	if (BlendState == nullptr)
	{
		return FRHIBlendStateRef();
	}

	const uint32_t kMaxRenderTargets = (std::min)(BlendStateInitializer.TargetsNum, (uint32_t)MaxSimultaneousRenderTargets);
	BlendState->TargetsNum = kMaxRenderTargets;
	for (uint32_t k = 0; k < kMaxRenderTargets; k++)
	{
		const FBlendStateInitializerRHI::FRenderTargetBlendState &SrcBlendState = BlendStateInitializer.RenderTargetBlendStates[k];
		FOpenGLBlendStateData::FRenderTagertBlendState &DstBlendState = BlendState->Data.RenderTargetBlendStates[k];

		DstBlendState.bEnableAlphaBlend = SrcBlendState.ColorBlendOp != BO_Add || SrcBlendState.ColorSrcFactor != BF_One || SrcBlendState.ColorDstFactor != BF_Zero
										|| SrcBlendState.AlphaBlendOp != BO_Add || SrcBlendState.AlphaSrcFactor != BF_One || SrcBlendState.AlphaDstFactor != BF_Zero;
		DstBlendState.ColorBlendOp = TranslateBlendOp(SrcBlendState.ColorBlendOp);
		DstBlendState.ColorSrcFactor = TranslateBlendFactor(SrcBlendState.ColorSrcFactor);
		DstBlendState.ColorDstFactor = TranslateBlendFactor(SrcBlendState.ColorDstFactor);
		DstBlendState.AlphaBlendOp = TranslateBlendOp(SrcBlendState.AlphaBlendOp);
		DstBlendState.AlphaSrcFactor = TranslateBlendFactor(SrcBlendState.AlphaSrcFactor);
		DstBlendState.AlphaDstFactor = TranslateBlendFactor(SrcBlendState.AlphaDstFactor);
		DstBlendState.bEnableWriteR = (SrcBlendState.ColorWriteMask & CW_RED) != 0;
		DstBlendState.bEnableWriteG = (SrcBlendState.ColorWriteMask & CW_GREEN) != 0;
		DstBlendState.bEnableWriteB = (SrcBlendState.ColorWriteMask & CW_BLUE) != 0;
		DstBlendState.bEnableWriteA = (SrcBlendState.ColorWriteMask & CW_ALPHA) != 0;
	} // end for k

	// add to cache
	BlendStateCache.insert(std::make_pair(BlendStateInitializer, BlendState));
	return BlendState;
}

