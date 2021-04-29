#include <iostream>
#include <thread>
#include <Windows.h>
#include <intrin.h>
#include "threadlib.hpp"
#include "workload.hpp"
#include "simplified_fn.hpp"

using namespace std;

// cpu name fn and calculate threads/cores grabbed from an old college project, posted on same github

std::string thread_lib::get_cpu_name()
{
	int cpu_information[4] = { -1 };
	char cpu_string[0x40];
	__cpuid(cpu_information, 0x80000000);
	unsigned int ex_ids = cpu_information[0];

	memset(cpu_string, 0, sizeof(cpu_string));

	for (int i = 0x80000000; i <= ex_ids; ++i)
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

int thread_lib::set_custom_affinity() {

	cout << "[debug] do you wish to set custom affinity of this process? Y/N";
	char ans;
	cin >> ans;
	if (ans == 'Y') {
		int custom_aff;
		cin >> custom_aff;
		return custom_aff;
	}
	else
	{
		return thread_lib::calculate_threads();
	}

}

// this function is used for ::SetProcessAffinityMask(GetCurrentProcess(), affinity), to be precise for the bitmask calculation

int thread_lib::calculate_affinity_req(int threads) {

	cout << "debug -> threads = " << threads << endl;

	int sqr = threads - 1;

	return pow(2, sqr) * 2 - 1; // i just push this as decimal, no need to mess around with hex 
}

thread_info thread_lib::initialize_info() {

	thread_info thread_object;
	thread_object.amount = thread_lib::calculate_threads();
	thread_object.cpu_name = thread_lib::get_cpu_name();
	thread_object.custom_set = false;
	cout << "[+] Initialized information about the system. " << endl;

	return thread_object;

}

// insanely dumb fix for assigning functions to a certain thread, but works

void thread_lib::assign_fix(int i) {

#ifdef _DEBUG
	auto result = smpl::get_execution_time(workload_selector, i);
	std::cout << " -> [debug] execution time: " << result << " microseconds" << std::endl;
	return;
#endif
	workload_selector(i);

}

void thread_lib::print_info(thread_info thread_object) {

	cout << "[*] CPU name: " << thread_object.cpu_name << endl;
	cout << "[*] Amount of threads: " << thread_object.amount << endl;

}

/*
void empty_fn(int a) {

	cout << "Test " << a << endl;     no need for this, will remove it if I remember..

}
*/

void thread_lib::run_workload(thread_info thread_object, thread threads[], bool custom_affinity) {

#ifdef _DEBUG
	if (custom_affinity)
	{;
		int affinity_req = thread_lib::calculate_affinity_req(thread_lib::set_custom_affinity());
		cout << "[debug] affinity_req = " << affinity_req << endl;
		::SetProcessAffinityMask(GetCurrentProcess(), affinity_req);
	} //0xf);
#endif

	smpl::spacer();  // aesthetics ya

	for (int i = 0; i < thread_object.amount; i++) {

		threads[i] = thread(assign_fix, i);
		threads[i].join();

	}

	smpl::spacer();

}
