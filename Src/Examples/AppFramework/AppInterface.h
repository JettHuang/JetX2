// \brief
//		A application framework interface.
//

#ifndef __JETX_APP_INTERFACE_H__
#define __JETX_APP_INTERFACE_H__

#include "Foundation/JetX.h"
#include "Foundation/RefCounting.h"


// Input Value Enum

struct EMouseButton
{
	enum Enum 
	{
		None,
		Left,
		Middle,
		Right,

		Count
	};
};


struct EGamepadAxis
{
	enum Enum
	{
		LeftX,
		LeftY,
		LeftZ,
		RightX,
		RightY,
		RightZ,

		Count
	};
};

struct EKeyModifier
{
	enum Enum
	{
		None = 0,
		LeftAlt = 0x01,
		RightAlt = 0x02,
		LeftCtrl = 0x04,
		RightCtrl = 0x08,
		LeftShift = 0x10,
		RightShift = 0x20,
		LeftMeta = 0x40,
		RightMeta = 0x80
	};
};

struct EKeyboard
{
	enum Enum
	{
		None = 0,
		Esc,
		Return,
		Tab,
		Space,
		Backspace,
		Up,
		Down,
		Left,
		Right,
		Insert,
		Delete,
		Home,
		End,
		PageUp,
		PageDown,
		Print,
		Plus,
		Minus,
		LeftBracket,
		RightBracket,
		Semicolon,
		Quote,
		Comma,
		Period,
		Slash,
		Backslash,
		Tilde,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		NumPad0,
		NumPad1,
		NumPad2,
		NumPad3,
		NumPad4,
		NumPad5,
		NumPad6,
		NumPad7,
		NumPad8,
		NumPad9,
		Key0,
		Key1,
		Key2,
		Key3,
		Key4,
		Key5,
		Key6,
		Key7,
		Key8,
		Key9,
		KeyA,
		KeyB,
		KeyC,
		KeyD,
		KeyE,
		KeyF,
		KeyG,
		KeyH,
		KeyI,
		KeyJ,
		KeyK,
		KeyL,
		KeyM,
		KeyN,
		KeyO,
		KeyP,
		KeyQ,
		KeyR,
		KeyS,
		KeyT,
		KeyU,
		KeyV,
		KeyW,
		KeyX,
		KeyY,
		KeyZ,

		GamepadA,
		GamepadB,
		GamepadX,
		GamepadY,
		GamepadThumbL,
		GamepadThumbR,
		GamepadShoulderL,
		GamepadShoulderR,
		GamepadUp,
		GamepadDown,
		GamepadLeft,
		GamepadRight,
		GamepadBack,
		GamepadStart,
		GamepadGuide,

		Count

	};
};

//FWindow
class FWindow : public FRefCountedObject
{
public:
	virtual ~FWindow() {}
	virtual void* GetNativeHandle() { return nullptr; }
};

typedef TRefCountPtr<FWindow>	FWindowRef;

//App interface
class FAppInterface
{
public:
	virtual ~FAppInterface() {}

	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	virtual void RunLoop() = 0;
	virtual void OnTick() = 0;

	// request quit app
	virtual void RequestQuit() = 0;

	// create a window
	virtual FWindow* CreateWindowX(const char* InTitle, uint32_t InWidth, uint32_t InHeight) = 0;
	virtual void DestroyWindowX(FWindow *InWin) = 0;

	// mouse capture
	virtual void CaptureMouse(FWindow *InWin) = 0;
	virtual void ReleaseMouse(FWindow *InWin) = 0;

	// change the window size
	virtual void SetWindowSize(FWindow *InWin, uint32_t InNewWidth, uint32_t InNewHeight, bool InbFullscreen) = 0;
	
	// window message
	virtual void OnCloseWindow(FWindow *InWin) = 0;
	virtual void OnSizeChanged(FWindow *InWin, uint32_t InNewWidth, uint32_t InNewHeight) = 0;
	virtual void OnKeyDown(FWindow *InWin, EKeyboard::Enum InKey, int32_t InModifier) = 0;
	virtual void OnKeyUp(FWindow *InWin, EKeyboard::Enum InKey, int32_t InModifier) = 0;
	virtual void OnMouseDown(FWindow *InWin, EMouseButton::Enum InMouse, int32_t InX, int32_t InY) = 0;
	virtual void OnMouseUp(FWindow *InWin, EMouseButton::Enum InMouse, int32_t InX, int32_t InY) = 0;
	virtual void OnMouseMove(FWindow *InWin, int32_t InX, int32_t InY) = 0;
	virtual void OnMouseWheel(FWindow *InWin, float InDelta, int32_t InX, int32_t InY, int32_t InModifier) = 0;
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

	
#endif //__JETX_APP_INTERFACE_H__
