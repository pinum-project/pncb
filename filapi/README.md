# Feather IL API (filapi)

`filapi` is an LLVM IRBuilder-style C library for programmatically generating Feather intermediate representation (IL) instructions and control flow. It allows frontends (such as the `quil` compiler) to emit IR cleanly without manually constructing raw instructions, temporaries, and basic blocks.

---

---

## API Summary

- **Lifecycle:** `il_create`, `il_destroy`, `il_set_insert_point`, `il_get_insert_block`, `il_create_block`, `il_create_function`, `il_finish`
- **Constants:** `il_const_int_w`, `il_const_int_l`, `il_const_float_s`, `il_const_float_d`, `il_const_undef`, `il_const_zero`
- **Arithmetic:** `il_create_add_*`, `il_create_sub_*`, `il_create_mul_*`, `il_create_div_*`, `il_create_udiv_*`, `il_create_rem_*`, `il_create_urem_*`, `il_create_neg_*`, `il_create_add`
- **Bitwise:** `il_create_and_*`, `il_create_or_*`, `il_create_xor_*`, `il_create_shl_*`, `il_create_shr_*`, `il_create_sar_*`
- **Memory:** `il_create_alloc4/8/16`, `il_create_load_*`, `il_create_store_*`, `il_create_blit`, `il_create_load`, `il_create_store`
- **Comparisons:** `il_create_icmp_*`, `il_create_fcmp_*`
- **Conversions / Casts:** `il_create_ext*`, `il_create_trunc*`, `il_create_stosi_*`, `il_create_dtosi_*`, `il_create_swtof_*`, `il_create_sltof_*`, `il_create_cast_*`, `il_create_copy_*`
- **Control Flow:** `il_create_br`, `il_create_cond_br`, `il_create_ret_*`, `il_create_ret_void`, `il_create_unreachable`
- **Phi, Calls & Varargs:** `il_create_phi_*`, `il_create_call_*`, `il_create_vastart`, `il_create_vaarg_*`

---

## Complete Example

Here is a complete example of creating a function that computes `a + b` using `filapi`:

```c
#include "include/ilbuilder.h"
#include <stdio.h>

// External target configuration required by feather
Target T;
char debug['Z' + 1];
int optlevel = 0;
extern Target T_amd64_sysv;

int main(void) {
    T = T_amd64_sysv;

    // 1. Initialize function and builder
    Lnk lnk = {.export = 1};
    Fn *fn = il_create_function("add_func", Kw, &lnk);
    ILBuilder *bd = il_create(fn);

    // 2. Create basic block and set insertion point
    Blk *entry = il_create_block(bd, "entry");
    il_set_insert_point(bd, entry);

    // 3. Emit instructions
    Ref a = il_const_int_w(bd, 10);
    Ref b = il_const_int_w(bd, 32);
    Ref sum = il_create_add_w(bd, a, b);

    // 4. Return statement
    il_create_ret_w(bd, sum);

    // 5. Finish building function
    fn = il_finish(bd);
    il_destroy(bd);

    printf("Function %s successfully generated!\n", fn->name);
    return 0;
}
```

## Running Tests

API tests are located in `test/apitest/` and can be run via:

```bash
make check-apitest
# or directly:
tools/apitest.sh all
`````
