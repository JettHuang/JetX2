// \brief
//		OpenGL renderer base on core3.3
//

#ifndef __JETX_OPENGL_RENDERER_H__
#define __JETX_OPENGL_RENDERER_H__

#include <map>
#include <vector>
#include "Renderer/Renderer.h"
#include "PlatformOpenGL.h"
#include "OpenGLState.h"
#include "OpenGLDataBuffer.h"
#include "OpenGLVertexDeclaration.h"
#include "OpenGLShader.h"


//FOpenGLRenderer
class FOpenGLRenderer : public FRenderer
{
public:
	FOpenGLRenderer() {}

	//Init
	virtual void Init(FOutputDevice *LogOutputDevice) override;
	virtual void Shutdown() override;

	//Capabilities
	virtual void DumpCapabilities() override;

//render viewport
	virtual FRHIViewportRef RHICreateViewport(void* InWindowHandle, uint32_t SizeX, uint32_t SizeY, bool bIsFullscreen) override;
	virtual void RHIResizeViewport(FRHIViewportRef InViewport, uint32_t SizeX, uint32_t SizeY, bool bIsFullscreen) override;
	
	virtual bool RHIGetAvailableResolutions(FScreenResolutionArray& Resolutions, bool bIgnoreRefreshRate) override;
	virtual void RHIGetSupportedResolution(uint32_t& Width, uint32_t& Height) override;

//Resource Creating
	// states
	virtual FRHISamplerStateRef RHICreateSamplerState(const FSamplerStateInitializerRHI &SamplerStateInitializer) override;
	virtual FRHIRasterizerStateRef RHICreateRasterizerState(const FRasterizerStateInitializerRHI &RasterizerStateInitializer) override;
	virtual FRHIDepthStencilStateRef RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI &DepthStencilStateInitializer) override;
	virtual FRHIBlendStateRef RHICreateBlendState(const FBlendStateInitializerRHI &BlendStateInitializer) override;

	// vertex buffers
	virtual FRHIVertexBufferRef RHICreateVertexBuffer(uint32_t InBytes, const void *InData, EBufferAccess InAccess, EBufferUsage InUsage) override;
	virtual FRHIIndexBufferRef RHICreateIndexBuffer(uint32_t InBytes, const void *InData, uint16_t InStride, EBufferAccess InAccess, EBufferUsage InUsage) override;

	virtual void FillDataBuffer(FRHIDataBufferRef InBuffer, uint32_t InOffset, uint32_t InBytes, const void *InData) override;
	virtual void* LockDataBuffer(FRHIDataBufferRef InBuffer, uint32_t InOffset, uint32_t InBytes, EBufferLockMode InMode) override;
	virtual void UnLockDataBuffer(FRHIDataBufferRef InBuffer) override;

	// vertex input layout
	virtual FRHIVertexDeclarationRef RHICreateVertexInputLayout(const FVertexElement *InVertexElements, uint32_t InCount) override;

	// textures
	//virtual FRHITexture2DRef RHICreateTexture2D() override;

	// shader
	virtual FRHIVertexShaderRef RHICreateVertexShader(const char *InSource, int32_t InLength = -1) override;
	virtual FRHIPixelShaderRef RHICreatePixelShader(const char *InSource, int32_t InLength = -1) override;
	virtual FRHIGPUProgramRef RHICreateGPUProgram(const FRHIVertexShaderRef &InVShader, const FRHIPixelShaderRef &InPShader) override;
	virtual FRHIGPUProgramRef RHICreateGPUProgram(const std::vector<FRHIShaderRef> &InShaders) override;

