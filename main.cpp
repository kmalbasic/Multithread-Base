#define _CRT_SECURE_NO_WARNINGS  // for use of C functions, if you want use the safe versions of the functions.


#include <iostream>
#include <thread>
#include <Windows.h>
#include <intrin.h>
#include "threadlib/threadlib.hpp"
#include "simplified_fn/simplified_fn.hpp"
#include "logs/logs.hpp"
#include "menu.hpp"

// securing the program is the next step, I will add the heartbeat fn somewhere..
// debuggers check also should be present with maybe some sort of obfuscation

using namespace std;

int main()
{
    thread_info thread_information = thread_lib::initialize_info();
    thread* threads = new thread[thread_information.amount];
    
    thread_lib::print_info(thread_information);
    

    // example fn pointer for sleep_sec

    void (*sleep_s)(int) = &smpl::sleep_sec;

    // a way to resize the console, nothing fancy and might wrap it in a function 

    HWND console = GetConsoleWindow();
    RECT console_rect;
    GetWindowRect(console, &console_rect);

    MoveWindow(console, console_rect.left, console_rect.top, 545, 600, TRUE);

#ifdef _DEBUG

    thread_lib::run_workload(thread_information, threads, true);
    (*sleep_s)(5);  // added for testing since there is almost 0 load and operations execute quickly
    MessageBox(0, "Finished.", "Multithreading operation finished", MB_OK);
    return 0;

#endif

    char log_test[32];
    strcpy(log_test, "Testing popup log.");
    char log_f_test[32] = "Testing file log.";

    thread_lib::run_workload(thread_information, threads, false);

    (*sleep_s)(5);   // added for testing since there is almost 0 load and operations execute quickly

    MessageBox(0, "Finished.", "Multithreading operation finished", MB_OK);

    logs::popup(log_test, DEFAULT_LOG);  // strings are more useful in C++, this is a C example.
    logs::lsavef(log_f_test, DEFAULT_LOG); // logs by date

    while (true) {
        menu::run_menu();
    }

    return 0;

}

