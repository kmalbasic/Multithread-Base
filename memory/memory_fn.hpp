#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

namespace memory {

	// These functions are in here for intermediate programmers, mostly people that are into game-hacking.
	// In future there will be some pattern scanning functions and possibly implementation of a communicating
	// with a driver to do some WPM/RPM through kernel.

	bool attach(const char* process_name, DWORD access_rights);
	bool detach(HANDLE h);

	template <typename t>
	t read_memory(DWORD address);

	template <typename t>
	void write_memory(DWORD address, t value);

	bool memcmp(const BYTE* data, const BYTE* mask, const char* mask_str);
	DWORD find_signature(DWORD start, DWORD size, const char* sig, const char* mask);

}