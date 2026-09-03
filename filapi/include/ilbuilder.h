/*
 * Original feather Codebase
 * Copyright (c) 2026-present Quil Project Authors
 *
 * Released under the MIT License.
 */

#ifndef FILAPI_ILBUILDER_H
#define FILAPI_ILBUILDER_H

#include "../../all.h"

/* ILBuilder - insertion point wrapper like LLVM IRBuilder<> */
typedef struct ILBuilder ILBuilder;
struct ILBuilder {
        Fn *fn;   /* current function */
        Blk *cur; /* insertion block */
};

/* lifecycle */
ILBuilder *il_create(Fn *fn);                                  /* create builder for fn */
void il_destroy(ILBuilder *bd);                                /* free builder (not fn) */
void il_set_insert_point(ILBuilder *bd, Blk *blk);             /* set cur block */
Blk *il_get_insert_block(ILBuilder *bd);                       /* get cur block */
Blk *il_create_block(ILBuilder *bd, const char *name);         /* new block @name */
Fn *il_create_function(const char *name, int retty, Lnk *lnk); /* alloc new Fn */
Fn *il_finish(ILBuilder *bd);                                  /* finalize mem/rpo */

/* constants */
Ref il_const_int_w(ILBuilder *bd, int32_t v);  /* w constant */
Ref il_const_int_l(ILBuilder *bd, int64_t v);  /* l constant */
Ref il_const_float_s(ILBuilder *bd, float v);  /* s constant */
Ref il_const_float_d(ILBuilder *bd, double v); /* d constant */
Ref il_const_undef(ILBuilder *bd);             /* UNDEF */
Ref il_const_zero(ILBuilder *bd);              /* 0 */

/* arithmetic T(T,T) / T(T) */
Ref il_create_add_w(ILBuilder *bd, Ref a, Ref b);  /* w add */
Ref il_create_add_l(ILBuilder *bd, Ref a, Ref b);  /* l add */
Ref il_create_add_s(ILBuilder *bd, Ref a, Ref b);  /* s add */
Ref il_create_add_d(ILBuilder *bd, Ref a, Ref b);  /* d add */
Ref il_create_sub_w(ILBuilder *bd, Ref a, Ref b);  /* w sub */
Ref il_create_sub_l(ILBuilder *bd, Ref a, Ref b);  /* l sub */
Ref il_create_sub_s(ILBuilder *bd, Ref a, Ref b);  /* s sub */
Ref il_create_sub_d(ILBuilder *bd, Ref a, Ref b);  /* d sub */
Ref il_create_mul_w(ILBuilder *bd, Ref a, Ref b);  /* w mul */
Ref il_create_mul_l(ILBuilder *bd, Ref a, Ref b);  /* l mul */
Ref il_create_mul_s(ILBuilder *bd, Ref a, Ref b);  /* s mul */
Ref il_create_mul_d(ILBuilder *bd, Ref a, Ref b);  /* d mul */
Ref il_create_div_w(ILBuilder *bd, Ref a, Ref b);  /* w signed div */
Ref il_create_div_l(ILBuilder *bd, Ref a, Ref b);  /* l signed div */
Ref il_create_div_s(ILBuilder *bd, Ref a, Ref b);  /* s div */
Ref il_create_div_d(ILBuilder *bd, Ref a, Ref b);  /* d div */
Ref il_create_udiv_w(ILBuilder *bd, Ref a, Ref b); /* w unsigned div */
Ref il_create_udiv_l(ILBuilder *bd, Ref a, Ref b); /* l unsigned div */
Ref il_create_rem_w(ILBuilder *bd, Ref a, Ref b);  /* w signed rem */
Ref il_create_rem_l(ILBuilder *bd, Ref a, Ref b);  /* l signed rem */
Ref il_create_urem_w(ILBuilder *bd, Ref a, Ref b); /* w unsigned rem */
Ref il_create_urem_l(ILBuilder *bd, Ref a, Ref b); /* l unsigned rem */
Ref il_create_neg_w(ILBuilder *bd, Ref a);         /* w neg */
Ref il_create_neg_l(ILBuilder *bd, Ref a);         /* l neg */
Ref il_create_neg_s(ILBuilder *bd, Ref a);         /* s neg */
Ref il_create_neg_d(ILBuilder *bd, Ref a);         /* d neg */

