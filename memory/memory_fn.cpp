#include "memory_fn.hpp"

/*
	HUGE NOTE: Do not use these functions if you are using any sort of anticheating software. You will be instantly flagged/banned.
	This applies to the more intrusive anticheats that operate in ring0. Some usermode anticheats could still be bypassed with implementing
	some additional measures against signature detection or hijacking handles from other legitimate processes. Somewhere in the source I said
	I will add a way of communicating with a loaded driver so you can read/write through it. Again, most up2date anticheats are flagging 
	vulnerable and unsigned drivers (as well as the method of mapping those drivers), so use it with caution.
*/

bool memory::attach(const char* process_name, DWORD access_rights) {

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	HANDLE process_handle = NULL;

	if (snapshot)
	{
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		wchar_t text_wchar[30];

		// conversion in case you want to use unicode instead of multibyte character set
		char w_to_c[256];
		sprintf_s(w_to_c, "%ws", pe.szExeFile);

		if (Process32First(snapshot, &pe))
		{
			do
			{
				if (!strcmp(w_to_c, process_name)) // w_to_c = szExeFile
				{
					process_handle = OpenProcess(access_rights, false, pe.th32ProcessID);
				}
			} while (Process32Next(snapshot, &pe));
		}
		CloseHandle(snapshot);
	}

	// sort of a check, testing
	printf("process_handle(%s): 0x%x\n", process_name, (DWORD)process_handle);

	if (process_handle) 
	{ 
		return true; 
	}
	else 
	{ 
		return false; 
	}

}

bool memory::detach(HANDLE h) {

	return CloseHandle(h);

}

template<typename t>
inline t memory::read_memory(DWORD address)
{
	t buffer;
	ReadProcessMemory(this->process_handle, (LPCVOID)address, &buffer, sizeof(t), NULL);
	return buffer;
}

template<typename t>
void memory::write_memory(DWORD address, t buffer)
{
	WriteProcessMemory(this->process_handle, (LPVOID)address, &buffer, sizeof(t), NULL);
}

bool memory::memcmp(const BYTE* data, const BYTE* mask, const char* mask_str) {
	for (; *mask_str; ++mask_str, ++data, ++mask) {
		if (*mask_str == 'x' && *data != *mask) {
			return false;
		}
	}
	return (*mask_str == NULL);
}

DWORD memory::find_signature(DWORD start, DWORD size, const char* sig, const char* mask)
{
	BYTE* data = new BYTE[size];
	SIZE_T read_bytes;

	memory::read_memory(TargetProcess, (LPVOID)start, data, size, &read_bytes);

	for (DWORD i = 0; i < size; i++)
	{
		if (memory::memcmp((const BYTE*)(data + i), (const BYTE*)sig, mask)) {
			return start + i;
		}
	}
	delete[] data;
	return NULL;
}
