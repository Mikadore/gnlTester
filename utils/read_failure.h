#ifndef READ_FAILURE_H
# define READ_FAILURE_H

# include <stddef.h>
# include <sys/types.h>

void	read_failure_start(unsigned int seed, unsigned int min_interval,
			unsigned int max_interval, unsigned int max_failures);
void	read_failure_stop(void);
unsigned int	read_failure_count(void);
unsigned int	read_failure_open_fd_count(void);
int		read_failure_eof_seen(void);
ssize_t	gnl_test_read(int fd, void *buffer, size_t count);

#endif
