#include <iostream>
#include <thread>
#include <Windows.h>
#include <intrin.h>
#include "threadlib.hpp"


using namespace std;

int main()
{
    thread_info thread_information = thread_lib::initialize_info();
    thread* threads = new thread[thread_information.amount];
    
    thread_lib::print_info(thread_information);
    thread_lib::run_workload(thread_information, threads);

    return 0;
}

