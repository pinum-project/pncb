/*
 * sieve kernel (C reference).
 * Counts primes <= N by sieve of Eratosthenes.
 * Mirrors bench/sieve.ssa exactly so checksums must match.
 */
#include <stdlib.h>
#include <string.h>

long
bench(long N)
{
	char *is = malloc(N + 1);
	memset(is, 1, N + 1);
	is[0] = 0;
	is[1] = 0;
	for (long i = 2; i * i <= N; i++)
		if (is[i])
			for (long j = i * i; j <= N; j += i)
				is[j] = 0;
	long cnt = 0;
	for (long k = 2; k <= N; k++)
		cnt += is[k];
	free(is);
	return cnt;
}
