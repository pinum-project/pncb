#!/bin/sh
#
# 5-second sieve throughput: clang (LLVM) vs pncb (QBE IL).
# bench(N) counts primes <= N; the driver runs it as many times as
# possible in ~5s and prints iters (throughput) + per-iteration checksum
# (correctness, independent of how many iterations each finished).
#
# Usage: N=100000000 ./sieve.sh

set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)
PNCB=${PNCB:-$ROOT/bin/pncb}
N=${N:-100000000}
CLANG=${CLANG:-clang}
CC=${CC:-cc}

TMP=$(mktemp -d)
trap 'rm -rf $TMP' EXIT

echo "=== sieve up to N=$N, 5s budget ==="

# LLVM via C
$CLANG -O2 -o $TMP/sieve_clang $ROOT/bench/sieve.c $ROOT/bench/sievedrv.c
r_clang=$($TMP/sieve_clang $N)

# pncb via QBE IL: IL -> asm -> link with driver
$PNCB -o $TMP/sieve.s $ROOT/bench/sieve.ssa
$CC -o $TMP/sieve_pncb $TMP/sieve.s $ROOT/bench/sievedrv.c
r_pncb=$($TMP/sieve_pncb $N)

echo "  clang : $r_clang"
echo "  pncb  : $r_pncb"

c_clang=$(echo "$r_clang" | sed -n 's/.*checksum=\([-0-9]*\).*/\1/p')
c_pncb=$(echo "$r_pncb" | sed -n 's/.*checksum=\([-0-9]*\).*/\1/p')
if [ "$c_clang" = "$c_pncb" ]; then
	echo "  CHECKSUM: OK (pncb == clang, $c_pncb primes <= N)"
else
	echo "  CHECKSUM: MISMATCH (pncb=$c_pncb clang=$c_clang)  <-- correctness bug"
fi
