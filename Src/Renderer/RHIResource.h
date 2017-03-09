// \brief
//		RHI resource base class.
//

#ifndef __JETX_RHI_RESOURCE_H__
#define	__JETX_RHI_RESOURCE_H__

#include <string>
#include "glm/glm.hpp"
#include "Foundation/JetX.h"
#include "Foundation/RefCounting.h"

using namespace glm;


// enumeration of the different RHI reference types.
enum ERHIResourceType
{
	RRT_None,

	RRT_SamplerState,
	RRT_RasterizerState,
	RRT_DepthStencilState,
	RRT_BlendState,
	
	RRT_VertexDeclaration,  // input assemble layout
	RRT_VertexShader,
	RRT_PixelShader,
	RRT_GpuProgram,

	RRT_UniformBuffer,
	RRT_IndexBuffer,
	RRT_VertexBuffer,

	RRT_Texture2D,
	RRT_Texture3D,
	RRT_TextureCube,

	RRT_FrameBuffer,
	RRT_RenderBuffer,

	RRT_OcclusionQuery,
	RRT_Viewport,

	RRT_Max
};

// FRHIResource
class FRHIResource : public FRefCountedObject
{
public:
	virtual ERHIResourceType Type() = 0;
};

// state blocks
class FRHISamplerState : public FRHIResource 
{
public:
	ERHIResourceType Type() override { return RRT_SamplerState; }
};

class FRHIRasterizerState : public FRHIResource 
{
public:
	ERHIResourceType Type() override { return RRT_RasterizerState; }
};

class FRHIDepthStencilState : public FRHIResource 
{
public:
	ERHIResourceType Type() override { return RRT_DepthStencilState; }
};

class FRHIBlendState : public FRHIResource 
{
public:
	ERHIResourceType Type() override { return RRT_BlendState; }
};

// shader & program
class FRHIShader : public FRHIResource
{
public:
	virtual void Dump(class FOutputDevice &OutDevice) {}
	
	void SetName(const std::string &InName)
	{
		ShaderName = InName;
	}
	
	std::string ShaderName;
};

class FRHIVertexShader : public FRHIShader
{
public:
	ERHIResourceType Type() override { return RRT_VertexShader; }
};

class FRHIPixelShader : public FRHIShader
{
public:
	ERHIResourceType Type() override { return RRT_PixelShader; }
};

class FRHIGPUProgram : public FRHIResource
{
public:
	virtual void Dump(class FOutputDevice &OutDevice) {}

	ERHIResourceType Type() override { return RRT_GpuProgram; }

	// get uniform parameter handle
	virtual int32_t GetUniformHandle(const std::string &InName) = 0;

	virtual bool SetUniform1iv(int32_t InHandle, const int32_t *V, uint32_t InCount) = 0;
	virtual bool SetUniform2iv(int32_t InHandle, const int32_t *V, uint32_t InCount) = 0;
	virtual bool SetUniform3iv(int32_t InHandle, const int32_t *V, uint32_t InCount) = 0;
	virtual bool SetUniform4iv(int32_t InHandle, const int32_t *V, uint32_t InCount) = 0;

	virtual bool SetUniform1uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount) = 0;
	virtual bool SetUniform2uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount) = 0;
	virtual bool SetUniform3uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount) = 0;
	virtual bool SetUniform4uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount) = 0;

	virtual bool SetUniform1fv(int32_t InHandle, const float *V, uint32_t InCount) = 0;
	virtual bool SetUniform2fv(int32_t InHandle, const float *V, uint32_t InCount) = 0;
	virtual bool SetUniform3fv(int32_t InHandle, const float *V, uint32_t InCount) = 0;
	virtual bool SetUniform4fv(int32_t InHandle, const float *V, uint32_t InCount) = 0;

	virtual bool SetUniformMatrix4fv(int32_t InHandle, const float *V, uint32_t InCount) = 0;
};

// program input & params
class FRHIVertexDeclaration : public FRHIResource
{
public:
	ERHIResourceType Type() override { return RRT_VertexDeclaration; }
};

