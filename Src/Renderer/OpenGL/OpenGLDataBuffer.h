//\brief
//		OpenGL Resource: Data Buffer
//

#ifndef __JETX_OPENGL_DATA_BUFFER_H__
#define __JETX_OPENGL_DATA_BUFFER_H__

#include "Renderer/RendererDefs.h"
#include "Renderer/RHIResource.h"
#include "PlatformOpenGL.h"


// OpenGL Buffer Bind Targets
enum EBufferBindTarget
{
	Array_Buffer = 0,
	CopyRead_Buffer,
	CopyWrite_Buffer,
	DrawIndirect_Buffer,
	ElementArray_Buffer,
	PixelPack_Buffer,
	PixelUnpack_Buffer,
	Texture_Buffer,
	TransformFeedback_Buffer,
	Uniform_Buffer,

	MaxBufferBinds
};

class FOpenGLBuffer
{
public:
	FOpenGLBuffer(class FOpenGLRenderer	*InRenderer, EBufferBindTarget InType)
		: Renderer(InRenderer)
		, Type(InType)
		, Resource(0)
		, Bytes(0)
		, Access(BA_None)
		, Usage(BU_None)
		, bIsLocked(false)
	{}
	
	virtual ~FOpenGLBuffer();

	bool Initialize(uint32_t InBytes, const void *InData, EBufferAccess InAccess, EBufferUsage InUsage);
	void UnInit();

	void FillData(uint32_t InOffset, uint32_t InBytes, const void *InData);

	// Lock Buffer
	void* Lock(uint32_t InOffset, uint32_t InBytes, EBufferLockMode InMode);
	void UnLock();

	// Active it
	void Bind();
	GLuint NativeResource() const { return Resource; }

	static GLenum TranslateBindTarget(EBufferBindTarget InTarget);
public:
	class FOpenGLRenderer	*Renderer;
	EBufferBindTarget		 Type;

	GLuint			Resource;
	uint32_t		Bytes;
	EBufferAccess	Access;
	EBufferUsage	Usage;
	bool			bIsLocked;
};

// Vertex Buffer
class FRHIOpenGLVertexBuffer : public FRHIVertexBuffer, public FOpenGLBuffer
{
public:
	FRHIOpenGLVertexBuffer(class FOpenGLRenderer *InRenderer);
	virtual ~FRHIOpenGLVertexBuffer() {}

};

// Index Buffer
class FRHIOpenGLIndexBuffer : public FRHIIndexBuffer, public FOpenGLBuffer
{
public:
	FRHIOpenGLIndexBuffer(class FOpenGLRenderer *InRenderer);
	virtual ~FRHIOpenGLIndexBuffer() {}

	virtual uint32_t GetIndexCount() override;

	bool Initialize(uint32_t InBytes, const void *InData, uint16_t InStride, EBufferAccess InAccess, EBufferUsage InUsage);

	// the stride must be 2 or 4
	GLuint GetStride() const { return Stride; }

protected:
	uint16_t		Stride;
};

typedef TRefCountPtr<FRHIOpenGLVertexBuffer>	FRHIOpenGLVertexBufferRef;
typedef TRefCountPtr<FRHIOpenGLIndexBuffer>		FRHIOpenGLIndexBufferRef;

#endif // __JETX_OPENGL_DATA_BUFFER_H__
