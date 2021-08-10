#include<iostream>
#include<Windows.h>
#include "menu.hpp"
#include "../simplified_fn/simplified_fn.hpp"
#include "../threadlib/threadlib.hpp"
#include "../memory/memory_fn.hpp"
#include "../injection/injection.hpp"
#include "../overlay/overlay.hpp"

using namespace std;

bool menu::select_options() {

	string cmd; 

	cout << "[?] Please enter the number of your desired action: ";
	cin >> cmd;

	if (cmd == "1") {
		thread_info thread_information = thread_lib::initialize_info();
		thread* threads = new thread[thread_information.amount];
		thread_lib::run_workload(thread_information, threads, false);
		smpl::sleep_sec(5);
		return true;
	}

	if (cmd == "2") {
		cout << "[?] Enter the process name: ";
		string process_name;
		cin >> process_name;
		cout << "[?] Enter the DLL path: ";
		string dll_path;
		cin >> dll_path;
		inject::load_lib(process_name.c_str(), dll_path.c_str());
		smpl::sleep_sec(5);
	}

	if (cmd == "3") {
		cout << "[?] Enter the process name: ";
		string process_name; 
		cin >> process_name;
		cout << "[?] Enter the DLL path: ";
		string dll_path;
		cin >> dll_path;
		inject::manual_map(process_name, dll_path.c_str());
		smpl::sleep_sec(5);
	}

	if (cmd == "4") {
		string process_name;
		cout << "[?] Enter the process name to inject hardcoded shellcode (custom shellcode requires modifying the source: ";
		cin >> process_name;
		const char* process_name_c = process_name.c_str();

		char shellcode[] =
		 "\x00\x53\xff\x0a\x1d\xb2\x00\x00\x50\xd3\x66\x24\x4a\x00\x00" // literally random
		 "\x00\x53\xff\x0a\x1d\xb2\x00\x00\x50\xd3\x66\x24\x4a\x00\x00";

		memory::inject_shell(shellcode, process_name_c);
		smpl::sleep_sec(20);
		return true;
	}

	if (cmd == "5") {
		char window_title[64];
		string program_name;
		cout << "[?] Enter the window title (D3D): ";
		cin.getline(window_title,30);
		cout << "[?] Enter the program name: ";
		cin.getline(window_title, 64);

		hijack* hijack_obj = &hijack();

		HWND hijacked_hwnd = hijack_obj->hijack_ol("notepad.exe", "Notepad", window_title);
		overlay* overlay_obj = &overlay(hijacked_hwnd);

		if (overlay_obj->InitiateD3D(hijack_obj->c_window_size_x, hijack_obj->c_window_size_y))
			overlay_obj->StartRender(hijack_obj->overlay_string, window_title);
		return true;
	}

	if (cmd == "6") {
		exit(EXIT_SUCCESS);
		return true;
	}


	
	
}

void menu::run_menu() {

	system("cls");  // dont ever do this

	cout << "[=] Available options" << endl;
	smpl::spacer();
	cout << "[1] Run workload defined in source files." << endl;
	cout << "[2] Run LoadLibrary injection tool." << endl;
	cout << "[3] Run PE manualmap tool." << endl;
	cout << "[4] Inject shellcode into a certain process." << endl;
	cout << "[5] Hijack a process and run an overlay." << endl;
	cout << "[6] Exit" << endl;
	smpl::spacer();
	if (!menu::select_options()) {
		cout << "[X] Wrong options selected or there was an error while performing the operation. Try again." << endl;
	};
}
