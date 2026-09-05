/*
 * Original feather Codebase
 * Copyright (c) 2026-present Quil Project Authors
 *
 * Released under the MIT License.
 */

/* API for easy IL creation in the frontend */

#include "../include/ilbuilder.h"
#include <stdlib.h>
#include <string.h>

/*------------------ LIFECYCLE -----------------*/
ILBuilder *il_create(Fn *fn) {
        ILBuilder *ilb = alloc(sizeof(ILBuilder));
        ilb->fn        = fn;
        ilb->cur       = fn->start;
        return ilb;
}
void il_destroy(ILBuilder *ilb) {
        free(ilb);
}
void il_set_insert_point(ILBuilder *ilb, Blk *blk) {
        ilb->cur = blk;
}
Blk *il_get_insert_block(ILBuilder *ilb) {
        return ilb->cur;
}
Blk *il_create_block(ILBuilder *ilb, const char *name) {
        Blk *b  = newblk();
        b->name = strf(PFn, "%s", name);
        b->id   = ilb->fn->nblk++;

        if (!ilb->fn->start) {
                ilb->fn->start = b;
                if (!ilb->cur) {
                        ilb->cur = b;
                }
        } else {
                Blk *t = ilb->fn->start;
                while (t->link) {
                        t = t->link;
                }
                t->link = b;
        }

        return b;
}
Fn *il_create_function(const char *name, int retty, Lnk *lnk) {
        Fn *fn   = alloc(sizeof(Fn));
        *fn      = (Fn){0};
        fn->tmp  = vnew(0, sizeof(Tmp), PFn);
        fn->con  = vnew(2, sizeof(Con), PFn);
        fn->ncon = 2;
        for (int i = 0; i < Tmp0; i++) {
                newtmp(0, T.fpr0 <= i && i < T.fpr0 + T.nfpr ? Kd : Kl, fn);
        }

        fn->con[0] = (Con){.type = CBits};
        fn->con[1] = (Con){.type = CBits};
        fn->name   = strf(PFn, "%s", name);
        fn->retty  = retty;
        fn->lnk    = lnk ? *lnk : (Lnk){0};
        fn->start  = NULL;
        fn->nblk   = 0; // blocks added via il_create_block

        fn->mem = vnew(0, sizeof(Mem), PFn);
        fn->rpo = vnew(0, sizeof(Blk *), PFn);

        return fn;
}
Fn *il_finish(ILBuilder *ilb) {
        Fn *fn = ilb->fn;
        // count blocks from fn->start->link chain, built via il_create_block
        int count = 0;
        for (Blk *b = fn->start; b; b = b->link) {
                count++;
        }
        fn->nblk = count;
        fn->rpo  = vnew(count, sizeof(Blk *), PFn);

        // copies link list of block into array
        int i = 0;
        for (Blk *b = fn->start; b; b = b->link) {
                fn->rpo[i++] = b;
        }
        if (!fn->mem) {
                fn->mem = vnew(0, sizeof(Mem), PFn);
        }

        return fn;
}

/*------------------ CONSTANT -----------------*/
Ref il_const_int_w(ILBuilder *ilb, int32_t v) {
        return getcon((int64_t)v, ilb->fn);
}
Ref il_const_int_l(ILBuilder *ilb, int64_t v) {
        return getcon(v, ilb->fn);
}
Ref il_const_float_s(ILBuilder *ilb, float v) {
        Con c = {.type = CBits, .flt = 1, .bits.s = v};
        return newcon(&c, ilb->fn);
}
Ref il_const_float_d(ILBuilder *ilb, double v) {
        Con c = {.type = CBits, .flt = 2, .bits.d = v}; // flt 2=d
        return newcon(&c, ilb->fn);
}
Ref il_const_undef(ILBuilder *ilb) {
        (void)ilb;
        return UNDEF;
}
Ref il_const_zero(ILBuilder *ilb) {
        return getcon(0, ilb->fn);
}

