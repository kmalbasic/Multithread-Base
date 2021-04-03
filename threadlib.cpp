#include <iostream>
#include <thread>
#include <Windows.h>
#include <intrin.h>
#include "threadlib.hpp"
#include "workload.hpp"

using namespace std;

std::string thread_lib::get_cpu_name()
{
	int cpu_information[4] = { -1 };
	char cpu_string[0x40];
	__cpuid(cpu_information, 0x80000000);
	unsigned int nExIds = cpu_information[0];

	memset(cpu_string, 0, sizeof(cpu_string));

	for (int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(cpu_information, i);
		if (i == 0x80000002)
			memcpy(cpu_string, cpu_information, sizeof(cpu_information));
		else if (i == 0x80000003)
			memcpy(cpu_string + 16, cpu_information, sizeof(cpu_information));
		else if (i == 0x80000004)
			memcpy(cpu_string + 32, cpu_information, sizeof(cpu_information));
	}
	return cpu_string;
}

int thread_lib::calculate_threads() {
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	int threads = system_info.dwNumberOfProcessors; // cores
	return threads;
	//thread* available_threads = new thread[cores];
}

thread_info thread_lib::initialize_info() {

	thread_info thread_object;
	thread_object.amount = thread_lib::calculate_threads();
	thread_object.cpu_name = thread_lib::get_cpu_name();
	thread_object.custom_set = false;
	cout << "[+] Initialized information about the system. " << endl;

	return thread_object;

}

void thread_lib::print_info(thread_info thread_object) {

	cout << "[*] CPU name: " << thread_object.cpu_name << endl;
	cout << "[*] Amount of threads: " << thread_object.amount << endl;

}

void empty_fn(int a) {
	cout << "Test " << a << endl;
}

void thread_lib::run_workload(thread_info thread_object, thread threads[]) {

	for (int i = 0; i < thread_object.amount; i++) {

		threads[i] = thread(workload_selector, i);
		threads[i].join();

	}

}