/* bitwise I(I,I) / I(I,ww) */
Ref il_create_and_w(ILBuilder *bd, Ref a, Ref b); /* w and */
Ref il_create_and_l(ILBuilder *bd, Ref a, Ref b); /* l and */
Ref il_create_or_w(ILBuilder *bd, Ref a, Ref b);  /* w or */
Ref il_create_or_l(ILBuilder *bd, Ref a, Ref b);  /* l or */
Ref il_create_xor_w(ILBuilder *bd, Ref a, Ref b); /* w xor */
Ref il_create_xor_l(ILBuilder *bd, Ref a, Ref b); /* l xor */
Ref il_create_shl_w(ILBuilder *bd, Ref a, Ref b); /* w shl */
Ref il_create_shl_l(ILBuilder *bd, Ref a, Ref b); /* l shl */
Ref il_create_shr_w(ILBuilder *bd, Ref a, Ref b); /* w shr (logical) */
Ref il_create_shr_l(ILBuilder *bd, Ref a, Ref b); /* l shr (logical) */
Ref il_create_sar_w(ILBuilder *bd, Ref a, Ref b); /* w sar (arith) */
Ref il_create_sar_l(ILBuilder *bd, Ref a, Ref b); /* l sar (arith) */

/* memory */
Ref il_create_alloc4(ILBuilder *bd, Ref n);                      /* alloc aligned 4 */
Ref il_create_alloc8(ILBuilder *bd, Ref n);                      /* alloc aligned 8 */
Ref il_create_alloc16(ILBuilder *bd, Ref n);                     /* alloc aligned 16 */
Ref il_create_load_w(ILBuilder *bd, Ref addr);                   /* load word */
Ref il_create_load_l(ILBuilder *bd, Ref addr);                   /* load long */
Ref il_create_load_s(ILBuilder *bd, Ref addr);                   /* load single */
Ref il_create_load_d(ILBuilder *bd, Ref addr);                   /* load double */
Ref il_create_load_sb(ILBuilder *bd, Ref addr);                  /* load signed byte -> w */
Ref il_create_load_ub(ILBuilder *bd, Ref addr);                  /* load unsigned byte -> w */
Ref il_create_load_sh(ILBuilder *bd, Ref addr);                  /* load signed half -> w */
Ref il_create_load_uh(ILBuilder *bd, Ref addr);                  /* load unsigned half -> w */
Ref il_create_load_sw(ILBuilder *bd, Ref addr);                  /* load signed word -> l/w */
Ref il_create_load_uw(ILBuilder *bd, Ref addr);                  /* load unsigned word -> l/w */
void il_create_store_w(ILBuilder *bd, Ref val, Ref addr);        /* store word */
void il_create_store_l(ILBuilder *bd, Ref val, Ref addr);        /* store long */
void il_create_store_s(ILBuilder *bd, Ref val, Ref addr);        /* store single */
void il_create_store_d(ILBuilder *bd, Ref val, Ref addr);        /* store double */
void il_create_store_b(ILBuilder *bd, Ref val, Ref addr);        /* store byte */
void il_create_store_h(ILBuilder *bd, Ref val, Ref addr);        /* store half */
void il_create_blit(ILBuilder *bd, Ref dst, Ref src, int64_t n); /* memcpy n bytes */

