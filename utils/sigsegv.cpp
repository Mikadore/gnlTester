#include "sigsegv.hpp"
#include "color.hpp"

#include <unistd.h>

void sigsegv(int signal)
{
	const char message[] = "\033[93mSIGSEGV\033[0m\n";
	write(STDERR_FILENO, message, sizeof(message) - 1);
	_exit(128 + signal);
}
