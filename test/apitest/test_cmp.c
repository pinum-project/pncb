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
  Fn *fn = il_create_function("test_cmp", Kw, &lnk);
  ILBuilder *bd = il_create(fn);
  Blk *b = il_create_block(bd, "start");
  il_set_insert_point(bd, b);

  Ref a = il_const_int_w(bd, 5);
  Ref b_val = il_const_int_w(bd, 10);
  Ref cmp1 = il_create_icmp_slt_w(bd, a, b_val);
  Ref cmp2 = il_create_icmp_eq_w(bd, a, a);
  (void)cmp2;

  Ref fa = il_const_float_s(bd, 1.5f);
  Ref fb = il_const_float_s(bd, 2.5f);
  Ref fcmp = il_create_fcmp_lt_s(bd, fa, fb);
  (void)fcmp;

  b->jmp.type = Jretw;
  b->jmp.arg = cmp1;

  fn = il_finish(bd);

  int ok = (fn->nblk == 1 && b->nins >= 3 && b->jmp.type == Jretw);

  if(!ok){
    printfn(fn, stderr);
  }

  printf("test_cmp...%*s[%s]\n", (int)(45 - 9), "", ok ? "ok" : "FAIL");
  return ok ? 0 : 1;
}