/* conversions / casts */
Ref il_create_extsb_w(ILBuilder *bd, Ref a);  /* sign-extend byte -> w */
Ref il_create_extsb_l(ILBuilder *bd, Ref a);  /* sign-extend byte -> l */
Ref il_create_extub_w(ILBuilder *bd, Ref a);  /* zero-extend byte -> w */
Ref il_create_extub_l(ILBuilder *bd, Ref a);  /* zero-extend byte -> l */
Ref il_create_extsh_w(ILBuilder *bd, Ref a);  /* sign-extend half -> w */
Ref il_create_extsh_l(ILBuilder *bd, Ref a);  /* sign-extend half -> l */
Ref il_create_extuh_w(ILBuilder *bd, Ref a);  /* zero-extend half -> w */
Ref il_create_extuh_l(ILBuilder *bd, Ref a);  /* zero-extend half -> l */
Ref il_create_extsw_l(ILBuilder *bd, Ref a);  /* sign-extend word -> l */
Ref il_create_extuw_l(ILBuilder *bd, Ref a);  /* zero-extend word -> l */
Ref il_create_exts_d(ILBuilder *bd, Ref a);   /* extend s -> d */
Ref il_create_truncd_s(ILBuilder *bd, Ref a); /* trunc d -> s */
Ref il_create_stosi_w(ILBuilder *bd, Ref a);  /* s -> signed w */
Ref il_create_stosi_l(ILBuilder *bd, Ref a);  /* s -> signed l */
Ref il_create_stoui_w(ILBuilder *bd, Ref a);  /* s -> unsigned w */
Ref il_create_stoui_l(ILBuilder *bd, Ref a);  /* s -> unsigned l */
Ref il_create_dtosi_w(ILBuilder *bd, Ref a);  /* d -> signed w */
Ref il_create_dtosi_l(ILBuilder *bd, Ref a);  /* d -> signed l */
Ref il_create_dtoui_w(ILBuilder *bd, Ref a);  /* d -> unsigned w */
Ref il_create_dtoui_l(ILBuilder *bd, Ref a);  /* d -> unsigned l */
Ref il_create_swtof_s(ILBuilder *bd, Ref a);  /* signed w -> s */
Ref il_create_swtof_d(ILBuilder *bd, Ref a);  /* signed w -> d */
Ref il_create_uwtof_s(ILBuilder *bd, Ref a);  /* unsigned w -> s */
Ref il_create_uwtof_d(ILBuilder *bd, Ref a);  /* unsigned w -> d */
Ref il_create_sltof_s(ILBuilder *bd, Ref a);  /* signed l -> s */
Ref il_create_sltof_d(ILBuilder *bd, Ref a);  /* signed l -> d */
Ref il_create_ultof_s(ILBuilder *bd, Ref a);  /* unsigned l -> s */
Ref il_create_ultof_d(ILBuilder *bd, Ref a);  /* unsigned l -> d */
Ref il_create_cast_s(ILBuilder *bd, Ref a);   /* bitcast to s */
Ref il_create_cast_d(ILBuilder *bd, Ref a);   /* bitcast to d */
Ref il_create_cast_w(ILBuilder *bd, Ref a);   /* bitcast to w */
Ref il_create_cast_l(ILBuilder *bd, Ref a);   /* bitcast to l */
Ref il_create_copy_w(ILBuilder *bd, Ref a);   /* copy w */
Ref il_create_copy_l(ILBuilder *bd, Ref a);   /* copy l */
Ref il_create_copy_s(ILBuilder *bd, Ref a);   /* copy s */
Ref il_create_copy_d(ILBuilder *bd, Ref a);   /* copy d */

/* comparisons I(ww,ww) / I(ss,ss) / I(dd,dd) -> w  (1 if true) */
Ref il_create_icmp_eq_w(ILBuilder *bd, Ref a, Ref b);  /* w == */
Ref il_create_icmp_ne_w(ILBuilder *bd, Ref a, Ref b);  /* w != */
Ref il_create_icmp_sge_w(ILBuilder *bd, Ref a, Ref b); /* w signed >= */
Ref il_create_icmp_sgt_w(ILBuilder *bd, Ref a, Ref b); /* w signed > */
Ref il_create_icmp_sle_w(ILBuilder *bd, Ref a, Ref b); /* w signed <= */
Ref il_create_icmp_slt_w(ILBuilder *bd, Ref a, Ref b); /* w signed < */
Ref il_create_icmp_uge_w(ILBuilder *bd, Ref a, Ref b); /* w unsigned >= */
Ref il_create_icmp_ugt_w(ILBuilder *bd, Ref a, Ref b); /* w unsigned > */
Ref il_create_icmp_ule_w(ILBuilder *bd, Ref a, Ref b); /* w unsigned <= */
Ref il_create_icmp_ult_w(ILBuilder *bd, Ref a, Ref b); /* w unsigned < */
Ref il_create_icmp_eq_l(ILBuilder *bd, Ref a, Ref b);  /* l == */
Ref il_create_icmp_ne_l(ILBuilder *bd, Ref a, Ref b);  /* l != */
Ref il_create_icmp_sge_l(ILBuilder *bd, Ref a, Ref b); /* l signed >= */
Ref il_create_icmp_sgt_l(ILBuilder *bd, Ref a, Ref b); /* l signed > */
Ref il_create_icmp_sle_l(ILBuilder *bd, Ref a, Ref b); /* l signed <= */
Ref il_create_icmp_slt_l(ILBuilder *bd, Ref a, Ref b); /* l signed < */
Ref il_create_icmp_uge_l(ILBuilder *bd, Ref a, Ref b); /* l unsigned >= */
Ref il_create_icmp_ugt_l(ILBuilder *bd, Ref a, Ref b); /* l unsigned > */
Ref il_create_icmp_ule_l(ILBuilder *bd, Ref a, Ref b); /* l unsigned <= */
Ref il_create_icmp_ult_l(ILBuilder *bd, Ref a, Ref b); /* l unsigned < */
Ref il_create_fcmp_eq_s(ILBuilder *bd, Ref a, Ref b);  /* s == */
Ref il_create_fcmp_ne_s(ILBuilder *bd, Ref a, Ref b);  /* s != */
Ref il_create_fcmp_ge_s(ILBuilder *bd, Ref a, Ref b);  /* s >= */
Ref il_create_fcmp_gt_s(ILBuilder *bd, Ref a, Ref b);  /* s > */
Ref il_create_fcmp_le_s(ILBuilder *bd, Ref a, Ref b);  /* s <= */
Ref il_create_fcmp_lt_s(ILBuilder *bd, Ref a, Ref b);  /* s < */
Ref il_create_fcmp_o_s(ILBuilder *bd, Ref a, Ref b);   /* s ordered (no NaN) */
Ref il_create_fcmp_uo_s(ILBuilder *bd, Ref a, Ref b);  /* s unordered (NaN) */
Ref il_create_fcmp_eq_d(ILBuilder *bd, Ref a, Ref b);  /* d == */
Ref il_create_fcmp_ne_d(ILBuilder *bd, Ref a, Ref b);  /* d != */
Ref il_create_fcmp_ge_d(ILBuilder *bd, Ref a, Ref b);  /* d >= */
Ref il_create_fcmp_gt_d(ILBuilder *bd, Ref a, Ref b);  /* d > */
Ref il_create_fcmp_le_d(ILBuilder *bd, Ref a, Ref b);  /* d <= */
Ref il_create_fcmp_lt_d(ILBuilder *bd, Ref a, Ref b);  /* d < */
Ref il_create_fcmp_o_d(ILBuilder *bd, Ref a, Ref b);   /* d ordered */
Ref il_create_fcmp_uo_d(ILBuilder *bd, Ref a, Ref b);  /* d unordered */

