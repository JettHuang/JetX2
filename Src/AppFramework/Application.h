// \brief
//		A application framework.  base on GLFW.
//
//

#ifndef __JETX_APPLICATION_H__
#define __JETX_APPLICATION_H__

#include "Foundation/JetX.h"
#include "Foundation/RefCounting.h"

#ifdef XPLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif
#ifdef XPLATFORM_MACOSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include "glfw3.h"
#include "glfw3native.h"


class IWindowClient;

//FWindow
class FWindow : public FRefCountedObject
{
public:
	FWindow();
	FWindow(const char *InTitle, int32_t InWidth, int32_t InHeight, bool bFullScreen=false);
	virtual ~FWindow();
	
	void SetWindowClient(IWindowClient *InClient);

	void SetWindowTitle(const char *InTitle);
	void SetWindowSize(int32_t InWidth, int32_t InHeight, bool bFullScreen);
	void SetWindowPosition(int32_t InPosX, int32_t InPosY);
	void ShowWindow(bool InbShow);
	void CloseWindow();
	void DestroyWindow();
	void LockCursor(bool InbLock);

	void GetWindowSize(int32_t &Width, int32_t &Height);
	void GetRenderAreaSize(int32_t &Width, int32_t &Height);
	bool IsWindowClosed();

	void* GetNativeHandle();

	// event handler
	virtual void OnCloseRequest();

	virtual void OnPosChanged(int32_t InPosX, int32_t InPosY);
	virtual void OnSizeChanged(int32_t InWidth, int32_t InHeight);
	virtual void OnRenderAreaSizeChanged(int32_t InWidth, int32_t InHeight);

	virtual void OnFocus(bool InbFocused);

	virtual void OnKeyDown(int32_t InKey, int32_t InScancode, int32_t InModifier);
	virtual void OnKeyUp(int32_t InKey, int32_t InScancode, int32_t InModifier);
	virtual void OnChar(uint32_t InUnicodepoint);
	
	virtual void OnMouseDown(int32_t InButton, int32_t InModifier);
	virtual void OnMouseUp(int32_t InButton, int32_t InModifier);
	virtual void OnMouseMove(double InX, double InY);
	virtual void OnMouseScroll(double InOffsetX, double InOffsetY);

protected:
	void Init(const char *InTitle, int32_t InWidth, int32_t InHeight, bool bFullScreen = false);

	GLFWwindow	*glfwWindow;
	IWindowClient *Client;
};

typedef TRefCountPtr<FWindow>	FWindowRef;

// Window Client Interface
class IWindowClient
{
public:
	virtual void OnCloseRequest(FWindow &InWin) {}

	virtual void OnPosChanged(FWindow &InWin, int32_t InPosX, int32_t InPosY) {}
	virtual void OnSizeChanged(FWindow &InWin, int32_t InWidth, int32_t InHeight) {}
	virtual void OnRenderAreaSizeChanged(FWindow &InWin, int32_t InWidth, int32_t InHeight) {}

	virtual void OnFocus(FWindow &InWin, bool InbFocused) {}

	virtual void OnKeyDown(FWindow &InWin, int32_t InKey, int32_t InScancode, int32_t InModifier) {}
	virtual void OnKeyUp(FWindow &InWin, int32_t InKey, int32_t InScancode, int32_t InModifier) {}
	virtual void OnChar(FWindow &InWin, uint32_t InUnicodepoint) {}

	virtual void OnMouseDown(FWindow &InWin, int32_t InButton, int32_t InModifier) {}
	virtual void OnMouseUp(FWindow &InWin, int32_t InButton, int32_t InModifier) {}
	virtual void OnMouseMove(FWindow &InWin, double InX, double InY) {}
	virtual void OnMouseScroll(FWindow &InWin, double InOffsetX, double InOffsetY) {}
};

//Application
class FApplication
{
public:
	FApplication() 
		: bReqQuit(false)
	{}

	virtual ~FApplication() {}

	virtual bool Init();
	virtual void Shutdown();
	virtual void RunLoop();
	virtual void OnTick(float InDeltaSeconds);

	// request quit app
	virtual void RequestQuit();

protected:
	bool	bReqQuit;
};


#define IMPLEMENT_APP_INSTANCE(AppClass)	\
	int main(int argc, char* argv[])		\
	{										\
		AppClass theApp;					\
											\
		theApp.Init();						\
		theApp.RunLoop();					\
		theApp.Shutdown();					\
		return 0;							\
	}

	
#endif //__JETX_APPLICATION_H__
