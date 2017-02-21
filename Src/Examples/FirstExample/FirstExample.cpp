// \brief
//		First Example Implementation.
//

#include <cassert>
#include "Foundation\JetX.h"
#include "..\AppFramework\AppWindows.h"
#include "Renderer\Renderer.h"


//First Example App
class FFirstExampleApp : public FPlatformApp
{
public:
	virtual void Init() override
	{
		FPlatformApp::Init();

		LogConsole = new FOutputConsole();
		assert(LogConsole);

		MainWindow = CreateWindowX("First Example", 500, 500);
		assert(MainWindow.IsValidRef());

		bWasFullscreen = false;

		GraphicRender = FRenderer::CreateRender(RT_OpenGL);
		assert(GraphicRender);
		
		GraphicRender->Init(LogConsole);
		GraphicRender->DumpCapabilities();

		Viewport = GraphicRender->RHICreateViewport(MainWindow->GetNativeHandle(), 500, 500, false);
		assert(Viewport.IsValidRef());

	}

	virtual void Shutdown() override
	{
		Viewport = nullptr;
		GraphicRender->Shutdown();
		delete GraphicRender;

		delete LogConsole;
	}

	virtual void OnTick() override
	{
		assert(GraphicRender);

		GraphicRender->RHIBeginFrame();
		GraphicRender->RHIBeginDrawingViewport(Viewport);

		GraphicRender->RHISetViewport(0, 0, 500, 500, 0.f, 1.f);
		GraphicRender->RHIClear(true, FLinearColor(1.f,0.f,0.f,1.f), false, 0.f, false, 0);

		GraphicRender->RHIEndDrawingViewport(Viewport, true, false);
		GraphicRender->RHIEndFrame();
	}

	virtual void OnKeyDown(FWindow *InWin, EKeyboard::Enum InKey, int32_t InModifier)
	{
		if (InKey == EKeyboard::KeyF)
		{
			bWasFullscreen = !bWasFullscreen;
			SetWindowSize(InWin, 800, 600, false);
		}
	}

	virtual void OnCloseWindow(FWindow *InWin) override
	{
		if (InWin == MainWindow.DeRef())
		{
			RequestQuit();
		}
	}

protected:
	FWindowRef	MainWindow;
	bool bWasFullscreen;

	FRenderer	*GraphicRender;
	FRHIViewportRef	Viewport;

	FOutputConsole *LogConsole;
};

IMPLEMENT_APP_INSTANCE(FFirstExampleApp);
