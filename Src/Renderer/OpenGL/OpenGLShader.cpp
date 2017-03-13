//\brief
//		OpenGL Shader Implementation.
//

#include <cassert>
#include "OpenGLRenderer.h"
#include "OpenGLShader.h"


FOpenGLShader::FOpenGLShader(GLenum InType, const GLchar *InSource, GLint InLength)
	: ShaderType(InType)
	, Resource(0)
	, CompileStatus(GL_FALSE)
	, InfoLogLength(0)
	, InfoLog(nullptr)
{
	assert(InSource);

	Resource = glCreateShader(ShaderType);
	glShaderSource(Resource, 1, &InSource, &InLength);
	glCompileShader(Resource);
	glGetShaderiv(Resource, GL_COMPILE_STATUS, &CompileStatus);
	glGetShaderiv(Resource, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0)
	{
		InfoLog = new GLchar[InfoLogLength];
		if (InfoLog)
		{
			glGetShaderInfoLog(Resource, InfoLogLength, nullptr, InfoLog);
		}
	}
}

FOpenGLShader::~FOpenGLShader()
{
	glDeleteShader(Resource);
	delete[] InfoLog;
}

bool FOpenGLShader::IsValid() const
{
	return glIsShader(Resource) == GL_TRUE;
}

void FOpenGLShader::DumpDebugInfo(FOutputDevice &OutDevice)
{
	OutDevice.Log(Log_Info, "GL-Shader Dump Debug Info:");
	OutDevice.Log(Log_Info, "    CompileStatus: %s", (CompileStatus ? "TRUE" : "FALSE"));
	OutDevice.Log(Log_Info, "    Info Log: %s", (InfoLog ? InfoLog : ""));
}


FRHIOpenGLVertexShader::FRHIOpenGLVertexShader(const GLchar *InSource, GLint InLength)
	: FOpenGLShader(GL_VERTEX_SHADER, InSource, InLength)
{

}

void FRHIOpenGLVertexShader::Dump(class FOutputDevice &OutDevice)
{
	DumpDebugInfo(OutDevice);
}

FRHIOpenGLPixelShader::FRHIOpenGLPixelShader(const GLchar *InSource, GLint InLength)
	: FOpenGLShader(GL_FRAGMENT_SHADER, InSource, InLength)
{

}

void FRHIOpenGLPixelShader::Dump(class FOutputDevice &OutDevice)
{
	DumpDebugInfo(OutDevice);
}

//////////////////////////////////////////////////////////////////////////
// GPU Program
static int32_t GetUniformElementSize(GLenum InType)
{
	switch (InType)
	{
	case GL_FLOAT:
		return sizeof(GLfloat);
	case GL_FLOAT_VEC2:
		return sizeof(GLfloat) * 2;
	case GL_FLOAT_VEC3:
		return sizeof(GLfloat) * 3;
	case GL_FLOAT_VEC4:
		return sizeof(GLfloat) * 4;

	case GL_INT:
		return sizeof(GLint);
	case GL_INT_VEC2:
		return sizeof(GLint) * 2;
	case GL_INT_VEC3:
		return sizeof(GLint) * 3;
	case GL_INT_VEC4:
		return sizeof(GLint) * 4;
	
	case GL_UNSIGNED_INT:
		return sizeof(GLuint);
	case GL_UNSIGNED_INT_VEC2:
		return sizeof(GLuint) * 2;
	case GL_UNSIGNED_INT_VEC3:
		return sizeof(GLuint) * 3;
	case GL_UNSIGNED_INT_VEC4:
		return sizeof(GLuint) * 4;

	case GL_BOOL:
		return sizeof(GLuint);
	case GL_BOOL_VEC2:
		return sizeof(GLuint) * 2;
	case GL_BOOL_VEC3:
		return sizeof(GLuint) * 3;
	case GL_BOOL_VEC4:
		return sizeof(GLuint) * 4;

	case GL_FLOAT_MAT4:
		return sizeof(GLfloat) * 16;

	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
		return sizeof(GLint);

	default:
		assert(0);
		return -1;
	}
}

FOpenGLProgramUniformInput::FOpenGLProgramUniformInput()
	: Data(nullptr)
	, DataLen(0)
{}

FOpenGLProgramUniformInput::FOpenGLProgramUniformInput(const GLchar* InName, GLenum InType, GLint InSize, GLint InLocation)
	: FOpenGLProgramInput(InName, InType, InSize, InLocation)
	, Modified(false)
{
	DataLen = GetUniformElementSize(InType);
	assert(DataLen > 0);

	Data = new uint8_t[DataLen];
	assert(Data);

	if (Data)
	{
		::memset(Data, 0, DataLen);
	}
}

