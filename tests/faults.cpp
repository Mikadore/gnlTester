#include "faults.hpp"

#include "gnl.hpp"
#include "check.hpp"

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

extern int iTest;

static bool child_succeeded(pid_t child)
{
	int	status;

	if (waitpid(child, &status, 0) != child)
		return (false);
	return (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS);
}

static size_t measure_first_call_allocations(void)
{
	int		pipefd[2];
	pid_t	child;
	size_t	calls;
	ssize_t	bytes;

	calls = 0;
	if (pipe(pipefd) != 0)
	{
		check(false);
		return (0);
	}
	child = fork();
	if (child == 0)
	{
		int		fd;
		HookStats stats;

		close(pipefd[0]);
		testFailures = 0;
		fd = open("files/faults", O_RDONLY);
		check(fd >= 0);
		if (fd >= 0)
		{
			stats = gnl(fd, "alpha\n");
			gnl(fd, "beta\n");
			gnl(fd, "omega");
			gnl(fd, NULL);
			close(fd);
			if (!showLeaks())
				check(false);
			write(pipefd[1], &stats.malloc_calls, sizeof(stats.malloc_calls));
		}
		close(pipefd[1]);
		exit(testFailures ? EXIT_FAILURE : EXIT_SUCCESS);
	}
	close(pipefd[1]);
	if (child < 0)
	{
		close(pipefd[0]);
		check(false);
		return (0);
	}
	bytes = read(pipefd[0], &calls, sizeof(calls));
	close(pipefd[0]);
	check(child_succeeded(child));
	check(bytes == (ssize_t)sizeof(calls));
	return (calls);
}

static void check_malloc_failure(size_t failure_point)
{
	pid_t child;

	child = fork();
	if (child == 0)
	{
		int			fd;
		HookOptions	options;
		HookStats	stats;

		testFailures = 0;
		fd = open("files/faults", O_RDONLY);
		check(fd >= 0);
		options.fail_malloc_at = failure_point;
		stats = gnl(fd, NULL, options);
		check(stats.malloc_failure_injected);
		if (fd >= 0)
			close(fd);
		if (!showLeaks())
			check(false);
		exit(testFailures ? EXIT_FAILURE : EXIT_SUCCESS);
	}
	if (child < 0)
		check(false);
	else
		check(child_succeeded(child));
}

static void test_malloc_failures(void)
{
	size_t calls;
	size_t i;

	title("malloc failure injection: ")
	calls = measure_first_call_allocations();
	check(calls > 0);
	i = 1;
	while (i <= calls)
	{
		check_malloc_failure(i);
		i++;
	}
	cout << ENDL;
}

void runFaultTests(void)
{
	int fd;

	title("short reads: ")
	TEST(
		HookOptions options;
		options.max_read_size = 2;
		fd = open("files/faults", O_RDONLY);
		check(fd >= 0);
		gnl(fd, "alpha\n", options);
		gnl(fd, "beta\n", options);
		gnl(fd, "omega", options);
		gnl(fd, NULL, options);
		if (fd >= 0) close(fd);
	)
	cout << ENDL;

	title("pipe input: ")
	TEST(
		int pipefd[2];
		int pipe_status;
		HookOptions options;
		options.max_read_size = 2;
		pipe_status = pipe(pipefd);
		check(pipe_status == 0);
		if (pipe_status == 0)
		{
			write(pipefd[1], "pipe\ntail", 9);
			close(pipefd[1]);
			gnl(pipefd[0], "pipe\n", options);
			gnl(pipefd[0], "tail", options);
			gnl(pipefd[0], NULL, options);
			close(pipefd[0]);
		}
	)
	cout << ENDL;

	title("interrupted read retry: ")
	TEST(
		HookOptions options;
		options.fail_read_at = 1;
		options.fail_read_errno = EINTR;
		fd = open("files/faults", O_RDONLY);
		check(fd >= 0);
		gnl(fd, "alpha\n", options);
		gnl(fd, "beta\n");
		gnl(fd, "omega");
		gnl(fd, NULL);
		if (fd >= 0) close(fd);
	)
	cout << ENDL;

	title("read error recovery: ")
	TEST(
		HookOptions options;
		options.fail_read_at = 1;
		options.fail_read_errno = EIO;
		fd = open("files/faults", O_RDONLY);
		check(fd >= 0);
		gnl(fd, NULL, options);
		gnl(fd, "alpha\n");
		gnl(fd, "beta\n");
		gnl(fd, "omega");
		gnl(fd, NULL);
		if (fd >= 0) close(fd);
	)
	cout << ENDL;

	test_malloc_failures();

	title("malloc failure with buffered remainder: ")
	TEST(
		if (BUFFER_SIZE > 6)
		{
			HookOptions options;
			HookStats stats;
			fd = open("files/faults", O_RDONLY);
			check(fd >= 0);
			gnl(fd, "alpha\n");
			options.fail_malloc_at = 1;
			stats = gnl(fd, NULL, options);
			check(stats.malloc_failure_injected);
			if (fd >= 0) close(fd);
		}
	)
	cout << ENDL;
}
