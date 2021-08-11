#include "overlay.hpp"
#include "../injection/injection.hpp"
#include "../memory/memory_fn.hpp"
#include "../render/render.hpp"


hijack::hijack()
{
	this->process_name = "";
	this->process_class_name = "";
	this->process_window_name = "";

}

overlay::overlay(HWND window)
{
	this->d3d_device = nullptr;
	this->d3d = nullptr;
	this->draw_window = window;
	this->d3d_param = {};
	this->Font = nullptr;
	this->overlay_window_string = "";
	//D3DXCreateFont(this->d3d_device, 50, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &this->Font);

}

overlay::~overlay()
{

}

HWND hijack::find_top_window(DWORD pid)
{
	std::pair<HWND, DWORD> params = { 0, pid };

	BOOL res = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			auto p_params = (std::pair<HWND, DWORD>*)(lParam);

			DWORD pid;
			if (GetWindowThreadProcessId(hwnd, &pid) && pid == p_params->second)
			{
				SetLastError(-1);
				p_params->first = hwnd;
				return FALSE;
			}
			return TRUE;
		}, (LPARAM)&params);

	if (!res && GetLastError() == -1 && params.first)
	{
		return params.first;
	}

	return 0;
}

std::vector<HWND> hijack::grab_ol_hwnd()
{
	std::vector<HWND> window_list = {};
	HWND current_hwnd = NULL;
	DWORD PID = inject::find_pid({ this->process_name.begin(), this->process_name.end() });

	do
	{
		current_hwnd = FindWindowEx(NULL, current_hwnd, NULL, NULL);
		GetWindowThreadProcessId(current_hwnd, &PID);

		char className[256]; char windowName[256];
		GetClassNameA(current_hwnd, className, 256);
		GetWindowTextA(current_hwnd, windowName, 256);

		std::string c_name = className;
		std::string w_name = windowName;


		if (this->process_class_name == c_name && this->process_window_name == "")
		{
			printf("[sys] HWND = 0x%llX    \n[sys] class_name = %s \n", current_hwnd, c_name.c_str());
			window_list.push_back(current_hwnd);
		}

		else if (this->process_class_name == c_name && w_name == this->process_window_name)
		{
			printf("[sys] HWND = 0x%llX  \n[sys] window_name = %s    \n[sys] class_name = %s \n", current_hwnd, w_name.c_str(), c_name.c_str());
			window_list.push_back(current_hwnd);
		}

	} while (current_hwnd != NULL);

	return window_list;
}

HWND hijack::hijack_ol(std::string_view target, std::string_view class_name, LPCSTR target_window,  std::string_view window_name)
{

	this->process_name = target;
	this->process_class_name = class_name;
	this->process_window_name = window_name;
	

	std::vector<unsigned int> running_processes = memory::get_pid_list(this->process_name);
	std::vector<HWND> running_windows = {};

	if (running_processes.empty())
		return 0x0;

	for (auto& Process : running_processes)
	{
		HANDLE OldProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, Process);
		TerminateProcess(OldProcessHandle, NULL);
		memory::detach(OldProcessHandle);
	}
	running_processes.clear();

	size_t suffix = target.find_last_of(".");
	std::string_view rawname = target.substr(0, suffix);
	std::string final = std::string("start ") + rawname.data();
	overlay_string = final;
	printf("[sys] System Call -> {%s}\n", final.c_str());

	if (system(final.data()) == 1)
		return 0x0;

	running_processes = memory::get_pid_list(target);

	if (running_processes.empty() || running_processes.size() > 1)
		return 0x0;

	window_val.WindowStyle = WS_VISIBLE;
	window_val.PIDOwner = running_processes.at(0);

	printf("[sys]Please wait... \n");

	Sleep(1000);

	running_windows = this->grab_ol_hwnd();
	HWND hwnd = running_windows.at(0);
	RECT c_window_rect;
	GetWindowRect(FindWindowA(0, target_window), &c_window_rect);
	if (!FindWindowA(0, target_window))
		printf("[sys] Error while finding target window! \n");
	c_window_rect.top += 30;

	this->c_window_size_x = c_window_rect.right - c_window_rect.left;
	this->c_window_size_y = c_window_rect.bottom - c_window_rect.top;

	SetMenu(hwnd, NULL);

	SetWindowPos(hwnd, HWND_TOPMOST, c_window_rect.left, c_window_rect.top, c_window_size_x, c_window_size_y, 0);

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY | LWA_ALPHA);

	MARGINS Margins = { -1 };
	DwmExtendFrameIntoClientArea(hwnd, &Margins);

	Sleep(3000);
	return hwnd;
}

