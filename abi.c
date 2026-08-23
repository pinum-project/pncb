/*
 * Original QBE Codebase
 * Copyright (c) 2015-2026 Quentin Carbonneaux <quentin@c9x.me>
 *
 * Modifications for PiNum Compiler Backend (pncb)
 * Copyright (c) 2026-present PiNum Project Authors
 *
 * Released under the MIT License.
 */

#include "all.h"

/* eliminate sub-word abi op
 * variants for targets that
 * treat char/short/... as
 * words with arbitrary high
 * bits
 */
void elimsb(Fn *fn) {
        Blk *b;
        Ins *i;

        for (b = fn->start; b; b = b->link) {
                for (i = b->ins; i < &b->ins[b->nins]; i++) {
                        if (isargbh(i->op)) {
                                i->op = Oarg;
                        }
                        if (isparbh(i->op)) {
                                i->op = Opar;
                        }
                }
                if (isretbh(b->jmp.type)) {
                        b->jmp.type = Jretw;
                }
        }
}
