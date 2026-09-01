#!/bin/sh
#
# Benchmark harness: builds each kernel with clang (LLVM), gcc, and feather
# (QBE IL), times the binaries best-of-5, and checks that feather's checksum
# matches clang -O2's (the correctness reference).
#
# Usage: REPS=2000 ./run.sh [kernel ...]
#   default kernels: the KERNELS list below

set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)
FEATHER=${FEATHER:-$ROOT/bin/feather}
DRIVER=$ROOT/bench/driver.c
REPS=${REPS:-2000}
KERNELS="$@"
if [ -z "$KERNELS" ]; then
	KERNELS="sum"
fi
CLANG=${CLANG:-clang}
GCC=${GCC:-gcc}
CC=${CC:-cc}

TMP=$(mktemp -d)
trap 'rm -rf $TMP' EXIT

# best_of <binary> -> prints "<best_time> <checksum>"
best_of() {
	bin="$1"
	best=""
	chk=""
	for _ in 1 2 3 4 5; do
		out=$("$bin" "$REPS")
		t=$(printf '%s' "$out" | sed -n 's/.*time=\([0-9.]*\).*/\1/p')
		c=$(printf '%s' "$out" | sed -n 's/.*checksum=\([-0-9]*\).*/\1/p')
		chk="$c"
		if [ -z "$best" ] || awk "BEGIN{exit !($t < $best)}"; then
			best="$t"
		fi
	done
	echo "$best $chk"
}

for k in $KERNELS; do
	cf=$ROOT/bench/$k.c
	sf=$ROOT/bench/$k.ssa
	if [ ! -f "$cf" ] || [ ! -f "$sf" ]; then
		echo "!! skip $k: missing $cf or $sf"
		continue
	fi
	echo "=== kernel: $k (reps=$REPS) ==="

	# LLVM / GCC via C
	$CLANG -O2 -o $TMP/${k}_clangO2 $cf $DRIVER
	$CLANG -O3 -o $TMP/${k}_clangO3 $cf $DRIVER
	$GCC   -O2 -o $TMP/${k}_gccO2   $cf $DRIVER

	# feather via QBE IL: IL -> asm -> link with driver
	$FEATHER -o $TMP/${k}.s $sf
	$CC -o $TMP/${k}_feather $TMP/${k}.s $DRIVER

	r_clang=$(best_of $TMP/${k}_clangO2)
	r_clang3=$(best_of $TMP/${k}_clangO3)
	r_gcc=$(best_of $TMP/${k}_gccO2)
	r_feather=$(best_of $TMP/${k}_feather)

	c_clang=$(echo $r_clang | cut -d' ' -f2)
	c_feather=$(echo $r_feather | cut -d' ' -f2)

	printf "  clang -O2 : %s s   (chk %s)\n" "$(echo $r_clang | cut -d' ' -f1)" "$c_clang"
	printf "  clang -O3 : %s s   (chk %s)\n" "$(echo $r_clang3 | cut -d' ' -f1)" "$(echo $r_clang3 | cut -d' ' -f2)"
	printf "  gcc   -O2 : %s s   (chk %s)\n" "$(echo $r_gcc | cut -d' ' -f1)" "$(echo $r_gcc | cut -d' ' -f2)"
	printf "  feather      : %s s   (chk %s)\n" "$(echo $r_feather | cut -d' ' -f1)" "$c_feather"

	if [ "$c_clang" = "$c_feather" ]; then
		echo "  CHECKSUM: OK (feather == clang -O2)"
	else
		echo "  CHECKSUM: MISMATCH (feather=$c_feather clang=$c_clang)  <-- correctness bug"
	fi
	echo
done
