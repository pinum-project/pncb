/*
 * Benchmark driver for the pncb backend.
 * Calls bench(i) repeatedly and prints a checksum (anti-DCE) and wall time.
 * The actual kernel is provided by the linked object (C or QBE IL).
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern long bench(long);

int
main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s reps\n", argv[0]);
		return 1;
	}
	long reps = atol(argv[1]);
	long chk = 0;
	clock_t t0 = clock();
	/* vary the input so the optimizer cannot fold bench away */
	for (long i = 1; i <= reps; i++)
		chk += bench(i);
	clock_t t1 = clock();
	double secs = (double)(t1 - t0) / CLOCKS_PER_SEC;
	printf("checksum=%ld time=%.6f\n", chk, secs);
	return 0;
}
