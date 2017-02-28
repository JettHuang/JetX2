// \brief
//		Implementation of OpenGL Vertex Declaration
//

#include "OpenGLVertexDeclaration.h"


static void SetGLElement(FOpenGLVertexElement &GLElement, GLenum InType, GLint InSize, GLboolean InNormalized, GLboolean InShouldConvertToFloat)
{
	GLElement.Type = InType;
	GLElement.Size = InSize;
	GLElement.Normalized = InNormalized;
	GLElement.ShouldConvertToFloat = InShouldConvertToFloat;
}

FRHIOpenGLVertexDeclaration::FRHIOpenGLVertexDeclaration(const FVertexElement *InVertexElements, uint32_t InCount)
{
	for (uint32_t Index=0; Index < InCount; Index++)
	{
		const FVertexElement &VertexElement = InVertexElements[Index];
		FOpenGLVertexElement GLElement;

		GLElement.StreamIndex = VertexElement.StreamIndex;
		GLElement.AttributeIndex = VertexElement.AttributeIndex;
		GLElement.Offset = VertexElement.Offset;
		GLElement.Stride = VertexElement.Stride;
		GLElement.Divisor = VertexElement.Divisor;
		switch (VertexElement.DataType)
		{
		case VET_Float1:	SetGLElement(GLElement, GL_FLOAT, 1, GL_FALSE, GL_TRUE); break;
		case VET_Float2:	SetGLElement(GLElement, GL_FLOAT, 2, GL_FALSE, GL_TRUE); break;
		case VET_Float3:	SetGLElement(GLElement, GL_FLOAT, 3, GL_FALSE, GL_TRUE); break;
		case VET_Float4:	SetGLElement(GLElement, GL_FLOAT, 4, GL_FALSE, GL_TRUE); break;
		case VET_PackedNormal:	SetGLElement(GLElement, GL_UNSIGNED_BYTE, 4, GL_TRUE, GL_TRUE); break;
		case VET_UByte4:	SetGLElement(GLElement, GL_UNSIGNED_BYTE, 4, GL_FALSE, GL_FALSE); break;
		case VET_UByte4N:	SetGLElement(GLElement, GL_UNSIGNED_BYTE, 4, GL_TRUE, GL_TRUE); break;
		case VET_Color:		SetGLElement(GLElement, GL_UNSIGNED_BYTE, 4, GL_TRUE, GL_TRUE); break;
		case VET_Short2:	SetGLElement(GLElement, GL_SHORT, 2, GL_FALSE, GL_FALSE); break;
		case VET_Short4:	SetGLElement(GLElement, GL_SHORT, 4, GL_FALSE, GL_FALSE); break;
		case VET_Short2N:	SetGLElement(GLElement, GL_SHORT, 2, GL_TRUE, GL_TRUE); break;
		case VET_Half2:		SetGLElement(GLElement, GL_HALF_FLOAT, 2, GL_FALSE, GL_TRUE); break;
		case VET_Half4:		SetGLElement(GLElement, GL_HALF_FLOAT, 4, GL_FALSE, GL_TRUE); break;
		case VET_Short4N:	SetGLElement(GLElement, GL_SHORT, 4, GL_TRUE, GL_TRUE); break;	// 4 X 16 bit word, normalized 
		case VET_UShort2:	SetGLElement(GLElement, GL_UNSIGNED_SHORT, 2, GL_FALSE, GL_FALSE); break;
		case VET_UShort4:	SetGLElement(GLElement, GL_UNSIGNED_SHORT, 4, GL_FALSE, GL_FALSE); break;
		case VET_UShort2N:	SetGLElement(GLElement, GL_UNSIGNED_SHORT, 2, GL_TRUE, GL_TRUE); break;	// 16 bit word normalized to (value/65535.0,value/65535.0,0,0,1)
		case VET_UShort4N:	SetGLElement(GLElement, GL_UNSIGNED_SHORT, 4, GL_TRUE, GL_TRUE); break;
		case VET_URGB10A2N:	SetGLElement(GLElement, GL_UNSIGNED_INT_2_10_10_10_REV, 4, GL_TRUE, GL_TRUE); break;
		default:
			assert(false);
			break;
		}

		VertexInputLayout.push_back(GLElement);
	} // end for Index
}
