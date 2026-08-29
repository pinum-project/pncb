/*
 * dot kernel (C reference).
 * Fills two arrays from rand() so the data is unknown at compile time
 * (defeats constant folding), then does a dependent multiply-accumulate.
 * Mirrors bench/dot.ssa exactly so checksums must match.
 */
#include <stdlib.h>

long
bench(long n)
{
	enum { N = 1024 };
	long a[N], b[N];
	int i;

	for (i = 0; i < N; i++) {
		a[i] = rand();
		b[i] = rand();
	}
	long s = 0;
	for (long k = 0; k <= n; k++) {
		long idx = k & (N - 1);
		s += a[idx] * b[idx];
	}
	return s;
}
