//\brief
//		OpenGL Shader
//

#ifndef __JETX_OPENGL_SHADER_H__
#define __JETX_OPENGL_SHADER_H__

#include "Foundation/JetX.h"
#include "Foundation/OutputDevice.h"
#include "Renderer/RendererDefs.h"
#include "Renderer/RHIResource.h"

#include <vector>
#include <GL/glew.h>


// \brief
//		OpenGL Shader
class FOpenGLShader
{
public:
	virtual ~FOpenGLShader();

	GLuint NativeResource() const { return Resource; }
	bool IsValid() const;
	void DumpDebugInfo(FOutputDevice &OutDevice);

protected:
	FOpenGLShader(GLenum InType, const GLchar *InSource, GLint InLength = -1);

private:
	GLenum		ShaderType;
	GLuint		Resource;
	GLint		CompileStatus;
	GLint		InfoLogLength;
	GLchar     *InfoLog;
};

// Vertex Shader
class FRHIOpenGLVertexShader : public FRHIVertexShader, public FOpenGLShader
{
public:
	FRHIOpenGLVertexShader(const GLchar *InSource, GLint InLength = -1);
	virtual ~FRHIOpenGLVertexShader() {}

	virtual void Dump(class FOutputDevice &OutDevice) override;
};

//Pixel Shader
class FRHIOpenGLPixelShader : public FRHIPixelShader, public FOpenGLShader
{
public:
	FRHIOpenGLPixelShader(const GLchar *InSource, GLint InLength = -1);
	virtual ~FRHIOpenGLPixelShader() {}

	virtual void Dump(class FOutputDevice &OutDevice) override;
};

typedef TRefCountPtr<FRHIOpenGLVertexShader>  FRHIOpenGLVertexShaderRef;
typedef TRefCountPtr<FRHIOpenGLPixelShader>	  FRHIOpenGLPixelShaderRef;


//////////////////////////////////////////////////////////////////////////
// GL Program

// Input Parameter of Program
// 1. input vertex attributes
// 2. uniform parameters
class FOpenGLProgramInput
{
public:
	FOpenGLProgramInput()
		: Name("undefined")
		, Type(0)
		, Size(0)
		, Location(-1)
	{
	}

	FOpenGLProgramInput(const GLchar* InName, GLenum InType, GLint InSize, GLint InLocation)
		: Name(InName)
		, Type(InType)
		, Size(InSize)
		, Location(InLocation)
	{
	}

public:
	std::string		Name;
	GLenum			Type;
	GLint			Size;
	GLint			Location;
};

class FRHIOpenGLGPUProgram : public FRHIGPUProgram
{
public:
	FRHIOpenGLGPUProgram(class FOpenGLRenderer	*InRenderer);
	virtual ~FRHIOpenGLGPUProgram();

	void AddShader(const FRHIShaderRef &InShader);
	bool Build();

	virtual void Dump(class FOutputDevice &OutDevice) override;

	GLuint NativeResource() const { return Resource; }
	bool IsValid() const;

	GLint GetUniformLocation(const std::string &InName) const;

private:
	class FOpenGLRenderer	*Renderer;

	GLuint		Resource;
	GLint		LinkStatus;
	GLint		InfoLogLength;
	GLchar     *InfoLog;

	std::vector<FRHIShaderRef>			Shaders;
	std::vector<FOpenGLProgramInput>	Attributes;
	std::vector<FOpenGLProgramInput>	Uniforms;
};

typedef TRefCountPtr<FRHIOpenGLGPUProgram>	FRHIOpenGLGPUProgramRef;

#endif // __JETX_OPENGL_SHADER_H__
