// \brief
//		Windows Platform App Implementation.
//

#include <Windows.h>
#include <stdlib.h>
#include <cassert>
#include "AppWindows.h"


// key maps
struct TranslateKeyModifiers
{
	int32_t		 m_vk;
	EKeyModifier::Enum m_modifier;
};

static const TranslateKeyModifiers s_translateKeyModifiers[8] =
{
	{ VK_LMENU,    EKeyModifier::LeftAlt },
	{ VK_RMENU,    EKeyModifier::RightAlt },
	{ VK_LCONTROL, EKeyModifier::LeftCtrl },
	{ VK_RCONTROL, EKeyModifier::RightCtrl },
	{ VK_LSHIFT,   EKeyModifier::LeftShift },
	{ VK_RSHIFT,   EKeyModifier::RightShift },
	{ VK_LWIN,     EKeyModifier::LeftMeta },
	{ VK_RWIN,     EKeyModifier::RightMeta },
};

static int32_t s_translateKey[256];
class FGlobalInitContext
{
public:
	FGlobalInitContext()
	{
		s_translateKey[VK_ESCAPE] = EKeyboard::Esc;
		s_translateKey[VK_RETURN] = EKeyboard::Return;
		s_translateKey[VK_TAB] = EKeyboard::Tab;
		s_translateKey[VK_BACK] = EKeyboard::Backspace;
		s_translateKey[VK_SPACE] = EKeyboard::Space;
		s_translateKey[VK_UP] = EKeyboard::Up;
		s_translateKey[VK_DOWN] = EKeyboard::Down;
		s_translateKey[VK_LEFT] = EKeyboard::Left;
		s_translateKey[VK_RIGHT] = EKeyboard::Right;
		s_translateKey[VK_INSERT] = EKeyboard::Insert;
		s_translateKey[VK_DELETE] = EKeyboard::Delete;
		s_translateKey[VK_HOME] = EKeyboard::Home;
		s_translateKey[VK_END] = EKeyboard::End;
		s_translateKey[VK_PRIOR] = EKeyboard::PageUp;
		s_translateKey[VK_NEXT] = EKeyboard::PageDown;
		s_translateKey[VK_SNAPSHOT] = EKeyboard::Print;
		s_translateKey[VK_OEM_PLUS] = EKeyboard::Plus;
		s_translateKey[VK_OEM_MINUS] = EKeyboard::Minus;
		s_translateKey[VK_OEM_4] = EKeyboard::LeftBracket;
		s_translateKey[VK_OEM_6] = EKeyboard::RightBracket;
		s_translateKey[VK_OEM_1] = EKeyboard::Semicolon;
		s_translateKey[VK_OEM_7] = EKeyboard::Quote;
		s_translateKey[VK_OEM_COMMA] = EKeyboard::Comma;
		s_translateKey[VK_OEM_PERIOD] = EKeyboard::Period;
		s_translateKey[VK_DECIMAL] = EKeyboard::Period;
		s_translateKey[VK_OEM_2] = EKeyboard::Slash;
		s_translateKey[VK_OEM_5] = EKeyboard::Backslash;
		s_translateKey[VK_OEM_3] = EKeyboard::Tilde;
		s_translateKey[VK_F1] = EKeyboard::F1;
		s_translateKey[VK_F2] = EKeyboard::F2;
		s_translateKey[VK_F3] = EKeyboard::F3;
		s_translateKey[VK_F4] = EKeyboard::F4;
		s_translateKey[VK_F5] = EKeyboard::F5;
		s_translateKey[VK_F6] = EKeyboard::F6;
		s_translateKey[VK_F7] = EKeyboard::F7;
		s_translateKey[VK_F8] = EKeyboard::F8;
		s_translateKey[VK_F9] = EKeyboard::F9;
		s_translateKey[VK_F10] = EKeyboard::F10;
		s_translateKey[VK_F11] = EKeyboard::F11;
		s_translateKey[VK_F12] = EKeyboard::F12;
		s_translateKey[VK_NUMPAD0] = EKeyboard::NumPad0;
		s_translateKey[VK_NUMPAD1] = EKeyboard::NumPad1;
		s_translateKey[VK_NUMPAD2] = EKeyboard::NumPad2;
		s_translateKey[VK_NUMPAD3] = EKeyboard::NumPad3;
		s_translateKey[VK_NUMPAD4] = EKeyboard::NumPad4;
		s_translateKey[VK_NUMPAD5] = EKeyboard::NumPad5;
		s_translateKey[VK_NUMPAD6] = EKeyboard::NumPad6;
		s_translateKey[VK_NUMPAD7] = EKeyboard::NumPad7;
		s_translateKey[VK_NUMPAD8] = EKeyboard::NumPad8;
		s_translateKey[VK_NUMPAD9] = EKeyboard::NumPad9;
		s_translateKey[uint8_t('0')] = EKeyboard::Key0;
		s_translateKey[uint8_t('1')] = EKeyboard::Key1;
		s_translateKey[uint8_t('2')] = EKeyboard::Key2;
		s_translateKey[uint8_t('3')] = EKeyboard::Key3;
		s_translateKey[uint8_t('4')] = EKeyboard::Key4;
		s_translateKey[uint8_t('5')] = EKeyboard::Key5;
		s_translateKey[uint8_t('6')] = EKeyboard::Key6;
		s_translateKey[uint8_t('7')] = EKeyboard::Key7;
		s_translateKey[uint8_t('8')] = EKeyboard::Key8;
		s_translateKey[uint8_t('9')] = EKeyboard::Key9;
		s_translateKey[uint8_t('A')] = EKeyboard::KeyA;
		s_translateKey[uint8_t('B')] = EKeyboard::KeyB;
		s_translateKey[uint8_t('C')] = EKeyboard::KeyC;
		s_translateKey[uint8_t('D')] = EKeyboard::KeyD;
		s_translateKey[uint8_t('E')] = EKeyboard::KeyE;
		s_translateKey[uint8_t('F')] = EKeyboard::KeyF;
		s_translateKey[uint8_t('G')] = EKeyboard::KeyG;
		s_translateKey[uint8_t('H')] = EKeyboard::KeyH;
		s_translateKey[uint8_t('I')] = EKeyboard::KeyI;
		s_translateKey[uint8_t('J')] = EKeyboard::KeyJ;
		s_translateKey[uint8_t('K')] = EKeyboard::KeyK;
		s_translateKey[uint8_t('L')] = EKeyboard::KeyL;
		s_translateKey[uint8_t('M')] = EKeyboard::KeyM;
		s_translateKey[uint8_t('N')] = EKeyboard::KeyN;
		s_translateKey[uint8_t('O')] = EKeyboard::KeyO;
		s_translateKey[uint8_t('P')] = EKeyboard::KeyP;
		s_translateKey[uint8_t('Q')] = EKeyboard::KeyQ;
		s_translateKey[uint8_t('R')] = EKeyboard::KeyR;
		s_translateKey[uint8_t('S')] = EKeyboard::KeyS;
		s_translateKey[uint8_t('T')] = EKeyboard::KeyT;
		s_translateKey[uint8_t('U')] = EKeyboard::KeyU;
		s_translateKey[uint8_t('V')] = EKeyboard::KeyV;
		s_translateKey[uint8_t('W')] = EKeyboard::KeyW;
		s_translateKey[uint8_t('X')] = EKeyboard::KeyX;
		s_translateKey[uint8_t('Y')] = EKeyboard::KeyY;
		s_translateKey[uint8_t('Z')] = EKeyboard::KeyZ;
	}
};
static FGlobalInitContext s_dummy;


