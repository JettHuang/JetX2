// \brief
//		some constant definitions
//

#ifndef __JETX_RENDERER_DEFS_H__
#define __JETX_RENDERER_DEFS_H__

#include <vector>
#include "Foundation/JetX.h"


//////////////////////////////////////////////////////////////////////////
// states enum

// rasterizer cull mode
enum ERasterizerCullMode
{
	CM_CW,
	CM_CCW
};

// rasterizer fill mode
enum ERasterizerFillMode
{
	FM_Point,
	FM_Wireframe,
	FM_Solid
};

// sampler filter
enum ESamplerFilter
{
	SF_Point,
	SF_Bilinear,
	SF_Trilinear
};

// sampler address mode
enum ESamplerAddressMode
{
	AM_Wrap,
	AM_Clamp,
	AM_Mirror,
	AM_Border
};

// output merge stage
// compare function
enum ECompareFunction
{
	CF_Less,
	CF_LessEqual,
	CF_Greater,
	CF_GreaterEqual,
	CF_Equal,
	CF_NotEqual,
	CF_Never,
	CF_Always
};

// face type
enum EFaceMode
{
	FM_Front,
	FM_Back,
	FM_FrontAndBack
};

// Stencil Op
enum EStencilOp
{
	SO_Keep,
	SO_Zero,
	SO_Replace,
	SO_Increment,
	SO_Decrement,
	SO_IncrementWrap,
	SO_DecrementWrap,
	SO_Invert
};

// blend operation
enum EBlendOperation
{
	BO_Add,
	BO_Subtract,
	BO_Min,
	BO_Max,
	BO_ReverseSubstract
};

enum EBlendFactor
{
	BF_Zero,
	BF_One,
	BF_SourceColor,
	BF_InverseSourceColor,
	BF_SourceAlpha,
	BF_InverseSourceAlpha,
	BF_DestAlpha,
	BF_InverseDestAlpha,
	BF_DestColor,
	BF_InverseDestColor,
	BF_ConstantBlendFactor,
	BF_InverseConstantBlendFactor
};

// color write mask
enum EColorWriteMask
{
	CW_RED = 0x01,
	CW_GREEN = 0x02,
	CW_BLUE = 0x04,
	CW_ALPHA = 0x08,

	CW_NONE = 0,
	CW_RGB = CW_RED | CW_GREEN | CW_BLUE,
	CW_RGBA = CW_RED | CW_GREEN | CW_BLUE | CW_ALPHA,
	CW_RG = CW_RED | CW_GREEN,
	CW_BA = CW_BLUE | CW_ALPHA
};

// others
enum EVertexElementType
{
	VET_None,
	VET_Float1,
	VET_Float2,
	VET_Float3,
	VET_Float4,
	VET_PackedNormal,	// FPackedNormal
	VET_UByte4,
	VET_UByte4N,
	VET_Color,
	VET_Short2,
	VET_Short4,
	VET_Short2N,		// 16 bit word normalized to (value/32767.0,value/32767.0,0,0,1)
	VET_Half2,			// 16 bit float using 1 bit sign, 5 bit exponent, 10 bit mantissa 
	VET_Half4,
	VET_Short4N,		// 4 X 16 bit word, normalized 
	VET_UShort2,
	VET_UShort4,
	VET_UShort2N,		// 16 bit word normalized to (value/65535.0,value/65535.0,0,0,1)
	VET_UShort4N,		// 4 X 16 bit word unsigned, normalized 
	VET_URGB10A2N,		// 10 bit r, g, b and 2 bit a normalized to (value/1023.0f, value/1023.0f, value/1023.0f, value/3.0f)
	VET_Int1,
	VET_Int2,
	VET_Int3,
	VET_Int4,
	VET_MAX
};

// cube face
enum ECubeFace
{
	CubeFace_PosX = 0,
	CubeFace_NegX,
	CubeFace_PosY,
	CubeFace_NegY,
	CubeFace_PosZ,
	CubeFace_NegZ,
	CubeFace_MAX
};

enum EClearType
{
	CT_None = 0x0,
	CT_Depth = 0x1,
	CT_Stencil = 0x2,
	CT_Color = 0x4,
	CT_DepthStencil = CT_Depth | CT_Stencil,
};

// buffer access frequency
enum EBufferAccess
{
	BA_None = 0,
	BA_Static = 0x01, // modified rarely and used many times
	BA_Dynamic= 0x02, // modified repeatedly and used many times
	BA_Stream = 0x04  // modified rarely and used a few times 
};

// buffer usage
enum EBufferUsage
{
	BU_None = 0,
	BU_Draw = 0x08, // modified by application and used as source for drawing or image commands.
	BU_Read = 0x10, // modified by back-end renderer and return data to application.
	BU_Copy = 0x20  // modified by back-end renderer and as source for drawing or image commands. 
};

// buffer lock mode
enum EBufferLockMode
{
	BL_ReadOnly,
	BL_WriteOnly,
	BL_ReadWrite
};

// primitive type
enum EPrimitiveType
{
	PT_Points = 0,
	PT_Lines,
	PT_LineStrips,
	PT_LineLoops,
	PT_Triangles,
	PT_TriangleStrips,
	PT_TriangleFans,

	PT_Max
};

// linear color rgba
struct FLinearColor
{
	FLinearColor()
	{}

	FLinearColor(float r, float g, float b, float a)
		: R(r), G(g), B(b), A(a)
	{}

	bool operator !=(const FLinearColor &rhs)
	{
		return (R != rhs.R) || (G != rhs.G) || (B != rhs.B) || (A != rhs.A);
	}

	union 
	{
		struct  
		{
			float R, G, B, A;
		};

		float RGBA[4];
	};
};

struct FIntPoint
{
	int32_t		X;
	int32_t		Y;

	FIntPoint(int32_t InX, int32_t InY)
		: X(InX)
		, Y(InY)
	{}
};

struct FScreenResolution
{
	uint32_t	Width;
	uint32_t	Height;
	uint32_t	RefreshRate;

	bool operator <(const FScreenResolution &rhs) const
	{
		if (Width != rhs.Width)
		{
			return Width < rhs.Width;
		}
		else if (Height != rhs.Height)
		{
			return Height < rhs.Height;
		}
		else
		{
			return RefreshRate < rhs.RefreshRate;
		}
	}
};

typedef std::vector<FScreenResolution> FScreenResolutionArray;

// vertex element
struct FVertexElement
{
	uint16_t 		StreamIndex;
	uint16_t 		AttributeIndex;
	uint16_t 		Offset;
	uint16_t 		Stride;
	uint16_t		Divisor;		// none zero for instancing.
	EVertexElementType	DataType;

	FVertexElement() {}
	FVertexElement(uint16_t InStreamIndex, uint16_t InAttriIndex, uint16_t InOffset, uint16_t InStride, uint16_t InDivisor, EVertexElementType InType)
		: StreamIndex(InStreamIndex)
		, AttributeIndex(InAttriIndex)
		, Offset(InOffset)
		, Stride(InStride)
		, Divisor(InDivisor)
		, DataType(InType)
	{
	}
};

typedef std::vector<FVertexElement>		FVertexElementsList;

/** The number of render-targets that may be simultaneously written to. */
enum { MaxSimultaneousRenderTargets = 8 };

/** The number of texture units */
enum { MaxTextureUnits = 8 };

/** The number of vertex input stream slot */
enum { MaxVertexStreamSources = 16 };

/** The number of vertex attributes */
enum { MaxVertexAttributes = 16 };


#endif // __JETX_RENDERER_DEFS_H__