/*------------------ ARITHMETIC HELPERS -----------------*/
// for binrary operations
static Ref mk2(ILBuilder *ilb, int op, int cls, Ref a, Ref b) {
        Ref r   = newtmp(0, cls, ilb->fn);
        Ins ins = {.op = op, .cls = cls, .to = r, .arg = {a, b}};
        addins(&ilb->cur->ins, &ilb->cur->nins, &ins);
        return r;
}
// for unary operations
static Ref mk1(ILBuilder *ilb, int op, int cls, Ref a) {
        Ref r   = newtmp(0, cls, ilb->fn);
        Ins ins = {.op = op, .cls = cls, .to = r, .arg = {a, R}};
        addins(&ilb->cur->ins, &ilb->cur->nins, &ins);
        return r;
}

/*----------------- ARITHMETIC -----------------*/
/* add */
Ref il_create_add_w(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Oadd, Kw, a, b); }
Ref il_create_add_l(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Oadd, Kl, a, b); }
Ref il_create_add_s(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Oadd, Ks, a, b); }
Ref il_create_add_d(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Oadd, Kd, a, b); }
/* sub */
Ref il_create_sub_w(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Osub, Kw, a, b); }
Ref il_create_sub_l(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Osub, Kl, a, b); }
Ref il_create_sub_s(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Osub, Ks, a, b); }
Ref il_create_sub_d(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Osub, Kd, a, b); }
/* mul */
Ref il_create_mul_w(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Omul, Kw, a, b); }
Ref il_create_mul_l(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Omul, Kl, a, b); }
Ref il_create_mul_s(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Omul, Ks, a, b); }
Ref il_create_mul_d(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Omul, Kd, a, b); }
/* div (signed) */
Ref il_create_div_w(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Odiv, Kw, a, b); }
Ref il_create_div_l(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Odiv, Kl, a, b); }
Ref il_create_div_s(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Odiv, Ks, a, b); }
Ref il_create_div_d(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Odiv, Kd, a, b); }
/* udiv (unsigned) */
Ref il_create_udiv_w(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Oudiv, Kw, a, b); }
Ref il_create_udiv_l(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Oudiv, Kl, a, b); }
/* remainder (signed) */
Ref il_create_rem_w(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Orem, Kw, a, b); }
Ref il_create_rem_l(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Orem, Kl, a, b); }
/* remainder (unsigned) */
Ref il_create_urem_w(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Ourem, Kw, a, b); }
Ref il_create_urem_l(ILBuilder *ilb, Ref a, Ref b) { return mk2(ilb, Ourem, Kl, a, b); }
/* unary negative */
Ref il_create_neg_w(ILBuilder *ilb, Ref a) { return mk1(ilb, Oneg, Kw, a); }
Ref il_create_neg_l(ILBuilder *ilb, Ref a) { return mk1(ilb, Oneg, Kl, a); }
Ref il_create_neg_s(ILBuilder *ilb, Ref a) { return mk1(ilb, Oneg, Ks, a); }
Ref il_create_neg_d(ILBuilder *ilb, Ref a) { return mk1(ilb, Oneg, Kd, a); }

/*----------------- BITWISE -----------------*/
/* and & */
Ref il_create_and_w(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Oand, Kw, a, b); }
Ref il_create_and_l(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Oand, Kl, a, b); }
/* or | */
Ref il_create_or_w(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Oor, Kw, a, b); }
Ref il_create_or_l(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Oor, Kl, a, b); }
/* xor ^ */
Ref il_create_xor_w(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Oxor, Kw, a, b); }
Ref il_create_xor_l(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Oxor, Kl, a, b); }
/* shift left << */
Ref il_create_shl_w(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Oshl, Kw, a, b); } // b is w shift amount
Ref il_create_shl_l(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Oshl, Kl, a, b); }
/* shift right >> */
Ref il_create_shr_w(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Oshr, Kw, a, b); }
Ref il_create_shr_l(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Oshr, Kl, a, b); }
/* shift right arithmatic */
Ref il_create_sar_w(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Osar, Kw, a, b); }
Ref il_create_sar_l(ILBuilder *bd, Ref a, Ref b) { return mk2(bd, Osar, Kl, a, b); }

