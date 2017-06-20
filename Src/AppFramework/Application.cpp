//\brief
//		application framework implementation.
//

#include "AppFramework/Application.h"

//////////////////////////////////////////////////////////////////////////
// helper functions
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void window_pos_callback(GLFWwindow* window, int x, int y)
{
	FWindow *WinObj = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(window));
	if (WinObj)
	{
		WinObj->OnPosChanged(x, y);
	}
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
	FWindow *WinObj = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(window));
	if (WinObj)
	{
		WinObj->OnSizeChanged(width, height);
	}
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	FWindow *WinObj = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(window));
	if (WinObj)
	{
		WinObj->OnRenderAreaSizeChanged(width, height);
	}
}

static void window_close_callback(GLFWwindow* window)
{
	FWindow *WinObj = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(window));
	if (WinObj)
	{
		WinObj->OnCloseRequest();
	}
}

static void window_refresh_callback(GLFWwindow* window)
{
	// do nothing
}

static void window_focus_callback(GLFWwindow* window, int focused)
{
	FWindow *WinObj = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(window));
	if (WinObj)
	{
		WinObj->OnFocus(focused != 0);
	}
}

static void window_iconify_callback(GLFWwindow* window, int iconified)
{
	// do nothing
}

static void window_maximize_callback(GLFWwindow* window, int maximized)
{
	// do nothing
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	FWindow *WinObj = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(window));
	if (WinObj)
	{
		if (action == GLFW_PRESS)
		{
			WinObj->OnMouseDown(button, mods);
		}
		else
		{
			WinObj->OnMouseUp(button, mods);
		}
	}
}

static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	FWindow *WinObj = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(window));
	if (WinObj)
	{
		WinObj->OnMouseMove(x, y);
	}
}

static void cursor_enter_callback(GLFWwindow* window, int entered)
{
	// do nothing
}

static void scroll_callback(GLFWwindow* window, double x, double y)
{
	FWindow *WinObj = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(window));
	if (WinObj)
	{
		WinObj->OnMouseScroll(x, y);
	}
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	FWindow *WinObj = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(window));
	if (WinObj)
	{
		switch (action)
		{
		case GLFW_PRESS:
		case GLFW_REPEAT:
			WinObj->OnKeyDown(key, scancode, mods);
			break;
		default:
			WinObj->OnKeyUp(key, scancode, mods);
			break;
		}
	}
}

static void char_callback(GLFWwindow* window, unsigned int codepoint)
{
	FWindow *WinObj = reinterpret_cast<FWindow*>(glfwGetWindowUserPointer(window));
	if (WinObj)
	{
		WinObj->OnChar(codepoint);
	}
}

static void char_mods_callback(GLFWwindow* window, unsigned int codepoint, int mods)
{
	// do nothing
}

static void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	/*
	example:
	for (int i = 0; i < count; i++)
		handle_dropped_file(paths[i]);  // utf-8 encode string.
	*/
}

static void monitor_callback(GLFWmonitor* monitor, int event)
{
	if (event == GLFW_CONNECTED)
	{
		int x, y, widthMM, heightMM;
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwGetMonitorPos(monitor, &x, &y);
		glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);

		printf("%0.3f: Monitor %s (%ix%i at %ix%i, %ix%i mm) was connected\n",
			glfwGetTime(),
			glfwGetMonitorName(monitor),
			mode->width, mode->height,
			x, y,
			widthMM, heightMM);
	}
	else if (event == GLFW_DISCONNECTED)
	{
		printf("%0.3f: Monitor %s was disconnected\n",
			glfwGetTime(),
			glfwGetMonitorName(monitor));
	}
}

static void joystick_callback(int jid, int event)
{
	if (event == GLFW_CONNECTED)
	{
		int axisCount, buttonCount, hatCount;

		glfwGetJoystickAxes(jid, &axisCount);
		glfwGetJoystickButtons(jid, &buttonCount);
		glfwGetJoystickHats(jid, &hatCount);

		printf("%0.3f: Joystick %i (%s) was connected with %i axes, %i buttons, and %i hats\n",
			glfwGetTime(),
			jid,
			glfwGetJoystickName(jid),
			axisCount,
			buttonCount,
			hatCount);
	}
	else
	{
		printf("%0.3f: Joystick %i was disconnected\n", glfwGetTime(), jid);
	}
}


