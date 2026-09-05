#include "../../filapi/include/ilbuilder.h"
#include "../../config.h"
#include <stdio.h>

Target T;
char debug['Z' + 1];
int optlevel = 0;
extern Target T_amd64_sysv;

int main(void){
  T = T_amd64_sysv;
  Lnk lnk = {.export=1};
  Fn *fn = il_create_function("test_phi_call_va", Kw, &lnk);
  ILBuilder *bd = il_create(fn);

  Blk *entry = il_create_block(bd, "entry");
  Blk *then_b = il_create_block(bd, "then");
  Blk *else_b = il_create_block(bd, "else");
  Blk *join = il_create_block(bd, "join");

  // Entry block
  il_set_insert_point(bd, entry);
  Ref cond = il_const_int_w(bd, 1);
  il_create_cond_br(bd, cond, then_b, else_b);

  // Then block
  il_set_insert_point(bd, then_b);
  Ref v1 = il_const_int_w(bd, 10);
  il_create_br(bd, join);

  // Else block
  il_set_insert_point(bd, else_b);
  Ref v2 = il_const_int_w(bd, 20);
  il_create_br(bd, join);

  // Join block (Phi node & call & vaarg)
  il_set_insert_point(bd, join);
  Blk *blks[] = {then_b, else_b};
  Ref vals[] = {v1, v2};
  Ref phi_val = il_create_phi_w(bd, blks, vals, 2);

  Ref callee = il_const_int_l(bd, 0); // placeholder address / symbol
  Ref args[] = {phi_val};
  Ref call_res = il_create_call_w(bd, callee, args, 1);

  Ref ap = il_create_alloc4(bd, il_const_int_w(bd, 8));
  il_create_vastart(bd, ap);
  Ref va_res = il_create_vaarg_w(bd, ap);
  (void)va_res;

  il_create_ret_w(bd, call_res);

  fn = il_finish(bd);

  int test_ok = (fn->nblk == 4 && join->nins >= 4);

  if(!test_ok){
    printfn(fn, stderr);
  }

  printf("test_phi_call_va...                          [%s]\n", test_ok ? "ok" : "FAIL");
  return test_ok ? 0 : 1;
}
