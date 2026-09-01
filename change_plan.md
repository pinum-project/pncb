# QBE Optimization — Change Plan for -O1 vs clang -O3 (feather backend)

Goal: `feather -O1` within 10-30% of `clang -O3` on quilcb runtime kernels, correctness first.

## Current state (2026-09-01)

- Flag: `all.h:24` `extern int optlevel` + `#define OPTIMIZE (optlevel>=1)`, `main.c:16` `optlevel=1` default (tests expect optimized), `main.c:150` `getopt "hd:O::o:t:"` handles `-O`/`-O0`/`-O1`, `main.c:206` help `-O[level] default 1`.
- Pipeline `main.c:63` `func()`: `T.abi0` -> `promote`/`ssa`/`loadopt`/`coalesce` -> **always** `fillcfg/simplcfg` (folds `jnz 1` for isel `amd64/isel.c:623` `RTmp` assert, fixes `test/mem3.ssa:18`) -> `if(OPTIMIZE){ gvn/simplcfg/gcm/ifconvert }` -> `T.abi1/simpl/T.isel` -> `live/loop/cost/spill/rega` -> `emit`. `make check` `59/59 All is fine!` after fix.
- Bench `bench/run.sh:58` now builds both `feather -O0` and `-O1` vs `clang -O2/-O3`/`gcc -O2`, `best_of 5`, checksum vs `clang -O2`. `REPS=200 ./bench/run.sh dot` currently `O0/O1` both `OK -7858044056580261105`, `sieve.sh` `OK 5761455` — workarounds mask bugs.

## Bugs (keep)

### BUG-1 [CONFIRMED, now workarounded, needs repro]
Bottom-checked `do { load/mul/add; k++; } while(k<=n)` hoists `cmp/jle` before `imul` in `gcm.c`, drops last `a[last]*b[last]`. Repro was `bench/dot.ssa` bottom form, asm showed `jge` before `imul`. Workaround: top-checked `cslel k<=n` before body `bench/dot.ssa:7`. Attempted repro `/tmp/bug1_bottom.ssa` (bottom `jmp @body; phi @dot/@body; ret %s1`) now passes both `O0/O1` vs clang for `n=0..2000` after fixing `ret %s0->%s1` authoring bug — no miscompile observed, original rotation bug likely fixed or needs more precise `pinned` pattern. TODO: add minimal `test/bug1_gcm.ssa` regression under `test/` that fails on old `gcm`.

### BUG-2 [SUSPECTED, needs repro]
Nested phi `sumx = sumc+it` single chain across outer phi loses inner iters, only last pixel counted. Repro `bench/mandel.ssa` first version no longer exists, `test/mandel.ssa` is printer not accumulator. Workaround: inner `xsum` phi + row subtotal. TODO: minimal dual-loop sum repro.

### IL pitfalls
`function l $name` header, `export` newline, `loadub` not `loadb`, `csltl` not `cltl`, phi preds match `Blok.pred`, `s_0.5` vs `d_0.5`, `sltof` etc. — see prior version.

## Benchmark methodology (new)
- `bench/driver.c` anti-DCE checksum loop over `bench(i)` varying `i`.
- `bench/run.sh` `REPS=2000 ./run.sh [sum|dot|sieve]` builds `clang -O2/-O3`, `gcc -O2`, `feather -O0`, `feather -O1`, times best-of-5 wall, 2 checksum checks `O0==clang-O2` and `O1==clang-O2`.
- Target to beat: `clang -O3` (vectorization, LICM, unroll). Current gap largest on `dot` (scalar vs `mulps`), `sieve` memory, `sum` trivial.

## Plan to get -O1 competing with clang -O3

### Tier 0 — quilcb lowering (no QBE change, biggest win)
- [ ] Inlining (QBE has none) — inline hot `bench` callees.
- [ ] SROA / scalar replacement of `alloc8` aggregates (`mem.c` promote only covers simple slots).
- [ ] Trivial LICM in lowering for loop invariants.

### Tier 1 — QBE mid-end (inside `if(OPTIMIZE)`)
- [ ] LICM proper via `cfg.c:fillloop` + `gcm.c` — currently `gcm` only hoists, not full loop-invariant code motion for memory. Add `alias` `NoAlias` hoist for `load`.
- [ ] PRE over `gvn.c` — partially redundant loads in loops (kill reloads in `dot`/`sieve` inner).
- [ ] Strength reduction / IV opts — `mul 8*idx` -> `add` chain, `and 1023` mask folding (`copy.c`/`fold.c`/`simpl.c`).
- [ ] Loop rotation fix + test: ensure bottom-checked correctly handled (fix `gcm.c:112` `uselatebid` uninit warning).

### Tier 2 — Regalloc
- [ ] `spill.c`/`rega.c` — cut spills in loops (`fillcost` loop depth weight, `hint` avoid). Spills destroy `dot` inner loop.

### Tier 3 — Codegen
- [ ] `amd64/isel.c` indexed `Addr` `all.h:428` `base+idx*scale+disp` for `a[idx]` (`amatch` `Pobis` etc., already via `Num` `all.h:349` `runmatch`), ensure `dot` uses it not `mul+add`.
- [ ] `arm64`/`rv64` parity.
- [ ] Vectorization — long term; need to close 2x gap on `dot` where `clang -O3` uses `mulps`. Without, target 10-30% requires other tiers to compensate.

### Metrics
- Success = `feather -O1` time within 30% of `clang -O3` on `dot`/`sieve`/`sum` average, checksums `OK` for both `O0`/`O1`, `make check` `All is fine!` and `tools/test.sh` regression for BUG-1/2.

### Next steps
1. Land minimal repros `test/bug1_*.ssa` + `test/bug2_*.ssa` with `expected checksum` harness.
2. Implement Tier1 LICM/PRE behind `OPTIMIZE`, bench `REPS=2000` before/after.
3. Iterate Tier2/3, keep `bench/run.sh` dual `-O0`/`-O1` as gate.
