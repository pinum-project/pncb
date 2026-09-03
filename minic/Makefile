# Original QBE Codebase
# Copyright (c) 2015-2026 Quentin Carbonneaux <quentin@c9x.me>
#
# Modifications for Quil Compiler Backend (feather)
# Copyright (c) 2026-present Quil Project Authors
#
# Released under the MIT License.

BIN = minic

CFLAGS += -g -Wall

$(BIN): yacc minic.y
	./yacc minic.y
	$(CC) $(CFLAGS) -o $@ y.tab.c

clean:
	rm -f yacc minic y.*

.PHONY: clean
