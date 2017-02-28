// \brief
//		methods for GL-Resource creating
//

#include "OpenGLDataBuffer.h"
#include "OpenGLRenderer.h"


// vertex buffers
FRHIVertexBufferRef FOpenGLRenderer::RHICreateVertexBuffer(uint32_t InBytes, const void *InData, EBufferAccess InAccess, EBufferUsage InUsage)
{
	FRHIOpenGLVertexBuffer *VBuffer = new FRHIOpenGLVertexBuffer(this);
	if (VBuffer && VBuffer->Initialize(InBytes, InData, InAccess, InUsage))
	{
		return VBuffer;
	}

	delete VBuffer;
	return nullptr;
}

FRHIIndexBufferRef FOpenGLRenderer::RHICreateIndexBuffer(uint32_t InBytes, const void *InData, uint16_t InStride, EBufferAccess InAccess, EBufferUsage InUsage)
{
	FRHIOpenGLIndexBuffer *IBuffer = new FRHIOpenGLIndexBuffer(this);
	if (IBuffer && IBuffer->Initialize(InBytes, InData, InStride, InAccess, InUsage))
	{
		return IBuffer;
	}

	delete IBuffer;
	return nullptr;
}

void FOpenGLRenderer::FillDataBuffer(FRHIDataBufferRef InBuffer, uint32_t InOffset, uint32_t InBytes, const void *InData)
{
	FOpenGLBuffer *OpenGLBuffer = dynamic_cast<FOpenGLBuffer*>(InBuffer.DeRef());
	if (OpenGLBuffer)
	{
		OpenGLBuffer->FillData(InOffset, InBytes, InData);
	}
}

void* FOpenGLRenderer::LockDataBuffer(FRHIDataBufferRef InBuffer, uint32_t InOffset, uint32_t InBytes, EBufferLockMode InMode)
{
	FOpenGLBuffer *OpenGLBuffer = dynamic_cast<FOpenGLBuffer*>(InBuffer.DeRef());
	if (OpenGLBuffer)
	{
		return OpenGLBuffer->Lock(InOffset, InBytes, InMode);
	}

	return nullptr;
}

void FOpenGLRenderer::UnLockDataBuffer(FRHIDataBufferRef InBuffer)
{
	FOpenGLBuffer *OpenGLBuffer = dynamic_cast<FOpenGLBuffer*>(InBuffer.DeRef());
	if (OpenGLBuffer)
	{
		return OpenGLBuffer->UnLock();
	}
}

FRHIVertexDeclarationRef FOpenGLRenderer::RHICreateVertexInputLayout(const FVertexElement *InVertexElements, uint32_t InCount)
{
	return new FRHIOpenGLVertexDeclaration(InVertexElements, InCount);
}
