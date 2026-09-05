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
  Fn *fn = il_create_function("test_ctrl", Kw, &lnk);
  ILBuilder *bd = il_create(fn);

  Blk *b_entry = il_create_block(bd, "entry");
  Blk *b_then = il_create_block(bd, "then");
  Blk *b_else = il_create_block(bd, "else");

  il_set_insert_point(bd, b_entry);
  Ref cond = il_const_int_w(bd, 1);
  il_create_cond_br(bd, cond, b_then, b_else);

  il_set_insert_point(bd, b_then);
  Ref v1 = il_const_int_w(bd, 10);
  il_create_ret_w(bd, v1);

  il_set_insert_point(bd, b_else);
  Ref v2 = il_const_int_w(bd, 20);
  il_create_ret_w(bd, v2);

  fn = il_finish(bd);

  int ok = (fn->nblk == 3 &&
            b_entry->jmp.type == Jjnz &&
            b_then->jmp.type == Jretw &&
            b_else->jmp.type == Jretw);

  if(!ok){
    printfn(fn, stderr);
  }

  printf("test_control...%*s[%s]\n", (int)(45 - 15), "", ok ? "ok" : "FAIL");
  return ok ? 0 : 1;
}