FOpenGLProgramUniformInput::~FOpenGLProgramUniformInput()
{
	if (Data)
	{
		delete[] Data; Data = nullptr;
	}
}


FRHIOpenGLGPUProgram::FRHIOpenGLGPUProgram(class FOpenGLRenderer *InRenderer)
	: Renderer(InRenderer)
	, Resource(0)
	, LinkStatus(GL_FALSE)
	, InfoLogLength(0)
	, InfoLog(nullptr)
{

}

FRHIOpenGLGPUProgram::~FRHIOpenGLGPUProgram()
{
	if (Resource)
	{
		glDeleteProgram(Resource);
	}
	delete[] InfoLog;
}

void FRHIOpenGLGPUProgram::AddShader(const FRHIShaderRef &InShader)
{
	Shaders.push_back(InShader);
}

bool FRHIOpenGLGPUProgram::Build()
{
	Resource = glCreateProgram();
	if (Resource == 0)
	{
		return false;
	}

	for (uint32_t Index = 0; Index < Shaders.size(); Index++)
	{
		FOpenGLShader *GLShader = dynamic_cast<FOpenGLShader*>(Shaders[Index].DeRef());
		assert(GLShader);

		glAttachShader(Resource, GLShader->NativeResource());
	} // end for Index

	glLinkProgram(Resource);
	glGetProgramiv(Resource, GL_LINK_STATUS, &LinkStatus);
	glGetProgramiv(Resource, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0)
	{
		InfoLog = new GLchar[InfoLogLength];
		if (InfoLog)
		{
			glGetProgramInfoLog(Resource, InfoLogLength, nullptr, InfoLog);
		}
	}

	GLint		AttributesNum;
	GLint		UniformsNum;
	GLchar		VarName[128];
	GLint		VarSize, VarLocation;
	GLenum		VarType;

	glGetProgramiv(Resource, GL_ACTIVE_ATTRIBUTES, &AttributesNum);
	glGetProgramiv(Resource, GL_ACTIVE_UNIFORMS, &UniformsNum);
	// get active attributes
	for (GLint Index = 0; Index < AttributesNum; Index++)
	{
		glGetActiveAttrib(Resource, Index, sizeof(VarName), nullptr, &VarSize, &VarType, VarName);
		VarLocation = glGetAttribLocation(Resource, VarName);
		Attributes.push_back(FOpenGLProgramInput(VarName, VarType, VarSize, VarLocation));
	} // end for

	  // get active uniforms
	for (GLint Index = 0; Index < UniformsNum; Index++)
	{
		glGetActiveUniform(Resource, Index, sizeof(VarName), nullptr, &VarSize, &VarType, VarName);
		VarLocation = glGetUniformLocation(Resource, VarName);
		Uniforms.push_back(FOpenGLProgramUniformInput(VarName, VarType, VarSize, VarLocation));
	} // end for

	return !Renderer->CheckError(__FILE__, __LINE__);
}

void FRHIOpenGLGPUProgram::Dump(class FOutputDevice &OutDevice)
{
	OutDevice.Log(Log_Info, "GL-Program Dump Debug Info:");
	OutDevice.Log(Log_Info, "    LinkStatus: %s", (LinkStatus ? "TRUE" : "FALSE"));
	OutDevice.Log(Log_Info, "    Active Attributes Count: %d", Attributes.size());
	OutDevice.Log(Log_Info, "    Active Uniforms Count: %d", Uniforms.size());
	OutDevice.Log(Log_Info, "    Info Log: %s", (InfoLog ? InfoLog : ""));
	OutDevice.Log(Log_Info, "    Input Attributes List:");
	// dump attributes
	for (size_t Index = 0; Index < Attributes.size(); Index++)
	{
		const FOpenGLProgramInput &Element = Attributes[Index];
		OutDevice.Log(Log_Info, "       name=%s, type=%s, size=%d, location=%d", Element.Name.c_str(),
			FOpenGLRenderer::LookupShaderAttributeTypeName(Element.Type), Element.Size, Element.Location);
	}
	// dump uniform params
	OutDevice.Log(Log_Info, "    Input Uniform List:");
	for (size_t Index = 0; Index < Uniforms.size(); Index++)
	{
		const FOpenGLProgramInput &Element = Uniforms[Index];
		OutDevice.Log(Log_Info, "       name=%s, type=%s, size=%d, location=%d", Element.Name.c_str(),
			FOpenGLRenderer::LookupShaderUniformTypeName(Element.Type), Element.Size, Element.Location);
	}
}