/* control */
void il_create_br(ILBuilder *bd, Blk *dst);                                    /* jmp dst */
void il_create_cond_br(ILBuilder *bd, Ref cond, Blk *then_blk, Blk *else_blk); /* jnz cond, then, else */
void il_create_ret_w(ILBuilder *bd, Ref v);                                    /* ret w */
void il_create_ret_l(ILBuilder *bd, Ref v);                                    /* ret l */
void il_create_ret_s(ILBuilder *bd, Ref v);                                    /* ret s */
void il_create_ret_d(ILBuilder *bd, Ref v);                                    /* ret d */
void il_create_ret_void(ILBuilder *bd);                                        /* ret void */
void il_create_unreachable(ILBuilder *bd);                                     /* hlt */

/* phi / select / call / variadic */
Ref il_create_phi_w(ILBuilder *bd, Blk *blks[], Ref vals[], int n); /* phi w */
Ref il_create_phi_l(ILBuilder *bd, Blk *blks[], Ref vals[], int n); /* phi l */
Ref il_create_phi_s(ILBuilder *bd, Blk *blks[], Ref vals[], int n); /* phi s */
Ref il_create_phi_d(ILBuilder *bd, Blk *blks[], Ref vals[], int n); /* phi d */
Ref il_create_call_w(ILBuilder *bd, Ref fn, Ref args[], int nargs); /* call -> w */
Ref il_create_call_l(ILBuilder *bd, Ref fn, Ref args[], int nargs); /* call -> l */
Ref il_create_call_s(ILBuilder *bd, Ref fn, Ref args[], int nargs); /* call -> s */
Ref il_create_call_d(ILBuilder *bd, Ref fn, Ref args[], int nargs); /* call -> d */
void il_create_vastart(ILBuilder *bd, Ref ap);                      /* vastart ap */
Ref il_create_vaarg_w(ILBuilder *bd, Ref ap);                       /* vaarg w */
Ref il_create_vaarg_l(ILBuilder *bd, Ref ap);                       /* vaarg l */
Ref il_create_vaarg_s(ILBuilder *bd, Ref ap);                       /* vaarg s */
Ref il_create_vaarg_d(ILBuilder *bd, Ref ap);                       /* vaarg d */

/* generic helpers (type-param) */
Ref il_create_add(ILBuilder *bd, int cls, Ref a, Ref b);         /* add with cls Kw/Kl/Ks/Kd */
Ref il_create_load(ILBuilder *bd, int cls, Ref addr);            /* load with cls */
void il_create_store(ILBuilder *bd, int cls, Ref val, Ref addr); /* store with cls */

#endif
