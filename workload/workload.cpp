#include "workload.hpp"
#include <chrono>

void workload1() {
	cout << "Workload 1\n";
}

void workload2() {
	cout << "Workload 2\n";
}

void workload3() {
	cout << "Workload 3\n";
}

void workload4() {
	cout << "Workload 4\n";
}

void workload5() {
	cout << "Workload 5\n";
}

void workload6() {
	cout << "Workload 6\n";
}

void workload7() {
	cout << "Workload 7\n";
}

void workload8() {
	cout << "Workload 8\n";
}

void workload_selector(int index) {

		switch (index) {

		case 0:
			workload1();
			break;
		case 1:
			workload2();
			break;
		case 2:
			workload3();
			break;
		case 3:
			workload4();
			break;
		case 4:
			workload5();
			break;
		case 5:
			workload6();
			break;
		case 6:
			workload7();
			break;
		case 7:
			workload8();
			break;

		}
}