//////////////////////////////////////////////////////////////////////////
// Window
#define DEFAULT_WIN_WIDTH	800
#define DEFAULT_WIN_HEIGHT	600

FWindow::FWindow()
	: Client(nullptr)
{
	Init("FWindow", DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, false);
}

FWindow::FWindow(const char *InTitle, int32_t InWidth, int32_t InHeight, bool bFullScreen)
	: Client(nullptr)
{
	Init(InTitle, InWidth, InHeight, bFullScreen);
}

FWindow::~FWindow() 
{
	DestroyWindow();
}

void FWindow::SetWindowClient(IWindowClient *InClient)
{
	Client = InClient;
}

void FWindow::Init(const char *InTitle, int32_t InWidth, int32_t InHeight, bool bFullScreen)
{
	GLFWmonitor* primary = bFullScreen ? glfwGetPrimaryMonitor() : nullptr;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindow = glfwCreateWindow(InWidth, InHeight, InTitle, primary, nullptr);
	assert(glfwWindow);
	if (glfwWindow)
	{
		glfwSetWindowUserPointer(glfwWindow, this);

		glfwSetWindowPosCallback(glfwWindow, window_pos_callback);
		glfwSetWindowSizeCallback(glfwWindow, window_size_callback);
		glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);
		glfwSetWindowCloseCallback(glfwWindow, window_close_callback);
		glfwSetWindowRefreshCallback(glfwWindow, window_refresh_callback);
		glfwSetWindowFocusCallback(glfwWindow, window_focus_callback);
		glfwSetWindowIconifyCallback(glfwWindow, window_iconify_callback);
		glfwSetWindowMaximizeCallback(glfwWindow, window_maximize_callback);
		glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);
		glfwSetCursorPosCallback(glfwWindow, cursor_position_callback);
		glfwSetCursorEnterCallback(glfwWindow, cursor_enter_callback);
		glfwSetScrollCallback(glfwWindow, scroll_callback);
		glfwSetKeyCallback(glfwWindow, key_callback);
		glfwSetCharCallback(glfwWindow, char_callback);
		glfwSetCharModsCallback(glfwWindow, char_mods_callback);
		glfwSetDropCallback(glfwWindow, drop_callback);
	}
}

void FWindow::SetWindowTitle(const char *InTitle)
{
	if (glfwWindow)
	{
		glfwSetWindowTitle(glfwWindow, InTitle);
	}
}

void FWindow::SetWindowSize(int32_t InWidth, int32_t InHeight, bool bFullScreen)
{
	if (!glfwWindow)
	{
		return;
	}

	if (bFullScreen)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, InWidth, InHeight, mode->refreshRate);
	}
	else
	{
		glfwSetWindowSize(glfwWindow, InWidth, InHeight);
	}
}

void FWindow::SetWindowPosition(int32_t InPosX, int32_t InPosY)
{
	if (glfwWindow)
	{
		glfwSetWindowPos(glfwWindow, InPosX, InPosY);
	}
}

void FWindow::ShowWindow(bool InbShow)
{
	if (glfwWindow)
	{
		InbShow ? glfwShowWindow(glfwWindow) : glfwHideWindow(glfwWindow);
	}
}

void FWindow::CloseWindow()
{
	if (glfwWindow)
	{
		glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
	}
}

void FWindow::DestroyWindow()
{
	if (glfwWindow)
	{
		glfwDestroyWindow(glfwWindow); 
		glfwWindow = nullptr;
	}
}

