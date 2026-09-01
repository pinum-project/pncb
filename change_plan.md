# QBE Optimization — Change Plan (quilcb backend)

Goal: make QBE-generated code fast enough to rival LLVM for the quilcb
compiler backend (runtime perf target; not trying to beat LLVM outright).

## Bugs found (do not lose these)

### BUG-1 [CONFIRMED QBE CORRECTNESS BUG — silent wrong code]
Loop transformation drops the final iteration.

- Symptom: a bottom-checked loop of the form
    load/compute body; idx++; if (idx < n) continue;
  produces code where the exit test (`cmp`/`jge`) is hoisted ABOVE the
  body's trailing multiply/accumulate, so for the last index the element
  is loaded but never accumulated.
- Repro: bench/dot.ssa (original version, before the top-checked rewrite).
  Generated asm had `jge .Lbb6` (exit) BEFORE `imulq` (multiply). Sum was
  short by exactly a[last]*b[last].
- Trigger: any loop whose exit test is placed AFTER the index increment
  and whose body has a trailing op that the scheduler floats the compare
  ahead of. Classic do-while-turned-while rotation bug in gcm/isel.
- Impact: SILENT wrong numerical results. High severity for a backend.
- Workaround used: rewrite as a top-checked `for` loop
  (check `idx < n` BEFORE the body). The top-checked form codegens
  correctly.
- TODO: root-cause in gcm.c (loop rotation / code motion) and add a
  regression test under test/.

### BUG-2 [SUSPECTED QBE MISCOMPILE — needs minimal repro]
Cross-loop phi sum accumulation loses iterations.

- Symptom: carrying a running total through nested loops via a single
  phi chain (inner-exit: `sumx = sumc + it0`; outer-phi: `sum1 = sumx`)
  caused QBE to accumulate the add only ONCE per outer iteration
  (added only the final pixel's `it`), massively under-counting.
- Repro: first version of bench/mandel.ssa.
- Workaround used: give the inner loop its OWN local accumulator
  (`xsum` phi inside x-loop) and carry only the row subtotal across the
  outer loop. That form is correct.
- TODO: confirm whether this is a QBE phi/SSA bug or an IL mistake.
  Build a minimal single/dual-loop sum repro and diff against clang.

### IL authoring pitfalls (NOT QBE bugs — logged to avoid churn)
These were my mistakes writing QBE IL by hand; record so we don't
waste time re-debugging them:
- Function header is `function l $name(t %a) {` — return type BEFORE
  the name, NO trailing type before `{`. (`function l $bench(l %i) l {`
  fails with "function body must start with {", but ONLY the trailing
  ` l` is the error.)
- `export` must be on its own line, then `function ...` on the next.
- There is no `loadb`; use `loadub` (or `loadsb`) for byte loads.
  Stores: `storeb`.
- There is no `cltl`; for signed long compare use `csltl` (unsigned
  `cultl`). `cltd` is the DOUBLE (float) less-than.
- Phi predecessors must exactly match the block's actual predecessors
  (e.g., `@sieve` is reached from `@init`, not `@start`).
- Single-precision float constant literal is `s_0.5`; double is `d_0.5`.
- `sltof` converts long -> single (float), not double. `stosi` converts
  float -> long for the final return.

## Benchmark methodology (current)
- bench/driver.c: `main` reads `reps` from argv[1], loops calling
  `bench(i)`, prints a checksum (prevents dead-code elimination).
- bench/run.sh: builds a full EXECUTABLE for each kernel with
  clang -O2, clang -O3, gcc -O2, and QBE, then times the actual
  binaries externally (best-of-5 wall clock) and checks the QBE
  checksum matches clang -O2.
- Kernels (matched C source + QBE IL):
  - sieve : integer + memory (Eratosthenes), n varies with i.
  - mandel: double FP escape-time over a grid; NON-vectorizable toy,
            not representative of real loops.
  - dot   : float dot product with non-reducible fill; clang vectorizes
            (mulps), QBE stays scalar.

## Optimization plan (from earlier analysis)
Tier 0 (in quilcb lowering — cheapest, biggest wins, no QBE changes):
  - inlining (QBE has none), SROA / scalar replacement, trivial LICM.
Tier 1 (QBE mid-end passes):
  - LICM using existing fillloop (cfg.c) infrastructure — biggest missing opt.
  - PRE over gvn.c (kill partially-redundant loads in loops).
  - strength reduction / induction variables.
Tier 2 (register allocation, rega.c / spill.c):
  - cut spills in loops (spills destroy loop perf).
Tier 3 (codegen, e.g. amd64/isel.c):
  - indexed addressing modes (base+idx*scale) for array indexing.
Realistic target: within ~10-30% of LLVM -O2 once inlining+SROA+loop
opts land. Beating LLVM outright needs vectorization.
