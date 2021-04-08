#pragma once
#include <thread>

using namespace std;

struct thread_info {

	std::string cpu_name;
	int amount;
	bool custom_set;

};

namespace thread_lib {
	
	std::string get_cpu_name();
	int calculate_threads();
	thread_info initialize_info();
	void print_info(thread_info thread_object);
	void run_workload(thread_info thread_object, thread threads[]);
	
}