/*----------------- MEMORY HELPERS -----------------*/
Ref allocator(ILBuilder *ilb, int op, int cls, Ref n) {
        Ref r = newtmp(0, cls, ilb->fn);
        Ins i = {.op = op, .cls = cls, .to = r, .arg = {n, R}}; // size = n, second arg R = none
        addins(&ilb->cur->ins, &ilb->cur->nins, &i);
        return r;
}
// load reads memory
Ref load(ILBuilder *ilb, int op, int cls, Ref addr) {
        Ref r = newtmp(0, cls, ilb->fn);
        Ins i = {.op = op, .cls = cls, .to = r, .arg = {addr, R}};
        addins(&ilb->cur->ins, &ilb->cur->nins, &i);
        return r;
}
// store writes memory
void store(ILBuilder *ilb, int op, int cls, Ref val, Ref addr) {
        Ins i = {.op = op, .cls = cls, .to = R, .arg = {val, addr}};
        addins(&ilb->cur->ins, &ilb->cur->nins, &i);
}

/*----------------- MEMORY -----------------*/
/* alloc */
Ref il_create_alloc4(ILBuilder *ilb, Ref n) { return allocator(ilb, Oalloc4, Kl, n); }
Ref il_create_alloc8(ILBuilder *ilb, Ref n) { return allocator(ilb, Oalloc8, Kl, n); }
Ref il_create_alloc16(ILBuilder *ilb, Ref n) { return allocator(ilb, Oalloc16, Kl, n); }
/* load */
Ref il_create_load_w(ILBuilder *ilb, Ref addr) { return load(ilb, Oloadsw, Kw, addr); }
Ref il_create_load_l(ILBuilder *ilb, Ref addr) { return load(ilb, Oload, Kl, addr); }
Ref il_create_load_s(ILBuilder *ilb, Ref addr) { return load(ilb, Oload, Ks, addr); }
Ref il_create_load_d(ILBuilder *ilb, Ref addr) { return load(ilb, Oload, Kd, addr); }
Ref il_create_load_sb(ILBuilder *ilb, Ref addr) { return load(ilb, Oloadsb, Kw, addr); }
Ref il_create_load_ub(ILBuilder *ilb, Ref addr) { return load(ilb, Oloadub, Kw, addr); }
Ref il_create_load_sh(ILBuilder *ilb, Ref addr) { return load(ilb, Oloadsh, Kw, addr); }
Ref il_create_load_uh(ILBuilder *ilb, Ref addr) { return load(ilb, Oloaduh, Kw, addr); }
Ref il_create_load_sw(ILBuilder *ilb, Ref addr) { return load(ilb, Oloadsw, Kl, addr); }
Ref il_create_load_uw(ILBuilder *ilb, Ref addr) { return load(ilb, Oloaduw, Kl, addr); }
/* store */
void il_create_store_w(ILBuilder *ilb, Ref val, Ref addr) { store(ilb, Ostorew, Kw, val, addr); }
void il_create_store_l(ILBuilder *ilb, Ref val, Ref addr) { store(ilb, Ostorel, Kl, val, addr); }
void il_create_store_s(ILBuilder *ilb, Ref val, Ref addr) { store(ilb, Ostores, Ks, val, addr); }
void il_create_store_d(ILBuilder *ilb, Ref val, Ref addr) { store(ilb, Ostored, Kd, val, addr); }
void il_create_store_b(ILBuilder *ilb, Ref val, Ref addr) { store(ilb, Ostoreb, Kw, val, addr); }
void il_create_store_h(ILBuilder *ilb, Ref val, Ref addr) { store(ilb, Ostoreh, Kw, val, addr); }
/* copies n type from src to dst */
void il_create_blit(ILBuilder *ilb, Ref dst, Ref src, int64_t n) {
        Ins i0 = {.op = Oblit0, .cls = Kw, .to = R, .arg = {src, dst}};
        addins(&ilb->cur->ins, &ilb->cur->nins, &i0);
        Ins i1 = {.op = Oblit1, .cls = Kw, .to = R, .arg = {getcon(n, ilb->fn), R}};
        addins(&ilb->cur->ins, &ilb->cur->nins, &i1);
}

/*----------------- CONTROL -----------------*/
void il_create_br(ILBuilder *ilb, Blk *dst) {}
void il_create_cond_br(ILBuilder *ilb, Ref cond, Blk *then_blk, Blk *else_blk) {}
void il_create_ret_w(ILBuilder *ilb, Ref v) {}
void il_create_ret_l(ILBuilder *ilb, Ref v) {}
void il_create_ret_s(ILBuilder *ilb, Ref v) {}
void il_create_ret_d(ILBuilder *ilb, Ref v) {}
void il_create_ret_void(ILBuilder *ilb) {}
void il_create_unreachable(ILBuilder *ilb) {}