//State Setting
	virtual void RHISetSamplerState(uint32_t InTexIndex, const FRHISamplerStateRef &InSamplerState) override;
	virtual void RHISetRasterizerState(const FRHIRasterizerStateRef &InRasterizerState) override;
	virtual void RHISetDepthStencilState(const FRHIDepthStencilStateRef &InDepthStencilState, int32_t InStencilRef) override;
	virtual void RHISetBlendState(const FRHIBlendStateRef &InBlendState, const FLinearColor &InBlendColor) override;

	virtual void RHISetViewport(int32_t InX, int32_t InY, int32_t InWidth, int32_t InHeight, float InMinZ, float InMaxZ) override;
	virtual void RHISetScissorRect(int32_t InX, int32_t InY, int32_t InWidth, int32_t InHeight) override;

	virtual void SetVertexStreamSource(uint32_t InStreamIndex, const FRHIVertexBufferRef &InVertexBuffer) override;
	virtual void SetVertexInputLayout(const FRHIVertexDeclarationRef &InVertexDecl) override;
	virtual void SetGPUProgram(const FRHIGPUProgramRef &InProgram) override;

//Draw Commands
	virtual void RHIBeginDrawingViewport(FRHIViewportRef Viewport) override;
	virtual void RHIEndDrawingViewport(FRHIViewportRef Viewport, bool bPresent, bool bLockToVsync) override;
	virtual void RHIBeginFrame() override;
	virtual void RHIEndFrame() override;

	virtual void RHIClear(bool bClearColor, const FLinearColor &InColor, bool bClearDepth, float InDepth, bool bClearStencil, int32_t InStencil) override;
	virtual void RHIClearMRT(bool bClearColor, const FLinearColor *InColors, uint32_t InColorsNum, bool bClearDepth, float InDepth, bool bClearStencil, int32_t InStencil) override;

	// draw primitives
	virtual void DrawIndexedPrimitive(const FRHIIndexBufferRef &InIndexBuffer, EPrimitiveType InMode, uint32_t InStart, uint32_t InCount) override;
	virtual void DrawIndexedPrimitiveInstanced(const FRHIIndexBufferRef &InIndexBuffer, EPrimitiveType InMode, uint32_t InStart, uint32_t InCount, uint32_t InInstances) override;
	virtual void DrawArrayedPrimitive(EPrimitiveType InMode, uint32_t InStart, uint32_t InCount) override;
	virtual void DrawArrayedPrimitiveInstanced(EPrimitiveType InMode, uint32_t InStart, uint32_t InCount, uint32_t InInstances) override;

//Others
	void AddViewport(class FRHIOpenGLViewport *InViewport);
	void RemoveViewport(class FRHIOpenGLViewport *InViewport);

	void CachedBindBuffer(EBufferBindTarget InBindPoint, GLuint InBuffer);
	void OnBufferDeleted(EBufferBindTarget InBindPoint, GLuint InBuffer);

//Helpers
	//\brief
	//	return true if has error.
	bool CheckError(const char* FILE, int LINE);

	static const char* LookupShaderAttributeTypeName(GLenum InType);
	static const char* LookupShaderUniformTypeName(GLenum InType);
	static const char* LookupErrorCode(GLenum InError);

    // create & release viewport context
    FPlatformViewportContext* CreateViewportContext(void* InWindowHandle);
    void ReleaseViewportContext(FPlatformViewportContext* InContext);
    void ResizeViewportContext(FPlatformViewportContext* InContext, uint32_t SizeX, uint32_t SizeY, bool bFullscreen, bool bWasFullscreen);
protected:
	void UpdatePendingRasterizerState(bool bForce=false);
	void UpdatePendingSamplers(bool bForce=false);
	void UpdatePendingDepthStencilState(bool bForce=false);
	void UpdatePendingBlendState(bool bForce=false);
	void UpdatePendingVertexInputLayout(bool bForce=false);
	void CachedEnableVertexAttributePointer(GLuint InBuffer, const FOpenGLVertexElement &InVertexElement);

	void UpdateGPUProgram();

