// \brief
//		First Example Implementation.
//

#include <cassert>
#include "Foundation/JetX.h"
#include "AppFramework/Application.h"
#include "Renderer/Renderer.h"

// Shaders
const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec4 vcolor;\n"
		"out vec4 color_vt;\n"
		"void main()\n"
		"{\n"
		"     if(gl_InstanceID == 1) \n"
		"         gl_Position = vec4(position.x+0.5, position.y+0.5, position.z, 1.0); \n"
		"     else \n"
		"         gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
		"     color_vt = vcolor; \n"
		"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 color;\n"
		"in vec4 color_vt;\n"
		"void main()\n"
		"{\n"
		"color = color_vt;\n"
		"}\n\0";

float vertices[] = {
	0.5f,  0.5f, 0.0f,  // Top Right
	0.5f, -0.5f, 0.0f,  // Bottom Right
	-0.5f, -0.5f, 0.0f,  // Bottom Left
	-0.5f,  0.5f, 0.0f   // Top Left 
};
float colors[] = {
	1.f, 0.f, 0.f, 1.f,
	0.f, 1.f, 0.f, 1.f
};

uint32_t indices[] = {  // Note that we start from 0!
	3, 1, 0,  // First Triangle
	3, 2, 1   // Second Triangle
};

//First Example App
class FFirstExampleApp : public FApplication, public IWindowClient
{
public:
	virtual bool Init() override
	{
		FApplication::Init();

		LogConsole = new FOutputConsole();
		assert(LogConsole);

		MainWindow = new FWindow("First Example", 500, 500);
		assert(MainWindow.IsValidRef());
		MainWindow->SetWindowClient(this);

		bWasFullscreen = false;

		GraphicRender = FRenderer::CreateRender(RT_OpenGL);
		assert(GraphicRender);
		
		GraphicRender->Init(LogConsole);
		GraphicRender->DumpCapabilities();

		Viewport = GraphicRender->RHICreateViewport(MainWindow->GetNativeHandle(), 500, 500, false);
		assert(Viewport.IsValidRef());

		// Create Resource
		FRHIVertexShaderRef VertexShader = GraphicRender->RHICreateVertexShader(vertexShaderSource);
		FRHIPixelShaderRef  PixelShader = GraphicRender->RHICreatePixelShader(fragmentShaderSource);

		assert(VertexShader.IsValidRef());
		assert(PixelShader.IsValidRef());
		VertexShader->Dump(*LogConsole);
		PixelShader->Dump(*LogConsole);

		GpuProgram = GraphicRender->RHICreateGPUProgram(VertexShader, PixelShader);
		assert(GpuProgram.IsValidRef());
		GpuProgram->Dump(*LogConsole);

		VertexBuffer = GraphicRender->RHICreateVertexBuffer(sizeof(vertices), vertices, BA_Static, BU_Draw);
		ColorBuffer = GraphicRender->RHICreateVertexBuffer(sizeof(colors), colors, BA_Static, BU_Draw);
		IndexBuffer = GraphicRender->RHICreateIndexBuffer(sizeof(indices), indices, 4, BA_Static, BU_Draw);

		// input layout
		// uint16_t InStreamIndex, uint16_t InAttriIndex, uint16_t InOffset, uint16_t InStride, uint16_t InDivisor, EVertexElementType InType
		FVertexElement Inputs[2];
		Inputs[0] = FVertexElement(0, 0, 0, sizeof(float) * 3, 0, VET_Float3);
		Inputs[1] = FVertexElement(1, 1, 0, sizeof(float) * 4, 1, VET_Float4);
		InputLayout = GraphicRender->RHICreateVertexInputLayout(Inputs, 2);
		assert(InputLayout);

		return true;
	}

	virtual void Shutdown() override
	{
		Viewport = nullptr;
		GraphicRender->Shutdown();
		delete GraphicRender;
		delete LogConsole;
		MainWindow.SafeRelease();

		FApplication::Shutdown();
	}

	virtual void OnTick(float InDeltaSeconds) override
	{
		int32_t Width, Height;
		assert(GraphicRender);

		GraphicRender->RHIBeginFrame();
		GraphicRender->RHIBeginDrawingViewport(Viewport);

		MainWindow->GetRenderAreaSize(Width, Height);
		GraphicRender->RHISetViewport(0, 0, Width, Height, 0.f, 1.f);
		GraphicRender->RHIClear(true, FLinearColor(0.f,0.f,0.f,1.f), true, 1.f, false, 0);

		// setup resource
		GraphicRender->SetVertexStreamSource(0, VertexBuffer);
		GraphicRender->SetVertexStreamSource(1, ColorBuffer);
		GraphicRender->SetVertexInputLayout(InputLayout);
		GraphicRender->SetGPUProgram(GpuProgram);
		GraphicRender->DrawIndexedPrimitiveInstanced(IndexBuffer, PT_Triangles, 0, IndexBuffer->GetIndexCount(), 2);

		GraphicRender->RHIEndDrawingViewport(Viewport, true, false);
		GraphicRender->RHIEndFrame();
	}

	virtual void OnKeyDown(FWindow &InWin, int32_t InKey, int32_t InScancode, int32_t InModifier) override
	{
		switch (InKey)
		{
		case GLFW_KEY_ESCAPE:
			RequestQuit(); break;
		case GLFW_KEY_C:
			InWin.CloseWindow(); break;
		default:
			break;
		}
	}

	virtual void OnCloseRequest(FWindow &InWin) override
	{
		InWin.DestroyWindow();
		MainWindow.SafeRelease();

		RequestQuit();
	}

protected:
	FWindowRef	MainWindow;
	bool bWasFullscreen;

	FRenderer	*GraphicRender;
	FRHIViewportRef	Viewport;
	FRHIGPUProgramRef GpuProgram;
	FRHIVertexBufferRef VertexBuffer;
	FRHIVertexBufferRef ColorBuffer;
	FRHIIndexBufferRef IndexBuffer;
	FRHIVertexDeclarationRef InputLayout;

	FOutputConsole *LogConsole;
};

IMPLEMENT_APP_INSTANCE(FFirstExampleApp);