bool FRHIOpenGLGPUProgram::IsValid() const
{
	return glIsProgram(Resource) == GL_TRUE;
}

// get uniform parameter handle
int32_t FRHIOpenGLGPUProgram::GetUniformHandle(const std::string &InName)
{
	for (size_t Index = 0; Index < Uniforms.size(); Index++)
	{
		const FOpenGLProgramInput &Element = Uniforms[Index];
		if (Element.Name == InName)
		{
			return (int32_t)Index;
		}
	}

	return -1;
}

bool FRHIOpenGLGPUProgram::SetUniformCommon(int32_t InHandle, const void *V, uint32_t InBytes, uint32_t /*InCount*/)
{
	if (!V || InHandle < 0 || InHandle >= (int32_t)Uniforms.size())
	{
		return false;
	}

	FOpenGLProgramUniformInput &Element = Uniforms[InHandle];
	if (Element.DataBytes() < InBytes)
	{
		return false;
	}

	::memcpy(Element.DataPtr(), V, InBytes);
	Element.SetModified(true);
	return true;
}

bool FRHIOpenGLGPUProgram::SetUniform1iv(int32_t InHandle, const int32_t *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(int32_t) * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform2iv(int32_t InHandle, const int32_t *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(int32_t) * 2 * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform3iv(int32_t InHandle, const int32_t *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(int32_t) * 3 * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform4iv(int32_t InHandle, const int32_t *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(int32_t) * 4 * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform1uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(uint32_t) * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform2uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(uint32_t) * 2 * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform3uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(uint32_t) * 3 * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform4uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(uint32_t) * 4 * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform1fv(int32_t InHandle, const float *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(float) * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform2fv(int32_t InHandle, const float *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(float) * 2 * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform3fv(int32_t InHandle, const float *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(float) * 3 * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniform4fv(int32_t InHandle, const float *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(float) * 4 * InCount, InCount);
}

bool FRHIOpenGLGPUProgram::SetUniformMatrix4fv(int32_t InHandle, const float *V, uint32_t InCount)
{
	return SetUniformCommon(InHandle, V, sizeof(float) * 16 * InCount, InCount);
}

void FRHIOpenGLGPUProgram::UpdateUniformVariables()
{
	for (size_t Index = 0; Index < Uniforms.size(); Index++)
	{
		FOpenGLProgramUniformInput &Element = Uniforms[Index];
		
		if (!Element.GetModified())
		{
			continue;
		}

		Element.SetModified(false);
		switch (Element.Type)
		{
		case GL_FLOAT:
			glUniform1fv(Element.Location, Element.Size, (GLfloat*)Element.DataPtr());
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv(Element.Location, Element.Size, (GLfloat*)Element.DataPtr());
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(Element.Location, Element.Size, (GLfloat*)Element.DataPtr());
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(Element.Location, Element.Size, (GLfloat*)Element.DataPtr());
			break;

		case GL_INT:
			glUniform1iv(Element.Location, Element.Size, (GLint*)Element.DataPtr());
			break;
		case GL_INT_VEC2:
			glUniform2iv(Element.Location, Element.Size, (GLint*)Element.DataPtr());
			break;
		case GL_INT_VEC3:
			glUniform3iv(Element.Location, Element.Size, (GLint*)Element.DataPtr());
			break;
		case GL_INT_VEC4:
			glUniform4iv(Element.Location, Element.Size, (GLint*)Element.DataPtr());
			break;

		case GL_UNSIGNED_INT:
		case GL_BOOL:
			glUniform1uiv(Element.Location, Element.Size, (GLuint*)Element.DataPtr());
			break;
		case GL_UNSIGNED_INT_VEC2:
		case GL_BOOL_VEC2:
			glUniform2uiv(Element.Location, Element.Size, (GLuint*)Element.DataPtr());
			break;
		case GL_UNSIGNED_INT_VEC3:
		case GL_BOOL_VEC3:
			glUniform3uiv(Element.Location, Element.Size, (GLuint*)Element.DataPtr());
			break;
		case GL_UNSIGNED_INT_VEC4:
		case GL_BOOL_VEC4:
			glUniform4uiv(Element.Location, Element.Size, (GLuint*)Element.DataPtr());
			break;

		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(Element.Location, Element.Size, GL_FALSE, (GLfloat*)Element.DataPtr());
			break;

		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
			glUniform1iv(Element.Location, Element.Size, (GLint*)Element.DataPtr());
			break;

		default:
			assert(0);
		}
	} // end for
}
