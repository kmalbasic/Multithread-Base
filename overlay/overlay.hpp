#pragma once
#undef UNICODE
#include <Windows.h>
#include <TlHelp32.h>
#include <Dwmapi.h> 
#include <iostream>
#include <string>
#include <sstream> 
#include <Psapi.h>
#include <vector>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")


//#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "Dwmapi.lib")

struct window_values
{
	unsigned int PIDOwner;
	std::string ClassName;
	std::string WindowName;
	uint64_t WindowStyle;
	uint64_t WindowStyleEx;
	HWND ProcessesHWND;
};

class hijack
{
private:
	std::string_view process_name;
	std::string_view process_class_name;
	std::string_view process_window_name;
public:
	std::string_view overlay_string = "";
	unsigned int c_window_size_x;
	unsigned int c_window_size_y;

	hijack();
	window_values window_val;

	HWND find_top_window(DWORD pid);
	HWND hijack_ol(std::string_view target, std::string_view class_name, LPCSTR target_window, std::string_view window_name = "");
	std::vector<HWND> grab_ol_hwnd();

};

class overlay
{
private:
	IDirect3DDevice9* d3d_device;
	IDirect3D9* d3d;
	HWND draw_window;
	std::string_view game_window_string;
	std::string_view overlay_window_string;
	D3DPRESENT_PARAMETERS d3d_param;
	LPD3DXFONT Font;

public:
	overlay(HWND Window);
	~overlay();

	bool InitiateD3D(unsigned int X, unsigned int Y);
	void StartRender(std::string_view to_draw_on, std::string_view game_window_name);
	void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char* fmt, ...);
	void ClearScreen();
	void Shutdown();
	void SetRenderStates();

};