static EKeyboard::Enum translateKey(uint16_t InVK)
{
	return (EKeyboard::Enum)s_translateKey[InVK];
}

// get current modifier key
static int32_t GetTranslateKeyModifiers()
{
	int32_t modifiers = 0;
	for (uint32_t ii = 0; ii < STATIC_ARRAY_COUNT(s_translateKeyModifiers); ++ii)
	{
		const TranslateKeyModifiers& tkm = s_translateKeyModifiers[ii];
		modifiers |= ::GetKeyState(tkm.m_vk) > 0 ? tkm.m_modifier : EKeyModifier::None;
	}
	return modifiers;
}

static const char* lpszClassName = "WinClass_AppWindows";
static FAppWindows* TheApp = nullptr;

FAppWindows::FAppWindows()
{
	TheApp = this;
}

FAppWindows::~FAppWindows()
{
	TheApp = nullptr;
}

void FAppWindows::Init()
{
	// register window class
	hInstance = (HINSTANCE)GetModuleHandle(NULL);

	WNDCLASSEXA WndCls;
	memset(&WndCls, 0, sizeof(WndCls));
	WndCls.cbSize = sizeof(WndCls);
	WndCls.style = CS_HREDRAW | CS_VREDRAW;
	WndCls.lpfnWndProc = AppWindowProc;
	WndCls.hInstance = hInstance;
	WndCls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndCls.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndCls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndCls.lpszClassName = lpszClassName;
	WndCls.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	::RegisterClassExA(&WndCls);
}

void FAppWindows::Shutdown()
{

}

