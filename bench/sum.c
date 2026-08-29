/*
 * sum kernel (C reference).
 * bench(n) = sum_{k=1..n} k*k
 * Mirrors bench/sum.ssa exactly so checksums must match.
 */
long
bench(long n)
{
	long r = 0;
	for (long k = 1; k <= n; k++)
		r += k * k;
	return r;
}
