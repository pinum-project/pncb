/*
 * Time-bounded driver for the sieve kernel.
 * Runs bench(N) as many times as possible in ~5 wall-clock seconds,
 * reports iteration count (throughput) and the PER-ITERATION checksum
 * (constant 5761455 for N=100M) so correctness is comparable regardless
 * of how many iterations each compiler finished.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern long bench(long);

int
main(int argc, char **argv)
{
	long N = (argc > 1) ? atol(argv[1]) : 100000000L;
	long iters = 0;
	long last = 0;
	time_t t0 = time(NULL);
	do {
		last = bench(N);
		iters++;
	} while (time(NULL) - t0 < 5);
	printf("N=%ld iters=%ld checksum=%ld\n", N, iters, last);
	return 0;
}
