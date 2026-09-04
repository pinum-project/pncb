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
