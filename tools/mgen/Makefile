# Original QBE Codebase
# Copyright (c) 2015-2026 Quentin Carbonneaux <quentin@c9x.me>
#
# Modifications for Quil Compiler Backend (feather)
# Copyright (c) 2026-present Quil Project Authors
#
# Released under the MIT License.

BIN = mgen
SRC = \
	match.ml \
	fuzz.ml \
	cgen.ml \
	sexp.ml \
	test.ml \
	main.ml

$(BIN): $(SRC)
	ocamlopt -o $(BIN) -g str.cmxa $(SRC)

clean:
	rm -f *.cm? *.o $(BIN)

.PHONY: clean
