#include "leaks.hpp"

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

bool is_initializing = false;

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

namespace
{
	typedef void *(*malloc_fn)(size_t);
	typedef void (*free_fn)(void *);
	typedef ssize_t (*read_fn)(int, void *, size_t);

	struct Allocation
	{
		void	*address;
		size_t	size;
		bool	known;
		bool	live;
	};

	const size_t	MAX_TRACKED_ALLOCATIONS = 65536;
	Allocation		allocations[MAX_TRACKED_ALLOCATIONS];
	malloc_fn		next_malloc;
	free_fn			next_free;
	read_fn			next_read;
	bool			resolving_symbols;
	bool			call_active;
	bool			allocation_table_overflow;
	HookOptions		active_options;
	HookStats		active_stats;

	void resolve_symbols(void)
	{
		if (resolving_symbols)
			return ;
		resolving_symbols = true;
		next_malloc = (malloc_fn)dlsym(RTLD_NEXT, "malloc");
		next_free = (free_fn)dlsym(RTLD_NEXT, "free");
		next_read = (read_fn)dlsym(RTLD_NEXT, "read");
		resolving_symbols = false;
	}

	int find_allocation(void *address)
	{
		size_t i;

		i = 0;
		while (i < MAX_TRACKED_ALLOCATIONS)
		{
			if (allocations[i].known && allocations[i].address == address)
				return ((int)i);
			i++;
		}
		return (-1);
	}

	void remember_allocation(void *address, size_t size)
	{
		size_t i;
		int		old;

		old = find_allocation(address);
		if (old >= 0)
		{
			allocations[old].size = size;
			allocations[old].live = true;
			return ;
		}
		i = 0;
		while (i < MAX_TRACKED_ALLOCATIONS && allocations[i].known)
			i++;
		if (i == MAX_TRACKED_ALLOCATIONS)
		{
			allocation_table_overflow = true;
			return ;
		}
		allocations[i].address = address;
		allocations[i].size = size;
		allocations[i].known = true;
		allocations[i].live = true;
	}

	void report_text(const char *message)
	{
		size_t length;

		length = 0;
		while (message[length])
			length++;
		write(STDOUT_FILENO, message, length);
	}
}

HookOptions::HookOptions(void)
	: fail_malloc_at(0), max_read_size(0), fail_read_at(0), fail_read_errno(0)
{
}

HookStats::HookStats(void)
	: read_calls(0), wrong_read_sizes(0), malloc_calls(0), free_calls(0),
	lifetime_errors(0), malloc_failure_injected(false), read_failure_injected(false)
{
}

void beginGnlCall(const HookOptions &options)
{
	active_options = options;
	active_stats = HookStats();
	call_active = true;
}

HookStats endGnlCall(void)
{
	call_active = false;
	return (active_stats);
}

bool isTrackedLive(void *address)
{
	int index;

	index = find_allocation(address);
	return (index >= 0 && allocations[index].live);
}

bool showLeaks(void)
{
	size_t i;
	size_t live_count;
	char	message[128];

	i = 0;
	live_count = 0;
	while (i < MAX_TRACKED_ALLOCATIONS)
	{
		if (allocations[i].known && allocations[i].live)
		{
			live_count++;
			snprintf(message, sizeof(message), "\033[31mLEAK [%p : %zu]\033[0m\n",
				allocations[i].address, allocations[i].size);
			report_text(message);
		}
		i++;
	}
	if (allocation_table_overflow)
		report_text("\033[31mALLOCATION TRACKER OVERFLOW\033[0m\n");
	return (live_count == 0 && !allocation_table_overflow);
}

#ifdef __APPLE__
extern "C" void *malloc(size_t size)
#endif
#ifdef __unix__
extern "C" void *malloc(size_t size) throw()
#endif
{
	void *result;

	if (!next_malloc)
		resolve_symbols();
	if (!next_malloc)
		return (NULL);
	if (call_active)
	{
		active_stats.malloc_calls++;
		if (active_options.fail_malloc_at == active_stats.malloc_calls)
		{
			active_stats.malloc_failure_injected = true;
			errno = ENOMEM;
			return (NULL);
		}
	}
	result = next_malloc(size);
	if (call_active && result)
		remember_allocation(result, size);
	return (result);
}

#ifdef __APPLE__
extern "C" void free(void *address)
#endif
#ifdef __unix__
extern "C" void free(void *address) throw()
#endif
{
	int index;

	if (!next_free)
		resolve_symbols();
	if (!next_free)
		return ;
	if (!address)
	{
		next_free(address);
		return ;
	}
	if (call_active)
		active_stats.free_calls++;
	index = find_allocation(address);
	if (index >= 0)
	{
		if (!allocations[index].live)
		{
			if (call_active)
				active_stats.lifetime_errors++;
			return ;
		}
		allocations[index].live = false;
		next_free(address);
		return ;
	}
	if (call_active)
	{
		active_stats.lifetime_errors++;
		return ;
	}
	next_free(address);
}

extern "C" ssize_t read(int fd, void *buffer, size_t count)
{
	size_t forwarded_count;

	if (!next_read)
		resolve_symbols();
	if (!next_read)
		return (-1);
	forwarded_count = count;
	if (call_active)
	{
		active_stats.read_calls++;
		if (count != (size_t)BUFFER_SIZE)
			active_stats.wrong_read_sizes++;
		if (active_options.fail_read_at == active_stats.read_calls)
		{
			active_stats.read_failure_injected = true;
			errno = active_options.fail_read_errno;
			return (-1);
		}
		if (active_options.max_read_size > 0
			&& forwarded_count > active_options.max_read_size)
			forwarded_count = active_options.max_read_size;
	}
	return (next_read(fd, buffer, forwarded_count));
}
