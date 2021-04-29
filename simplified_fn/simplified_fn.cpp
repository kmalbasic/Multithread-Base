#include<iostream>
#include<chrono>
#include <Windows.h>
#include "simplified_fn.hpp"

int smpl::get_execution_time(void (*fn)(int), int threads) {

#ifdef _DEBUG 
	auto start_time = std::chrono::high_resolution_clock::now();
	fn(threads);
	auto end_time = std::chrono::high_resolution_clock::now();
	auto int_ms = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
	std::chrono::duration<long, std::micro> int_usec = int_ms;

	return int_usec.count();
#endif;
	return 1;
	
}

void smpl::sleep_sec(int s) {

	int conversion_to_s = s * 1000;
	Sleep(conversion_to_s);

}

void smpl::spacer() {

	std::cout << "==============================================================\n";
	//really retarded, but looks cleaner overall
}