void FWindow::LockCursor(bool InbLock)
{
	if (glfwWindow)
	{
		InbLock ? glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED)
			: glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void FWindow::GetWindowSize(int32_t &Width, int32_t &Height)
{
	Width = Height = 0;
	if (glfwWindow)
	{
		glfwGetWindowSize(glfwWindow, &Width, &Height);
	}
}

void FWindow::GetRenderAreaSize(int32_t &Width, int32_t &Height)
{
	Width = Height = 0;
	if (glfwWindow)
	{
		glfwGetWindowSize(glfwWindow, &Width, &Height);
	}
}

bool FWindow::IsWindowClosed()
{
	if (glfwWindow)
	{
		return glfwWindowShouldClose(glfwWindow) == GLFW_TRUE;
	}

	return true;
}

void* FWindow::GetNativeHandle()
{
	if (glfwWindow)
	{
#ifdef GLFW_EXPOSE_NATIVE_WIN32
		return glfwGetWin32Window(glfwWindow);
#elif defined(GLFW_EXPOSE_NATIVE_COCOA)
        return glfwGetCocoaWindow(glfwWindow);
#endif
	}

	return nullptr;
}

// event handler
void FWindow::OnCloseRequest()
{
	if (Client)
	{
		Client->OnCloseRequest(*this);
	}
}

void FWindow::OnPosChanged(int32_t InPosX, int32_t InPosY)
{
	if (Client)
	{
		Client->OnPosChanged(*this, InPosX, InPosY);
	}
}

void FWindow::OnSizeChanged(int32_t InWidth, int32_t InHeight)
{
	if (Client)
	{
		Client->OnSizeChanged(*this, InWidth, InHeight);
	}
}

void FWindow::OnRenderAreaSizeChanged(int32_t InWidth, int32_t InHeight)
{
	if (Client)
	{
		Client->OnRenderAreaSizeChanged(*this, InWidth, InHeight);
	}
}

void FWindow::OnFocus(bool InbFocused)
{
	if (Client)
	{
		Client->OnFocus(*this, InbFocused);
	}
}

void FWindow::OnKeyDown(int32_t InKey, int32_t InScancode, int32_t InModifier)
{
	if (Client)
	{
		Client->OnKeyDown(*this, InKey, InScancode, InModifier);
	}
}

void FWindow::OnKeyUp(int32_t InKey, int32_t InScancode, int32_t InModifier)
{
	if (Client)
	{
		Client->OnKeyUp(*this, InKey, InScancode, InModifier);
	}
}

void FWindow::OnChar(uint32_t InUnicodepoint)
{
	if (Client)
	{
		Client->OnChar(*this, InUnicodepoint);
	}
}

void FWindow::OnMouseDown(int32_t InButton, int32_t InModifier)
{
	if (Client)
	{
		Client->OnMouseDown(*this, InButton, InModifier);
	}
}

void FWindow::OnMouseUp(int32_t InButton, int32_t InModifier)
{
	if (Client)
	{
		Client->OnMouseUp(*this, InButton, InModifier);
	}
}

void FWindow::OnMouseMove(double InX, double InY)
{
	if (Client)
	{
		Client->OnMouseMove(*this, InX, InY);
	}
}

void FWindow::OnMouseScroll(double InOffsetX, double InOffsetY)
{
	if (Client)
	{
		Client->OnMouseScroll(*this, InOffsetX, InOffsetY);
	}
}

//////////////////////////////////////////////////////////////////////////
// Application

bool FApplication::Init()
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		return false;
	}

	glfwSetMonitorCallback(monitor_callback);
	glfwSetJoystickCallback(joystick_callback);
	return true;
}

void FApplication::Shutdown()
{
	glfwTerminate();
}

void FApplication::RunLoop()
{
	double LastTime = glfwGetTime();

	while (!bReqQuit)
	{
		glfwPollEvents();
		// check again, because event handler maybe set it.
		if (bReqQuit)
		{
			break;
		}

		double NowTime = glfwGetTime();
		OnTick((float)(NowTime - LastTime));
		LastTime = NowTime;
	}
}

void FApplication::OnTick(float InDeltaSeconds)
{

}

void FApplication::RequestQuit()
{
	bReqQuit = true;
}