void FAppWindows::RunLoop()
{
	MSG msg;
	bool bDone = false;

	while (!bDone)
	{
		if (::PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
		else
		{
			OnTick();
		}
	}
}

// request quit app
void FAppWindows::RequestQuit()
{
	::PostQuitMessage(0);
}

void FAppWindows::RemoveFromWindowsMap(FWinWindow *InWin)
{
	assert(InWin);
	std::map<HWND, FWinWindowRef>::iterator Itr = WindowsMap.find(InWin->hWnd);
	if (Itr != WindowsMap.end())
	{
		WindowsMap.erase(Itr);
	}
}

// create a window
FWindow* FAppWindows::CreateWindowX(const char* InTitle, uint32_t InWidth, uint32_t InHeight)
{
	DWORD Style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	RECT rect;
	rect.left = rect.top = 0;
	rect.right = InWidth;
	rect.bottom = InHeight;
	::AdjustWindowRect(&rect, Style, FALSE);

	HWND hWnd = ::CreateWindowA(lpszClassName, InTitle
		, Style
		, 0
		, 0
		, (rect.right - rect.left)
		, (rect.bottom - rect.top)
		, NULL
		, NULL
		, hInstance
		, 0);

	::ShowWindow(hWnd, SW_NORMAL);
	::UpdateWindow(hWnd);

	FWinWindow *pWin = new FWinWindow();
	assert(pWin);
	pWin->hWnd = hWnd;

	WindowsMap.insert(std::make_pair(hWnd, FWinWindowRef(pWin)));
	return pWin;
}

void FAppWindows::DestroyWindowX(FWindow *InWin)
{
	FWinWindow *pWinWindow = dynamic_cast<FWinWindow*>(InWin);
	assert(pWinWindow);

	::DestroyWindow(pWinWindow->hWnd);

	// erase it
	RemoveFromWindowsMap(pWinWindow);
	delete InWin;
}

// mouse capture
void FAppWindows::CaptureMouse(FWindow *InWin)
{
	FWinWindow *pWinWindow = dynamic_cast<FWinWindow*>(InWin);
	assert(pWinWindow);

	if (!pWinWindow->bCaptured)
	{
		::SetCapture(pWinWindow->hWnd);
		pWinWindow->bCaptured = true;
	}
}

void FAppWindows::ReleaseMouse(FWindow *InWin)
{
	FWinWindow *pWinWindow = dynamic_cast<FWinWindow*>(InWin);
	assert(pWinWindow);

	if (pWinWindow->bCaptured)
	{
		::ReleaseCapture();
		pWinWindow->bCaptured = false;
	}
}

// change the window size
void FAppWindows::SetWindowSize(FWindow *InWin, uint32_t InNewWidth, uint32_t InNewHeight, bool InbFullscreen)
{
	FWinWindow *pWinWindow = dynamic_cast<FWinWindow*>(InWin);
	assert(pWinWindow);

	uint32_t WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	uint32_t WindowStyleEx = 0;
	HWND InsertAfter = HWND_NOTOPMOST;

	if (InbFullscreen)
	{
		// Get the monitor info from the window handle.
		HMONITOR hMonitor = MonitorFromWindow(pWinWindow->hWnd, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFOEX MonitorInfo;
		memset(&MonitorInfo, 0, sizeof(MONITORINFOEX));
		MonitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &MonitorInfo);

		DEVMODE Mode;
		Mode.dmSize = sizeof(DEVMODE);
		Mode.dmBitsPerPel = 32;
		Mode.dmPelsWidth = InNewWidth;
		Mode.dmPelsHeight = InNewHeight;
		Mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Turn on fullscreen mode for the current monitor
		ChangeDisplaySettingsEx(MonitorInfo.szDevice, &Mode, NULL, CDS_FULLSCREEN, NULL);

		WindowStyle = WS_POPUP;
		WindowStyleEx = WS_EX_APPWINDOW | WS_EX_TOPMOST;
		InsertAfter = HWND_TOPMOST;

		pWinWindow->bWasFullscreen = true;
	}
	else if (pWinWindow->bWasFullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
		pWinWindow->bWasFullscreen = false;
	}

	RECT rect;
	int X, Y, Width, Height;

	::GetWindowRect(pWinWindow->hWnd, &rect);
	X = rect.left;
	Y = rect.top;

	rect.left = 0;
	rect.top = 0;
	rect.right = InNewWidth;
	rect.bottom = InNewHeight;
	::AdjustWindowRectEx(&rect, WindowStyle, FALSE, WindowStyleEx);

	::SetWindowLongA(pWinWindow->hWnd, GWL_STYLE, WindowStyle);
	::SetWindowLongA(pWinWindow->hWnd, GWL_EXSTYLE, WindowStyleEx);

	Width = rect.right - rect.left;
	Height = rect.bottom - rect.top;
	//::MoveWindow(pWinWindow->hWnd, 0, 0, Width, Height, TRUE);
	::SetWindowPos(pWinWindow->hWnd, InsertAfter, X, Y, Width, Height, 0);
}

// window message
void FAppWindows::OnCloseWindow(FWindow *InWin)
{
	DestroyWindowX(InWin);
}

void FAppWindows::OnSizeChanged(FWindow *InWin, uint32_t InNewWidth, uint32_t InNewHeight)
{

}

void FAppWindows::OnKeyDown(FWindow *InWin, EKeyboard::Enum InKey, int32_t InModifier)
{

}

void FAppWindows::OnKeyUp(FWindow *InWin, EKeyboard::Enum InKey, int32_t InModifier)
{

}

void FAppWindows::OnMouseDown(FWindow *InWin, EMouseButton::Enum InMouse, int32_t InX, int32_t InY)
{

}

void FAppWindows::OnMouseUp(FWindow *InWin, EMouseButton::Enum InMouse, int32_t InX, int32_t InY)
{

}

void FAppWindows::OnMouseMove(FWindow *InWin, int32_t InX, int32_t InY)
{

}

void FAppWindows::OnMouseWheel(FWindow *InWin, float InDelta, int32_t InX, int32_t InY, int32_t InModifier)
{

}

void FAppWindows::OnTick()
{

}

LRESULT CALLBACK FAppWindows::AppWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	FWinWindow *pWindow = nullptr;

	if (TheApp)
	{
		std::map<HWND, FWinWindowRef>::iterator Found = TheApp->WindowsMap.find(hWnd);
		if (Found != TheApp->WindowsMap.end())
		{
			pWindow = Found->second.DeRef();
		}
	}

	switch (Msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_CLOSE:
	{
		TheApp->OnCloseWindow(pWindow);
		// Don't process message. Window will be destroyed later.
		return 0;
	}
	break;
	case WM_SIZE:
	{
		uint32_t width = LOWORD(lParam);
		uint32_t height = HIWORD(lParam);

		TheApp->OnSizeChanged(pWindow, width, height);
	}
	break;
	case WM_MOUSEMOVE:
	{
		int32_t mx = LOWORD(lParam);
		int32_t my = HIWORD(lParam);

		TheApp->OnMouseMove(pWindow, mx, my);
	}
	break;

	case WM_MOUSEWHEEL:
	{
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };

		::ScreenToClient(hWnd, &pt);
		int32_t mx = pt.x;
		int32_t my = pt.y;

		float DeltZ = (float)(GET_WHEEL_DELTA_WPARAM(wParam)) / (float)(WHEEL_DELTA);
		int32_t Modifier = GetTranslateKeyModifiers();

		TheApp->OnMouseWheel(pWindow, DeltZ, mx, my, Modifier);
	}
	break;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	{
		int32_t mx = LOWORD(lParam);
		int32_t my = HIWORD(lParam);

		EMouseButton::Enum mouse;
		if (Msg == WM_LBUTTONDOWN)
		{
			mouse = EMouseButton::Left;
		}
		else if (Msg == WM_MBUTTONDOWN)
		{
			mouse = EMouseButton::Middle;
		}
		else
		{
			mouse = EMouseButton::Right;
		}
		TheApp->OnMouseDown(pWindow, mouse, mx, my);
	}
	break;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	{
		int32_t mx = LOWORD(lParam);
		int32_t my = HIWORD(lParam);

		EMouseButton::Enum mouse;
		if (Msg == WM_LBUTTONUP)
		{
			mouse = EMouseButton::Left;
		}
		else if (Msg == WM_MBUTTONUP)
		{
			mouse = EMouseButton::Middle;
		}
		else
		{
			mouse = EMouseButton::Right;
		}
		TheApp->OnMouseUp(pWindow, mouse, mx, my);
	}
	break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		int32_t Modifiers = GetTranslateKeyModifiers();
		EKeyboard::Enum Key = translateKey(wParam);

		TheApp->OnKeyDown(pWindow, Key, Modifiers);
	}
	break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		int32_t Modifiers = GetTranslateKeyModifiers();
		EKeyboard::Enum Key = translateKey(wParam);

		TheApp->OnKeyUp(pWindow, Key, Modifiers);
	}
	break;
	default:
		break;
	}

	return ::DefWindowProcA(hWnd, Msg, wParam, lParam);
}
