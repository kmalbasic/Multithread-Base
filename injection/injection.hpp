#pragma once
#include<iostream>
#include <stdint.h>
#include <Windows.h>
#include <TlHelp32.h>

#define ERASE_ENTRY_POINT    TRUE
#define ERASE_PE_HEADER      TRUE

using namespace std;





typedef HMODULE(__stdcall* pLoadLibraryA)(LPCSTR);
typedef FARPROC(__stdcall* pGetProcAddress)(HMODULE, LPCSTR);
typedef INT(__stdcall* dllmain)(HMODULE, DWORD, LPVOID);

struct inject_data
{
    LPVOID ImageBase;
    PIMAGE_NT_HEADERS NtHeaders;
    PIMAGE_BASE_RELOCATION BaseReloc;
    PIMAGE_IMPORT_DESCRIPTOR ImportDirectory;
    pLoadLibraryA fnLoadLibraryA;
    pGetProcAddress fnGetProcAddress;

};


namespace inject {

    void manual_map(string process_name, LPCSTR szDllFile);
    void load_lib(const char* process_name, const char* dll_name);
    DWORD find_pid(string processName);
    DWORD __stdcall library_load(LPVOID mem);


    // if you want to inject a hex array (dll to hex concept by Daniel Krupinski)

   /* uint8_t str_to_hex(const string& str)
    {
        uint8_t hex_val = 0;
        for (size_t n = 0; n < 2; ++n)
        {
            hex_val <<= 4;
            char c = str.at(n);
            if (c >= '0' && c <= '9')
                hex_val |= static_cast<uint8_t>(c - '0');
            else if (c >= 'A' && c <= 'F')
                hex_val |= static_cast<uint8_t>(c - 'A' + 0xA);
            else if (c >= 'a' && c <= 'f')
                hex_val |= static_cast<uint8_t>(c - 'a' + 0xA);
        }
        return hex_val;
    }*/
}

