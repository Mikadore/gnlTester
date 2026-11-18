extern "C"
{
#ifdef TEST_BONUS
# include "get_next_line_bonus.h"
#else
# include "get_next_line.h"
#endif
# include "read_failure.h"
}

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

int	main(void)
{
	int				fd;
	int				attempt;
	int				reached_eof;
	unsigned int	before;
	char			*line;

	fd = open("files/big_line_no_nl", O_RDONLY);
	if (fd < 0)
		return (std::fprintf(stderr, "could not open read-error fixture\n"), 1);
	read_failure_start(0x42, 1, 5, 32);
	attempt = 0;
	reached_eof = 0;
	while (attempt++ < 10000)
	{
		before = read_failure_count();
		line = get_next_line(fd);
		if (read_failure_count() > before && line)
			return (std::fprintf(stderr, "read error did not return NULL\n"), 1);
		if (!line && read_failure_count() == before
			&& !read_failure_eof_seen())
			return (std::fprintf(stderr, "unexpected NULL before EOF\n"), 1);
		std::free(line);
		if (!line && read_failure_eof_seen())
		{
			reached_eof = 1;
			break ;
		}
	}
	read_failure_stop();
	close(fd);
	if (!reached_eof || read_failure_count() != 32
		|| read_failure_open_fd_count() != 32)
		return (std::fprintf(stderr, "read-error injection checks failed\n"), 1);
	std::printf("read-error injection passed: 32 EIO failures on an open fd\n");
	return (0);
}
