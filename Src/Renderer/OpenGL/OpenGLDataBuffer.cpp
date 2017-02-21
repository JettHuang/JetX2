// \brief
//		OpenGL Vertex Buffer implementation.
//

#include <cassert>
#include "OpenGLRenderer.h"
#include "OpenGLDataBuffer.h"


static GLenum TranslateBufferAccessUsage(EBufferAccess InAccess, EBufferUsage InUsage)
{
	switch ((InAccess | InUsage))
	{
	case (BA_Static | BU_Draw):
		return GL_STATIC_DRAW;
	case (BA_Static | BU_Read):
		return GL_STATIC_READ;
	case (BA_Static | BU_Copy):
		return GL_STATIC_COPY;
	case (BA_Dynamic | BU_Draw):
		return GL_DYNAMIC_DRAW;
	case (BA_Dynamic | BU_Read):
		return GL_DYNAMIC_READ;
	case (BA_Dynamic | BU_Copy):
		return GL_DYNAMIC_COPY;
	case (BA_Stream | BU_Draw):
		return GL_STREAM_DRAW;
	case (BA_Stream | BU_Read):
		return GL_STREAM_READ;
	case (BA_Stream | BU_Copy):
		return GL_STREAM_COPY;
	default:
		assert(0);
		return GL_STATIC_DRAW;
	}
}

static GLbitfield TranslateBufferLockMode(EBufferLockMode InMode)
{
	switch (InMode)
	{
	case BL_ReadOnly:
		return GL_MAP_READ_BIT;
	case BL_WriteOnly:
		return GL_MAP_WRITE_BIT;
	case BL_ReadWrite:
		return (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	default:
		assert(0);
		return (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	}
}

GLenum FOpenGLBuffer::TranslateBindTarget(EBufferBindTarget InTarget)
{
	static const GLenum kBindPointTable[MaxBufferBinds] = 
	{
		GL_ARRAY_BUFFER,
		GL_COPY_READ_BUFFER,
		GL_COPY_WRITE_BUFFER,
		GL_DRAW_INDIRECT_BUFFER,
		GL_ELEMENT_ARRAY_BUFFER,
		GL_PIXEL_PACK_BUFFER,
		GL_PIXEL_UNPACK_BUFFER,
		GL_TEXTURE_BUFFER,
		GL_TRANSFORM_FEEDBACK_BUFFER,
		GL_UNIFORM_BUFFER
	};

	assert(InTarget < MaxBufferBinds);
	return kBindPointTable[InTarget];
}

FOpenGLBuffer::~FOpenGLBuffer()
{
	UnInit();
}

bool FOpenGLBuffer::Initialize(uint32_t InBytes, const void *InData, EBufferAccess InAccess, EBufferUsage InUsage)
{
	glGenBuffers(1, &Resource);
	if (Resource != 0)
	{
		Bytes = InBytes;
		Access = InAccess;
		Usage = InUsage;

		Bind();
		glBufferData(TranslateBindTarget(Type), InBytes, InData, TranslateBufferAccessUsage(InAccess, InUsage));
		return !(Renderer->CheckError(__FILE__, __LINE__));
	}

	return false;
}

void FOpenGLBuffer::UnInit()
{
	if (Resource)
	{
		glDeleteBuffers(1, &Resource);
		Renderer->OnBufferDeleted(Type, Resource);
		Resource = 0;
	}
}

void FOpenGLBuffer::FillData(uint32_t InOffset, uint32_t InBytes, const void *InData)
{
	Bind();
	glBufferSubData(TranslateBindTarget(Type), InOffset, InBytes, InData);
	Renderer->CheckError(__FILE__, __LINE__);
}

// Lock Buffer
void* FOpenGLBuffer::Lock(uint32_t InOffset, uint32_t InBytes, EBufferLockMode InMode)
{
	assert(!bIsLocked);
	bIsLocked = true;

	Bind();
	void *pData = nullptr;

	pData = glMapBufferRange(TranslateBindTarget(Type), InOffset, InBytes, TranslateBufferLockMode(InMode));
	Renderer->CheckError(__FILE__, __LINE__);
	return pData;
}

void FOpenGLBuffer::UnLock()
{
	assert(bIsLocked);
	bIsLocked = false;

	Bind();
	glUnmapBuffer(TranslateBindTarget(Type));
	Renderer->CheckError(__FILE__, __LINE__);
}

// Active it
void FOpenGLBuffer::Bind()
{
	Renderer->CachedBindBuffer(Type, Resource);
}

//////////////////////////////////////////////////////////////////////////
// Vertex Buffer
FRHIOpenGLVertexBuffer::FRHIOpenGLVertexBuffer(class FOpenGLRenderer *InRenderer)
	: FOpenGLBuffer(InRenderer, Array_Buffer)
{
}

//////////////////////////////////////////////////////////////////////////
// Index Buffer
FRHIOpenGLIndexBuffer::FRHIOpenGLIndexBuffer(class FOpenGLRenderer *InRenderer)
	: FOpenGLBuffer(InRenderer, ElementArray_Buffer)
{
}

bool FRHIOpenGLIndexBuffer::Initialize(uint32_t InBytes, const void *InData, uint16_t InStride, EBufferAccess InAccess, EBufferUsage InUsage)
{
	Stride = InStride;
	return FOpenGLBuffer::Initialize(InBytes, InData, InAccess, InUsage);
}
