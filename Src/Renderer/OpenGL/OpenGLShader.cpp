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

#if 0
FOpenGLProgram::FOpenGLProgram(FOpenGLVertexShader *InVertexShader, FOpenGLPixelShader *InPixelShader)
	: Resource(0)
	, LinkStatus(GL_FALSE)
	, InfoLogLength(0)
	, InfoLog(nullptr)
	, AttributesNum(0)
	, UniformsNum(0)
{
	assert(InVertexShader && InPixelShader);


}

FOpenGLProgram::~FOpenGLProgram()
{
	if (Resource)
	{
		glDeleteProgram(Resource);
	}
	delete[] InfoLog;
}

bool FOpenGLProgram::IsValid() const
{
	return glIsProgram(Resource) == GL_TRUE;
}

void FOpenGLProgram::DumpDebugInfo()
{

}

GLint FOpenGLProgram::GetParamLocation(const std::string &InParamName) const
{

}

#endif

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
		Uniforms.push_back(FOpenGLProgramInput(VarName, VarType, VarSize, VarLocation));
	} // end for

	return Renderer->CheckError(__FILE__, __LINE__);
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

GLint FRHIOpenGLGPUProgram::GetUniformLocation(const std::string &InName) const
{
	std::vector<FOpenGLProgramInput>::const_iterator It = Uniforms.begin();

	for (; It != Uniforms.end(); It++)
	{
		const FOpenGLProgramInput &Entry = *It;
		if (InName == Entry.Name)
		{
			return Entry.Location;
		}
	} // end for It

	return -1;
}
