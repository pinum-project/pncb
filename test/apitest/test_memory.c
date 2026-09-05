#include "../../filapi/include/ilbuilder.h"
#include "../../config.h"
#include <stdio.h>

Target T;
char debug['Z' + 1];
int optlevel = 0;
extern Target T_amd64_sysv;

static int check_op(Blk *b,int idx,int op,const char *name){
  if(idx >= (int)b->nins){fprintf(stderr,"missing %s at %d nins %d\n",name,idx,b->nins);return 0;}
  if(b->ins[idx].op != op){fprintf(stderr,"%s op %d want %d\n",name,b->ins[idx].op,op);return 0;}
  return 1;
}
int main(void){
  T=T_amd64_sysv;
  Lnk lnk={.export=1};
  Fn *fn=il_create_function("mem",Kw,&lnk);
  ILBuilder *bd=il_create(fn);
  Blk *b=il_create_block(bd,"start");
  il_set_insert_point(bd,b);

  Ref sz=il_const_int_w(bd,16);
  Ref ptr=il_create_alloc4(bd,sz);
  if(!check_op(b,0,Oalloc4,"alloc4")) return 1;

  Ref val=il_const_int_w(bd,0x41424344);
  il_create_store_w(bd,val,ptr);
  if(!check_op(b,1,Ostorew,"store_w")) return 1;

  Ref loaded=il_create_load_w(bd,ptr);
  if(!check_op(b,2,Oloadsw,"load_w")) return 1;

  Ref sb=il_create_load_sb(bd,ptr);
  if(!check_op(b,3,Oloadsb,"load_sb")) return 1;

  Ref ub=il_create_load_ub(bd,ptr);
  if(!check_op(b,4,Oloadub,"load_ub")) return 1;

  Ref sh=il_create_load_sh(bd,ptr);
  if(!check_op(b,5,Oloadsh,"load_sh")) return 1;

  Ref uh=il_create_load_uh(bd,ptr);
  if(!check_op(b,6,Oloaduh,"load_uh")) return 1;

  Ref sw=il_create_load_sw(bd,ptr);
  if(!check_op(b,7,Oloadsw,"load_sw")) return 1;

  Ref uw=il_create_load_uw(bd,ptr);
  if(!check_op(b,8,Oloaduw,"load_uw")) return 1;

  Ref sl=il_create_load_s(bd,ptr);
  if(!check_op(b,9,Oload,"load_s")) return 1;

  Ref dl=il_create_load_d(bd,ptr);
  if(!check_op(b,10,Oload,"load_d")) return 1;

  il_create_store_b(bd,val,ptr);
  if(!check_op(b,11,Ostoreb,"store_b")) return 1;

  il_create_store_h(bd,val,ptr);
  if(!check_op(b,12,Ostoreh,"store_h")) return 1;

  Ref dst=il_create_alloc4(bd,il_const_int_w(bd,16));
  Ref src=il_create_alloc4(bd,il_const_int_w(bd,16));
  il_create_blit(bd,dst,src,16);
  // blit emits 2 instructions: Oblit0 + Oblit1 at idx 15,16
  if(b->nins != 17 || b->ins[15].op != Oblit0 || b->ins[16].op != Oblit1){
    fprintf(stderr,"blit failed nins=%d op15=%d op16=%d\n",b->nins,b->ins[15].op,b->ins[16].op);
    return 1;
  }

  b->jmp.type=Jretw;
  b->jmp.arg=loaded;
  il_finish(bd);

  printf("test_memory...                             [ok]\n");
  return 0;
}