class FRHIUniformBuffer : public FRHIResource
{
public:
	ERHIResourceType Type() override { return RRT_UniformBuffer; }
};

// texture resource
class FRHITexture : public FRHIResource
{
public:

	// dynamic cast methods
	virtual class FRHITexture2D* GetTexture2D() { return nullptr; }
	virtual class FRHITexture3D* GetTexture3D() { return nullptr; }
	virtual class FRHITextureCube* GetTextureCube() { return nullptr; }
};

class FRHITexture2D : public FRHITexture
{
public:
	ERHIResourceType Type() override { return RRT_Texture2D; }
	class FRHITexture2D* GetTexture2D() override { return this; }
};

class FRHITexture3D : public FRHITexture
{
public:
	ERHIResourceType Type() override { return RRT_Texture3D; }
	class FRHITexture3D* GetTexture3D() override { return this; }
};

class FRHITextureCube : public FRHITexture
{
public:
	ERHIResourceType Type() override { return RRT_TextureCube; }
	class FRHITextureCube* GetTextureCube() override { return this; }
};

class FRHIDataBuffer : public FRHIResource
{
};

// vertex buffer resource
class FRHIVertexBuffer : public FRHIDataBuffer
{
public:
	ERHIResourceType Type() override { return RRT_VertexBuffer; }
};

class FRHIIndexBuffer : public FRHIDataBuffer
{
public:
	ERHIResourceType Type() override { return RRT_IndexBuffer; }
};

// frame buffer & render object
class FRHIFrameBuffer : public FRHIResource
{
public:
	ERHIResourceType Type() override { return RRT_FrameBuffer; }
};

class FRHIRenderBuffer : public FRHIResource
{
public:
	ERHIResourceType Type() override { return RRT_RenderBuffer; }
};

// others
class FRHIOcclusionQuery : public FRHIResource
{
public:
	ERHIResourceType Type() override { return RRT_OcclusionQuery; }
};

class FRHIViewport : public FRHIResource
{
public:
	ERHIResourceType Type() override { return RRT_Viewport; }
};

typedef TRefCountPtr<FRHISamplerState>	FRHISamplerStateRef;
typedef TRefCountPtr<FRHIRasterizerState> FRHIRasterizerStateRef;
typedef TRefCountPtr<FRHIDepthStencilState> FRHIDepthStencilStateRef;
typedef TRefCountPtr<FRHIBlendState> FRHIBlendStateRef;

typedef TRefCountPtr<FRHIShader>	FRHIShaderRef;
typedef TRefCountPtr<FRHIVertexShader> FRHIVertexShaderRef;
typedef TRefCountPtr<FRHIPixelShader> FRHIPixelShaderRef;
typedef TRefCountPtr<FRHIGPUProgram> FRHIGPUProgramRef;

typedef TRefCountPtr<FRHIVertexDeclaration> FRHIVertexDeclarationRef;
typedef TRefCountPtr<FRHIUniformBuffer> FRHIUniformBufferRef;

typedef TRefCountPtr<FRHITexture> FRHITextureRef;
typedef TRefCountPtr<FRHITexture2D> FRHITexture2DRef;
typedef TRefCountPtr<FRHITexture3D> FRHITexture3DRef;
typedef TRefCountPtr<FRHITextureCube> FRHITextureCubeRef;

typedef TRefCountPtr<FRHIDataBuffer> FRHIDataBufferRef;
typedef TRefCountPtr<FRHIVertexBuffer> FRHIVertexBufferRef;
typedef TRefCountPtr<FRHIIndexBuffer> FRHIIndexBufferRef;
typedef TRefCountPtr<FRHIFrameBuffer> FRHIFrameBufferRef;
typedef TRefCountPtr<FRHIRenderBuffer> FRHIRenderBufferRef;

typedef TRefCountPtr<FRHIOcclusionQuery> FRHIOcclusionQueryRef;
typedef TRefCountPtr<FRHIViewport> FRHIViewportRef;


#endif // __JETX_RHI_RESOURCE_H__
