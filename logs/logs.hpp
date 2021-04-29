#pragma once

#include <iostream>
#include <Windows.h>

// enum just because I didn't use it for quite some time

enum log_type {

	DEBUG_LOG = 1,
	ERROR_LOG = 2,
	DEFAULT_LOG = 3,
	START_LOG = 4,
	END_LOG = 5

};

namespace logs {

	void lsavef(char* txt, log_type type);
	void popup(char* txt, log_type type);

}