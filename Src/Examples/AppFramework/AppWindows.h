//\brief
//		Windows Platform App Implementation.
//

#ifndef __JETX_APP_WINDOWS_H__
#define __JETX_APP_WINDOWS_H__

#include <map>
#include "AppInterface.h"
#include <Windows.h>


// Window
class FWinWindow : public FWindow
{
public:
	FWinWindow()
		: hWnd(0)
		, bCaptured(false)
		, bWasFullscreen(false)
	{}

	virtual void* GetNativeHandle()override
	{
		return (void*)hWnd;
	}

	HWND		hWnd;
	bool		bCaptured;
	bool		bWasFullscreen;
};

typedef TRefCountPtr<FWinWindow>	FWinWindowRef;

// FAppWindows
class FAppWindows : public FAppInterface
{
public:
	FAppWindows();
	virtual ~FAppWindows();

	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void RunLoop() override;
	virtual void OnTick() override;

	// request quit app
	virtual void RequestQuit() override;

	// create a window
	virtual FWindow* CreateWindowX(const char* InTitle, uint32_t InWidth, uint32_t InHeight) override;
	virtual void DestroyWindowX(FWindow *InWin) override;

	// mouse capture
	virtual void CaptureMouse(FWindow *InWin) override;
	virtual void ReleaseMouse(FWindow *InWin) override;

	// change the window size
	virtual void SetWindowSize(FWindow *InWin, uint32_t InNewWidth, uint32_t InNewHeight, bool InbFullscreen) override;

	// window message
	virtual void OnCloseWindow(FWindow *InWin) override;
	virtual void OnSizeChanged(FWindow *InWin, uint32_t InNewWidth, uint32_t InNewHeight) override;
	virtual void OnKeyDown(FWindow *InWin, EKeyboard::Enum InKey, int32_t InModifier) override;
	virtual void OnKeyUp(FWindow *InWin, EKeyboard::Enum InKey, int32_t InModifier) override;
	virtual void OnMouseDown(FWindow *InWin, EMouseButton::Enum InMouse, int32_t InX, int32_t InY) override;
	virtual void OnMouseUp(FWindow *InWin, EMouseButton::Enum InMouse, int32_t InX, int32_t InY) override;
	virtual void OnMouseMove(FWindow *InWin, int32_t InX, int32_t InY) override;
	virtual void OnMouseWheel(FWindow *InWin, float InDelta, int32_t InX, int32_t InY, int32_t InModifier) override;

protected:
	void RemoveFromWindowsMap(FWinWindow *InWin);
	static LRESULT CALLBACK AppWindowProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
protected:
	std::map<HWND, FWinWindowRef>		WindowsMap;
	HINSTANCE	hInstance;
};

typedef FAppWindows		FPlatformApp;


#endif // __JETX_APP_WINDOWS_H__
