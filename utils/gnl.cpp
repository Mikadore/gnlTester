extern "C"
{
#define new tripouille
#include "get_next_line.h"
#undef new
}

#include "gnl.hpp"
#include "color.hpp"
#include "check.hpp"

#include <string.h>

extern int iTest;

using namespace std;

HookStats gnl(int fd, char const *expectedReturn, const HookOptions &options)
{
	char	*gnlReturn;
	HookStats stats;
	bool	return_matches;
	bool	return_is_live;
	bool	call_ok;

	beginGnlCall(options);
	gnlReturn = get_next_line(fd);
	stats = endGnlCall();
	call_ok = true;
	if (stats.wrong_read_sizes != 0)
	{
		check(false, "read size must equal BUFFER_SIZE");
		call_ok = false;
	}
	if (stats.lifetime_errors != 0)
	{
		check(false, "invalid or double free");
		call_ok = false;
	}
	return_is_live = (!gnlReturn || isTrackedLive(gnlReturn));
	if (!return_is_live)
	{
		check(false, "returned line must be live and allocated");
		call_ok = false;
	}
	if (expectedReturn == NULL)
		return_matches = (gnlReturn == NULL);
	else
		return_matches = (return_is_live && gnlReturn
			&& !strcmp(gnlReturn, expectedReturn));
	if (!return_matches)
	{
		check(false, "return value");
		call_ok = false;
	}
	if (call_ok)
		check(true);
	if (return_is_live)
		free(gnlReturn);
	iTest++;
	return (stats);
}
