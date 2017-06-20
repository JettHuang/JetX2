//\brief
//		OpenGL pipeline vertex attributes data-structure.
//

#ifndef __JETX_OPENGL_VERTEX_DECL_H__
#define __JETX_OPENGL_VERTEX_DECL_H__

#include "Renderer/RendererDefs.h"
#include "Renderer/RHIResource.h"
#include "PlatformOpenGL.h"


// GL vertex element
struct FOpenGLVertexElement
{
	GLuint		StreamIndex;
	GLuint		AttributeIndex;
	GLint		Size;
	GLenum		Type;
	GLsizei		Stride;
	GLuint		Offset;
	GLuint		Divisor;
	GLboolean	Normalized;
	GLboolean	ShouldConvertToFloat;

	FOpenGLVertexElement() {}

	FOpenGLVertexElement(GLuint InStreamIndex, GLuint InAttributeIndex, GLint InSize, GLenum InType, GLsizei InStride, GLuint InOffset, GLuint InDivisor, GLboolean InNormalized,
		GLboolean InShouldConvertToFloat)
		: StreamIndex(InStreamIndex)
		, AttributeIndex(InAttributeIndex)
		, Size(InSize)
		, Type(InType)
		, Stride(InStride)
		, Offset(InOffset)
		, Divisor(InDivisor)
		, Normalized(InNormalized)
		, ShouldConvertToFloat(InShouldConvertToFloat)
	{}

	void SetValue(GLuint InStreamIndex, GLuint InAttributeIndex, GLint InSize, GLenum InType, GLsizei InStride, GLuint InOffset, GLuint InDivisor,GLboolean InNormalized, GLboolean InShouldConvertToFloat)
	{
		StreamIndex = InStreamIndex;
		AttributeIndex = InAttributeIndex;
		Size = InSize;
		Type = InType;
		Stride = InStride;
		Offset = InOffset;
		Divisor = InDivisor;
		Normalized = InNormalized;
		ShouldConvertToFloat = InShouldConvertToFloat;
	}
};

typedef std::vector<FOpenGLVertexElement>  FOpenGLVertexElementsList;

// vertex declaration
class FRHIOpenGLVertexDeclaration : public FRHIVertexDeclaration
{
public:
	FRHIOpenGLVertexDeclaration(const FVertexElement *InVertexElements, uint32_t InCount);

	FOpenGLVertexElementsList	VertexInputLayout;
};

typedef TRefCountPtr<FRHIOpenGLVertexDeclaration>	FRHIOpenGLVertexDeclarationRef;

#endif // __JETX_OPENGL_VERTEX_DECL_H__
