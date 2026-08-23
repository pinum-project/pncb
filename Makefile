#
# Original QBE Codebase
# Copyright (c) 2015-2026 Quentin Carbonneaux <quentin@c9x.me>
#
# Modifications for PiNum Compiler Backend (pncb)
# Copyright (c) 2026-present PiNum Project Authors
#
# Released under the MIT License.
#

.POSIX:
.SUFFIXES: .o .c

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

BUILDDIR = bin

COMMOBJ  = main.o util.o parse.o abi.o cfg.o mem.o ssa.o alias.o load.o \
           copy.o fold.o gvn.o gcm.o simpl.o ifopt.o live.o spill.o rega.o \
           emit.o
AMD64OBJ = amd64/targ.o amd64/sysv.o amd64/isel.o amd64/emit.o amd64/winabi.o
ARM64OBJ = arm64/targ.o arm64/abi.o arm64/isel.o arm64/emit.o
RV64OBJ  = rv64/targ.o rv64/abi.o rv64/isel.o rv64/emit.o
OBJ      = $(addprefix $(BUILDDIR)/,$(COMMOBJ) $(AMD64OBJ) $(ARM64OBJ) $(RV64OBJ))

REL_CFLAGS = -std=c99 -O2 -Wall -Wextra -Wpedantic

SRCALL   = $(COMMOBJ:.o=.c) $(AMD64OBJ:.o=.c) $(ARM64OBJ:.o=.c) $(RV64OBJ:.o=.c)

CC       = cc
CFLAGS   = -std=c99 -O2 -g -Wall -Wextra -Wpedantic

qbe: bin/qbe

bin/qbe: $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(addprefix $(BUILDDIR)/,$(COMMOBJ)): all.h ops.h
$(addprefix $(BUILDDIR)/,$(AMD64OBJ)): amd64/all.h
$(addprefix $(BUILDDIR)/,$(ARM64OBJ)): arm64/all.h
$(addprefix $(BUILDDIR)/,$(RV64OBJ)): rv64/all.h
$(BUILDDIR)/main.o: config.h

config.h:
	@case `uname` in                               \
	*Darwin*)                                      \
		case `uname -m` in                     \
		*arm64*)                               \
			echo "#define Deftgt T_arm64_apple";\
			;;                             \
		*)                                     \
			echo "#define Deftgt T_amd64_apple";\
			;;                             \
		esac                                   \
		;;                                     \
	*)                                             \
		case `uname -m` in                     \
		*aarch64*|*arm64*)                     \
			echo "#define Deftgt T_arm64"; \
			;;                             \
		*riscv64*)                             \
			echo "#define Deftgt T_rv64";  \
			;;                             \
		*)                                     \
			echo "#define Deftgt T_amd64_sysv";\
			;;                             \
		esac                                   \
		;;                                     \
	esac > $@

install: qbe
	mkdir -p "$(DESTDIR)$(BINDIR)"
	install -m755 bin/qbe "$(DESTDIR)$(BINDIR)/qbe"

uninstall:
	rm -f "$(DESTDIR)$(BINDIR)/qbe"

clean:
	rm -f $(BUILDDIR)/*.o $(BUILDDIR)/*/*.o $(BUILDDIR)/qbe

release:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(REL_CFLAGS)" bin/qbe

clean-gen: clean
	rm -f config.h

check: qbe
	bin=bin/qbe tools/test.sh all

check-x86_64: qbe
	TARGET=x86_64 bin=bin/qbe tools/test.sh all

check-arm64: qbe
	TARGET=arm64 bin=bin/qbe tools/test.sh all

check-rv64: qbe
	TARGET=rv64 bin=bin/qbe tools/test.sh all

check-amd64_win: qbe
	TARGET=amd64_win bin=bin/qbe tools/test.sh all

src:
	@echo $(SRCALL)

80:
	@for F in $(SRCALL);                       \
	do                                         \
		awk "{                             \
			gsub(/\\t/, \"        \"); \
			if (length(\$$0) > $@)     \
				printf(\"$$F:%d: %s\\n\", NR, \$$0); \
		}" < $$F;                          \
	done

wc:
	@wc -l $(SRCALL)

.PHONY: clean clean-gen check check-arm64 check-rv64 check-amd64_win src 80 wc install uninstall release
