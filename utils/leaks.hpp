#ifndef LEAK_HPP
# define LEAK_HPP

# include <stddef.h>

struct HookOptions
{
	size_t	fail_malloc_at;
	size_t	max_read_size;
	size_t	fail_read_at;
	int		fail_read_errno;

	HookOptions(void);
};

struct HookStats
{
	size_t	read_calls;
	size_t	wrong_read_sizes;
	size_t	malloc_calls;
	size_t	free_calls;
	size_t	lifetime_errors;
	bool	malloc_failure_injected;
	bool	read_failure_injected;

	HookStats(void);
};

void		beginGnlCall(const HookOptions &options);
HookStats	endGnlCall(void);
bool		isTrackedLive(void *address);
bool		showLeaks(void);

#endif
