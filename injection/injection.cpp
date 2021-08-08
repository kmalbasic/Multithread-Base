#include "injection.hpp"
#include "../memory/memory_fn.hpp"

VOID stub(VOID) { }

DWORD inject::find_pid(string processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processSnapshot);
	return 0;
}


DWORD __stdcall inject::library_load(LPVOID mem)
{

	inject_data* param = (inject_data*)mem;

	PIMAGE_BASE_RELOCATION pIBR = param->BaseReloc;

	DWORD delta = (DWORD)((LPBYTE)param->ImageBase - param->NtHeaders->OptionalHeader.ImageBase);

	while (pIBR->VirtualAddress)
	{
		if (pIBR->SizeOfBlock >= sizeof(IMAGE_BASE_RELOCATION))
		{
			int count = (pIBR->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
			PWORD list = (PWORD)(pIBR + 1);

			for (int i = 0; i < count; i++)
			{
				if (list[i])
				{
					PDWORD ptr = (PDWORD)((LPBYTE)param->ImageBase + (pIBR->VirtualAddress + (list[i] & 0xFFF)));
					*ptr += delta;
				}
			}
		}

		pIBR = (PIMAGE_BASE_RELOCATION)((LPBYTE)pIBR + pIBR->SizeOfBlock);
	}

	PIMAGE_IMPORT_DESCRIPTOR pIID = param->ImportDirectory;

	while (pIID->Characteristics)
	{
		PIMAGE_THUNK_DATA OrigFirstThunk = (PIMAGE_THUNK_DATA)((LPBYTE)param->ImageBase + pIID->OriginalFirstThunk);
		PIMAGE_THUNK_DATA FirstThunk = (PIMAGE_THUNK_DATA)((LPBYTE)param->ImageBase + pIID->FirstThunk);

		HMODULE hModule = param->fnLoadLibraryA((LPCSTR)param->ImageBase + pIID->Name);

		if (!hModule)
			return FALSE;

		while (OrigFirstThunk->u1.AddressOfData)
		{
			if (OrigFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				DWORD Function = (DWORD)param->fnGetProcAddress(hModule,
					(LPCSTR)(OrigFirstThunk->u1.Ordinal & 0xFFFF));

				if (!Function)
					return FALSE;

				FirstThunk->u1.Function = Function;
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME pIBN = (PIMAGE_IMPORT_BY_NAME)((LPBYTE)param->ImageBase + OrigFirstThunk->u1.AddressOfData);
				DWORD Function = (DWORD)param->fnGetProcAddress(hModule, (LPCSTR)pIBN->Name);
				if (!Function)
					return FALSE;

				FirstThunk->u1.Function = Function;
			}
			OrigFirstThunk++;
			FirstThunk++;
		}
		pIID++;
	}

	if (param->NtHeaders->OptionalHeader.AddressOfEntryPoint)
	{
		dllmain EntryPoint = (dllmain)((LPBYTE)param->ImageBase + param->NtHeaders->OptionalHeader.AddressOfEntryPoint);

		return EntryPoint((HMODULE)param->ImageBase, DLL_PROCESS_ATTACH, NULL);
	}
	return TRUE;
}

void inject::manual_map(string process_name, LPCSTR szDllFile)
{
	DWORD pid = inject::find_pid(process_name);

	inject_data param;

	HANDLE hFile = CreateFileA(szDllFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, 0, NULL); // Open the DLL

	DWORD FileSize = GetFileSize(hFile, NULL);
	PVOID FileBuffer = VirtualAlloc(NULL, FileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	ReadFile(hFile, FileBuffer, FileSize, NULL, NULL);

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)FileBuffer;

	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)FileBuffer + pDosHeader->e_lfanew);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	PVOID ExecutableImage = VirtualAllocEx(hProcess, NULL, pNtHeaders->OptionalHeader.SizeOfImage,
		MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);


	WriteProcessMemory(hProcess, ExecutableImage, FileBuffer,
		pNtHeaders->OptionalHeader.SizeOfHeaders, NULL);

	PIMAGE_SECTION_HEADER pSectHeader = (PIMAGE_SECTION_HEADER)(pNtHeaders + 1);

	for (int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++)
	{
		WriteProcessMemory(hProcess, (PVOID)((LPBYTE)ExecutableImage + pSectHeader[i].VirtualAddress),
			(PVOID)((LPBYTE)FileBuffer + pSectHeader[i].PointerToRawData), pSectHeader[i].SizeOfRawData, NULL);
	}

	PVOID LoaderMemory = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE); // Allocate memory for the loader code

	param.ImageBase = ExecutableImage;
	param.NtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)ExecutableImage + pDosHeader->e_lfanew);

	param.BaseReloc = (PIMAGE_BASE_RELOCATION)((LPBYTE)ExecutableImage
		+ pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
	param.ImportDirectory = (PIMAGE_IMPORT_DESCRIPTOR)((LPBYTE)ExecutableImage
		+ pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	param.fnLoadLibraryA = LoadLibraryA;
	param.fnGetProcAddress = GetProcAddress;


	WriteProcessMemory(hProcess, LoaderMemory, &param, sizeof(inject_data),
		NULL);
	WriteProcessMemory(hProcess, (PVOID)((inject_data*)LoaderMemory + 1), inject::library_load,
		(DWORD)stub - (DWORD)inject::library_load, NULL);

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)((inject_data*)LoaderMemory + 1),
		LoaderMemory, 0, NULL);


	WaitForSingleObject(hThread, INFINITE);

	std::cin.get();
	VirtualFreeEx(hProcess, LoaderMemory, 0, MEM_RELEASE);
}

void inject::load_lib(const char* process_name, const char* dll_name) {

	HANDLE process_handle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, (DWORD)memory::attach(process_name, PROCESS_ALL_ACCESS));
	char dll[MAX_PATH];

	GetFullPathName(dll_name, MAX_PATH, dll, 0);
	LPVOID alloc_mem = VirtualAllocEx(process_handle, NULL, sizeof(dll), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(process_handle, alloc_mem, dll, sizeof(dll), NULL);
	CreateRemoteThread(process_handle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, alloc_mem, 0, 0);

	memory::detach(process_handle);

}

#define RELOC_FLAG32(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_HIGHLOW)
#define RELOC_FLAG64(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_DIR64)

#ifdef _WIN64
#define RELOC_FLAG RELOC_FLAG64
#else
#define RELOC_FLAG RELOC_FLAG32
#endif