protected:
	struct FIntRect
	{
		FIntRect() 
			: x(0)
			, y(0)
			, width(0)
			, height(0)
		{}

		GLint x, y, width, height;
	};
	struct FViewportBox
	{
		FViewportBox()
			: x(0)
			, y(0)
			, width(0)
			, height(0)
			, zMin(0.f)
			, zMax(1.f)
		{}

		GLint x, y, width, height;
		GLfloat zMin, zMax;
	};

	// Vertex Input Attribute
	struct FVertexInputAttribute
	{
		GLuint		Buffer;
		GLint		Size;
		GLenum		Type;
		GLsizei		Stride;
		GLuint		Offset;
		GLuint		Divisor;
		GLboolean	Normalized;
		GLboolean	ShouldConvertToFloat;
		GLboolean	Enabled;

		FVertexInputAttribute()
			: Buffer(0)
			, Size(0)
			, Type(GL_NONE)
			, Stride(0)
			, Offset(0)
			, Divisor(0)
			, Normalized(GL_FALSE)
			, ShouldConvertToFloat(GL_FALSE)
			, Enabled(GL_FALSE)
		{
		}
	};

	// VAO State
	struct FVertexArrayObjectState
	{
		FVertexInputAttribute	VertexInputAttris[MaxVertexAttributes];
		// GLuint					ElementArrayBuffer;  /* it is same with Element-array BufferBind Point */
	};

	// Render Context
	struct FRenderContext
	{
		// viewport
		FIntRect					ScissorRect;
		GLboolean					bEnableScissorTest;
		FViewportBox				ViewportBox;

		FRHIOpenGLRasterizerStateRef	RasterizerState;
		FRHIOpenGLSamplerStateRef		TextureSamplers[MaxTextureUnits];
		
		FRHIOpenGLDepthStencilStateRef	DepthStencilState;
		GLint							StencilRef;

		FRHIOpenGLBlendStateRef		BlendState;
		FOpenGLBlendStateData		BlendStateCache;
		FLinearColor				BlendColor;

		GLuint						SharedVAO;
		GLuint						BufferBinds[MaxBufferBinds];

		// Vertex Input Attributes
		FVertexArrayObjectState		VAOState;

		// GPU Program
		FRHIOpenGLGPUProgramRef		GPUProgram;
	};

	// Pending States Set to execute
	struct FPendingStatesSet
	{
		FPendingStatesSet() {}

		FRHIOpenGLRasterizerStateRef	RasterizerState;
		FRHIOpenGLSamplerStateRef		TextureSamplers[MaxTextureUnits];

		FRHIOpenGLDepthStencilStateRef	DepthStencilState;
		GLint							StencilRef;

		FRHIOpenGLBlendStateRef			BlendState;
		FLinearColor					BlendColor;

		// Vertex Inputs
		FRHIOpenGLVertexDeclarationRef	VertexDecl;
		FRHIOpenGLVertexBufferRef		VertexStreams[MaxVertexStreamSources];
		GLboolean						VertexDeclDirty;
		GLboolean						VertexStreamsDirty;
	};

protected:
	FOutputDevice *Logger;

    class FPlatformOpenGLContext PlatformGLContext;
	std::vector<class FRHIOpenGLViewport*>	Viewports;
	class FRHIOpenGLViewport *ViewportDrawing;
    
	FRenderContext			RenderContext;
	FPendingStatesSet		PendingStatesSet;

	std::map<FSamplerStateInitializerRHI, FRHISamplerStateRef> SamplerStateCache;
	std::map<FRasterizerStateInitializerRHI, FRHIRasterizerStateRef> RasterizerStateCache;
	std::map<FDepthStencilStateInitializerRHI, FRHIDepthStencilStateRef> DepthStencilStateCache;
	std::map<FBlendStateInitializerRHI, FRHIBlendStateRef> BlendStateCache;

	//capabilities
	GLint		cap_MajorVersion;
	GLint		cap_MinorVersion;
	GLint		cap_GL_MAX_TEXTURE_IMAGE_UNITS;
	GLint		cap_GL_MAX_DRAW_BUFFERS;
	GLint		cap_GL_MAX_ELEMENTS_VERTICES;
	GLint		cap_GL_MAX_ELEMENTS_INDICES;
};

#endif //__JETX_OPENGL_RENDERER_H__
