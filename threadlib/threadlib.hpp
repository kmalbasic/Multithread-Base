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
	int set_custom_affinity();  // dbg fn mostly
	int calculate_affinity_req(int threads);
	thread_info initialize_info();
	void assign_fix(int i);
	void print_info(thread_info thread_object);
	void run_workload(thread_info thread_object, thread threads[], bool custom_affinity);

}