bool overlay::InitiateD3D(unsigned int X, unsigned int Y)
{

	HRESULT hr;
	this->d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (this->d3d == nullptr)
		return false;

	this->d3d_param = { 0 };

	this->d3d_param.Windowed = true;
	this->d3d_param.SwapEffect = D3DSWAPEFFECT_DISCARD;
	this->d3d_param.hDeviceWindow = this->draw_window;
	this->d3d_param.BackBufferFormat = D3DFMT_A8R8G8B8;
	this->d3d_param.SwapEffect = D3DSWAPEFFECT_DISCARD;
	this->d3d_param.BackBufferWidth = X;
	this->d3d_param.BackBufferHeight = Y;
	this->d3d_param.AutoDepthStencilFormat = D3DFMT_D16;
	this->d3d_param.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	this->d3d_param.EnableAutoDepthStencil = TRUE;
	this->d3d_param.MultiSampleQuality = DEFAULT_QUALITY;

	this->d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->draw_window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &this->d3d_param, &this->d3d_device);

	if (this->d3d_device == nullptr)
		return false;

	D3DXCreateFont(this->d3d_device, 50, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma", &this->Font);

	if (this->Font == nullptr)
		return false;

	return true;
}

void overlay::StartRender(std::string_view to_draw_on, std::string_view game_window_name)
{

	
	this->overlay_window_string = to_draw_on;

	

	while (1)
	{
		MSG message;
		message.message = WM_NULL;

		

		if (PeekMessage(&message, this->draw_window, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			HWND gamewindow = FindWindow(NULL, game_window_name.data());

			if (gamewindow != NULL)
			{
				WINDOWINFO info;
				GetWindowInfo(gamewindow, &info);

				if (!IsIconic(this->draw_window))
				{
					SetWindowLongPtr(this->draw_window, GWL_STYLE, WS_VISIBLE);
					SetWindowLongPtr(this->draw_window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

					SetWindowPos(this->draw_window, HWND_TOPMOST, info.rcClient.left, info.rcClient.top, ((info.rcClient.right) - (info.rcClient.left)), ((info.rcClient.bottom) - (info.rcClient.top)), SWP_SHOWWINDOW);

				}

				HWND foreground = GetForegroundWindow();

				if (foreground == gamewindow)
				{
					this->d3d_device->BeginScene();
					SetRenderStates();
					this->ClearScreen();

					DrawString(100, 100, D3DCOLOR_ARGB(255, 0, 0, 0), this->Font, "drawing a string");
					render::Line(this, 100, 100, 200, 100, Color(255, 255, 0, 255));
					render::Rect(this, 100, 250, 200, 250, Color(255, 255, 0, 255));
					render::RectFilled(this, 100, 400, 200, 400, Color(255, 255, 0, 255));

					this->d3d_device->EndScene();
					this->d3d_device->Present(NULL, NULL, NULL, NULL);
				}

				else
				{
					this->d3d_device->BeginScene();
					this->ClearScreen();
					this->d3d_device->EndScene();
				}
			}

			else
			{
				this->d3d_device->BeginScene();
				this->ClearScreen();
				this->d3d_device->EndScene();

				std::stringstream ss;
				//ss << "taskkill / f / im " << this->overlay_window_string;
				//system(ss.str().c_str());

			}
		}
		if (GetAsyncKeyState(VK_HOME))
		{
			this->d3d_device->BeginScene();
			this->ClearScreen();
			this->d3d_device->EndScene();
			break;
		};

		Sleep(1);
	}

}



void overlay::DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char* fmt, ...)
{
	RECT FontPos = { x, y, x + 120, y + 16 };
	char buf[1024] = { '\0' };
	va_list va_alist;

	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

void overlay::ClearScreen()
{
	d3d_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
}

void overlay::Shutdown()
{

}

void overlay::SetRenderStates()
{
	this->d3d_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	this->d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	this->d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	this->d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
	this->d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	this->d3d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	this->d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
	this->d3d_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	this->d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	this->d3d_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	this->d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	this->d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	this->d3d_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	this->d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	this->d3d_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	this->d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	this->d3d_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
	this->d3d_device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	this->d3d_device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}
