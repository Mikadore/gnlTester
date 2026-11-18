#include "read_failure.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

static unsigned long	g_random_state;
static unsigned int	g_min_interval;
static unsigned int	g_max_interval;
static unsigned int	g_interval_left;
static unsigned int	g_max_failures;
static unsigned int	g_failure_count;
static unsigned int	g_open_fd_count;
static int				g_enabled;
static int				g_eof_seen;

static unsigned int	next_interval(void)
{
	g_random_state = g_random_state * 1103515245UL + 12345UL;
	return (g_min_interval + g_random_state
		% (g_max_interval - g_min_interval + 1));
}

void	read_failure_start(unsigned int seed, unsigned int min_interval,
		unsigned int max_interval, unsigned int max_failures)
{
	if (!min_interval || max_interval < min_interval || !max_failures)
		return ;
	g_random_state = seed;
	if (!g_random_state)
		g_random_state = 1;
	g_min_interval = min_interval;
	g_max_interval = max_interval;
	g_max_failures = max_failures;
	g_failure_count = 0;
	g_open_fd_count = 0;
	g_eof_seen = 0;
	g_interval_left = next_interval();
	g_enabled = 1;
}

void	read_failure_stop(void)
{
	g_enabled = 0;
}

unsigned int	read_failure_count(void)
{
	return (g_failure_count);
}

unsigned int	read_failure_open_fd_count(void)
{
	return (g_open_fd_count);
}

int	read_failure_eof_seen(void)
{
	return (g_eof_seen);
}

ssize_t	gnl_test_read(int fd, void *buffer, size_t count)
{
	ssize_t	result;

	if (g_enabled && g_failure_count < g_max_failures)
	{
		if (!g_interval_left)
		{
			g_failure_count++;
			if (fcntl(fd, F_GETFD) != -1)
				g_open_fd_count++;
			g_interval_left = next_interval();
			errno = EIO;
			return (-1);
		}
		g_interval_left--;
	}
	result = read(fd, buffer, count);
	if (g_enabled && result == 0)
		g_eof_seen = 1;
	return (result);
}
