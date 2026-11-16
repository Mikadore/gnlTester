# gnlTester

An executable test suite for the 42 `get_next_line` project. It checks the mandatory and bonus implementations with `BUFFER_SIZE` values of 1, 42, and 10,000,000. On Linux, it also runs Valgrind when Valgrind is installed.

## Run it

Place `gnlTester` beside the project directory, then run:

```sh
cd gnlTester
make m        # mandatory tests
make b        # bonus tests
make a        # both suites
```

If the source directory is elsewhere, pass it explicitly:

```sh
make a GNL_DIR=/path/to/get_next_line
```

The test commands no longer run `git pull`; they only build and execute the checked out tester. A failed check or crash makes `make` return a nonzero status.

## Coverage

- Empty input, newline-only input, files with and without a final newline, long lines, repeated lines, and lengths around the common 42-byte buffer boundary.
- Standard input, pipes, and interleaved bonus reads from several descriptors.
- Interposed `read` calls: every request made by `get_next_line` must use exactly `BUFFER_SIZE`. The wrapper can force short positive reads, `EINTR`, and other read errors.
- Interposed `malloc` and `free`: failures can be injected at each allocation in a call. The suite checks for leaks, invalid or repeated frees, and a returned pointer that was already freed.
- Isolated test processes with timeout and crash detection.

The fault-injection controls are in `utils/leaks.hpp`; additional cases are in `tests/faults.cpp`. The wrappers forward to the system functions using `dlsym(RTLD_NEXT, ...)` and only apply assertions or fault injection while a `get_next_line` call is active.
