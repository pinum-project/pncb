#!/bin/sh
#
# 5-second sieve throughput: clang (LLVM) vs feather (QBE IL).
# bench(N) counts primes <= N; the driver runs it as many times as
# possible in ~5s and prints iters (throughput) + per-iteration checksum
# (correctness, independent of how many iterations each finished).
#
# Usage: N=100000000 ./sieve.sh

set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)
FEATHER=${FEATHER:-$ROOT/bin/feather}
N=${N:-100000000}
CLANG=${CLANG:-clang}
CC=${CC:-cc}

TMP=$(mktemp -d)
trap 'rm -rf $TMP' EXIT

echo "=== sieve up to N=$N, 5s budget ==="

# LLVM via C
$CLANG -O2 -o $TMP/sieve_clang $ROOT/bench/sieve.c $ROOT/bench/sievedrv.c
r_clang=$($TMP/sieve_clang $N)

# feather via QBE IL: IL -> asm -> link with driver
$FEATHER -o $TMP/sieve.s $ROOT/bench/sieve.ssa
$CC -o $TMP/sieve_feather $TMP/sieve.s $ROOT/bench/sievedrv.c
r_feather=$($TMP/sieve_feather $N)

echo "  clang : $r_clang"
echo "  feather  : $r_feather"

c_clang=$(echo "$r_clang" | sed -n 's/.*checksum=\([-0-9]*\).*/\1/p')
c_feather=$(echo "$r_feather" | sed -n 's/.*checksum=\([-0-9]*\).*/\1/p')
if [ "$c_clang" = "$c_feather" ]; then
	echo "  CHECKSUM: OK (feather == clang, $c_feather primes <= N)"
else
	echo "  CHECKSUM: MISMATCH (feather=$c_feather clang=$c_clang)  <-- correctness bug"
fi
