#ifndef GNL_HPP
# define GNL_HPP
# include "leaks.hpp"
# include <signal.h>
# include <sys/wait.h>
# include <unistd.h>
# include <stdlib.h>
# include "check.hpp"

// Run each sequence in its own process so static state and crashes are isolated.
#define TEST(x) { \
	int status = 0; \
	pid_t test = fork(); \
	if (test == 0) { \
		testFailures = 0; \
		x \
		if (!showLeaks()) check(false); \
		cout << FG_DEFAULT << std::flush; \
		exit(testFailures ? EXIT_FAILURE : EXIT_SUCCESS); \
	} else if (test < 0) { \
		check(false); \
	} else { \
		int nb_sleep = 0; \
		const int usleep_time = TIMEOUT_US / 100; \
		pid_t result = 0; \
		while ((result = waitpid(test, &status, WNOHANG)) == 0 && nb_sleep * usleep_time <= TIMEOUT_US) \
		{ \
			usleep(usleep_time); \
			nb_sleep++; \
		} \
		if (result == 0) \
		{ \
			kill(test, 9); \
			waitpid(test, &status, 0); \
			cout << FG_RED << "TIMEOUT" << FG_DEFAULT; \
			check(false); \
		} else if (result < 0 || !WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS) { \
			check(false); \
		} \
	} \
}

HookStats gnl(int fd, char const *expected,
	const HookOptions &options = HookOptions());

#endif
