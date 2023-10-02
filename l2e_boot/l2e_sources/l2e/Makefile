# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for L2E Init
#

# choose your compiler, e.g. gcc/clang
# example override to clang: make run CC=clang

CC = gcc

MOD_PATH    = model.bin
TOK_PATH    = tokenizer.bin
MUSL_SYSROOT = ../../musl_build
MUSL_SYSINC = ../../musl_build/include
MUSL_LIB = ../../musl_build/lib
INITLINKER= lld
INITLINKERGCC = lld
INITLINKERCLANG = lld


# Preprocessed $(CC) static + embedded model & tokenizer, init ; for debugging only

.PHONY: l2e_bin_pre_cc
l2e_bin_pre_cc:
	$(CC) -E -Ofast -static -D LINUXK -D OSPROMPT="ORACLE$$" -D MODPATH=$(MOD_PATH) -D TOKPATH=$(TOK_PATH) run.c -lm -o l2e_bin
	$(CC) -E -Ofast -static -nostdinc --sysroot $(MUSL_SYSROOT) -isystem $(MUSL_SYSINC) -L$(MUSL_LIB) -fuse-ld=$(INITLINKER) init.c -o init

# $(CC) static + embedded model & tokenizer, init

.PHONY: l2e_bin_cc
l2e_bin_cc:
	$(CC) -Ofast -static -D LINUXK -D OSPROMPT="ORACLE$$" -D MODPATH=$(MOD_PATH) -D TOKPATH=$(TOK_PATH) run.c -lm -o l2e_bin
	$(CC) -Ofast -static -nostdinc --sysroot $(MUSL_SYSROOT) -isystem $(MUSL_SYSINC) -L$(MUSL_LIB) -fuse-ld=$(INITLINKER) init.c -o init
	strip -s l2e_bin
	strip -s init
	
# GCC static + embedded model & tokenizer, init

.PHONY: l2e_bin_gcc
l2e_bin_gcc:
	gcc -Ofast -static -D LINUXK -D OSPROMPT="ORACLE$$" -D MODPATH=$(MOD_PATH) -D TOKPATH=$(TOK_PATH) run.c -lm -o l2e_bin
	gcc -Ofast -static -nostdinc --sysroot $(MUSL_SYSROOT) -isystem $(MUSL_SYSINC) -L$(MUSL_LIB) -fuse-ld=$(INITLINKERGCC) init.c -o init
	strip -s l2e_bin
	strip -s init
		
# Clang static + embedded model & tokenizer, init

.PHONY: l2e_bin_clang
l2e_bin_clang:
	clang -Ofast -static -flto=thin -D LINUXK -D OSPROMPT="ORACLE$$" -D MODPATH=$(MOD_PATH) -D TOKPATH=$(TOK_PATH) run.c -lm -o l2e_bin
	clang -Ofast -static -flto=thin -nostdinc --sysroot $(MUSL_SYSROOT) -isystem $(MUSL_SYSINC) -L$(MUSL_LIB) -fuse-ld=$(INITLINKERCLANG) init.c -o init
	llvm-strip -s l2e_bin
	llvm-strip -s init

# Clean
.PHONY: clean
clean:
	rm -f l2e_bin init toybox busybox l2eterm fbdoom *.wad ../drivers/misc/l2e_os/*.o ../drivers/misc/l2e_os/*.a ../drivers/misc/l2e_os/l2e_bin
	find . -name '*~'
	find . -name '*~' -delete

.PHONY: list
list:
	@LC_ALL=C $(MAKE) -pRrq -f $(firstword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/(^|\n)# Files(\n|$$)/,/(^|\n) / {if ($$1 !~ "^[#.]") {print $$1}}' | sort | grep -E -v -e '^[^[:alnum:]]' -e '^$@$$'
	
