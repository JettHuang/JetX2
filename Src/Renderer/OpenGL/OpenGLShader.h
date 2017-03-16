//\brief
//		OpenGL Shader
//

#ifndef __JETX_OPENGL_SHADER_H__
#define __JETX_OPENGL_SHADER_H__

#include "Foundation/JetX.h"
#include "Foundation/OutputDevice.h"
#include "Renderer/RendererDefs.h"
#include "Renderer/RHIResource.h"
#include "glew.h"

#include <vector>


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
// 2. uniform variable
class FOpenGLProgramInput
{
public:
	FOpenGLProgramInput()
		: Type(0)
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

// uniform variable
class FOpenGLProgramUniformInput : public FOpenGLProgramInput 
{
public:
	FOpenGLProgramUniformInput();
	FOpenGLProgramUniformInput(const GLchar* InName, GLenum InType, GLint InSize, GLint InLocation);

	~FOpenGLProgramUniformInput();

	bool GetModified() const { return Modified; }
	void SetModified(bool InDirty) { Modified = InDirty; }

	void* DataPtr() const { return Data; }
	uint32_t DataBytes() const { return DataLen; }

protected:
	uint8_t		*Data;
	uint32_t	 DataLen;
	bool		 Modified;
};

// Uniform Input Block
/*
struct FUniformInputEntry
{
	union 
	{
		int32_t			IntVal;
		tvec2<int32_t>	Int2Val;
		tvec3<int32_t>	Int3Val;
		tvec4<int32_t>	Int4Val;

		uint32_t		UIntVal;
		tvec2<uint32_t>	UInt2Val;
		tvec3<uint32_t>	UInt3Val;
		tvec4<uint32_t>	UInt4Val;

		float			FltVal;
		tvec2<float>	Flt2Val;
		tvec3<float>	Flt3Val;
		tvec4<float>	Flt4Val;

		mat4x4			Matrix4;
	};
};
*/

class FRHIOpenGLGPUProgram : public FRHIGPUProgram
{
public:
	FRHIOpenGLGPUProgram(class FOpenGLRenderer	*InRenderer);
	virtual ~FRHIOpenGLGPUProgram();

	void AddShader(const FRHIShaderRef &InShader);
	bool Build();

	virtual void Dump(class FOutputDevice &OutDevice) override;
	// get uniform parameter handle
	virtual int32_t GetUniformHandle(const std::string &InName) override;

	virtual bool SetUniform1iv(int32_t InHandle, const int32_t *V, uint32_t InCount) override;
	virtual bool SetUniform2iv(int32_t InHandle, const int32_t *V, uint32_t InCount) override;
	virtual bool SetUniform3iv(int32_t InHandle, const int32_t *V, uint32_t InCount) override;
	virtual bool SetUniform4iv(int32_t InHandle, const int32_t *V, uint32_t InCount) override;

	virtual bool SetUniform1uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount) override;
	virtual bool SetUniform2uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount) override;
	virtual bool SetUniform3uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount) override;
	virtual bool SetUniform4uiv(int32_t InHandle, const uint32_t *V, uint32_t InCount) override;

	virtual bool SetUniform1fv(int32_t InHandle, const float *V, uint32_t InCount) override;
	virtual bool SetUniform2fv(int32_t InHandle, const float *V, uint32_t InCount) override;
	virtual bool SetUniform3fv(int32_t InHandle, const float *V, uint32_t InCount) override;
	virtual bool SetUniform4fv(int32_t InHandle, const float *V, uint32_t InCount) override;

	virtual bool SetUniformMatrix4fv(int32_t InHandle, const float *V, uint32_t InCount) override;

	void UpdateUniformVariables();

	GLuint NativeResource() const { return Resource; }
	bool IsValid() const;

protected:
	bool SetUniformCommon(int32_t InHandle, const void *V, uint32_t InBytes, uint32_t InCount);
private:
	class FOpenGLRenderer	*Renderer;

	GLuint		Resource;
	GLint		LinkStatus;
	GLint		InfoLogLength;
	GLchar     *InfoLog;

	std::vector<FRHIShaderRef>			Shaders;
	std::vector<FOpenGLProgramInput>	Attributes;
	std::vector<FOpenGLProgramUniformInput>	Uniforms;
};

typedef TRefCountPtr<FRHIOpenGLGPUProgram>	FRHIOpenGLGPUProgramRef;

#endif // __JETX_OPENGL_SHADER_H__
