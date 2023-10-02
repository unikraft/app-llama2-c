# SPDX-License-Identifier: GPL-2.0-only
#
# Makefile for L2E OS Driver
#

$(obj)/l2e_blob.o: $(obj)/l2e_bin
$(obj)/l2e_bin: $(srctree)/l2e/run.c
	# We could also pass any static l2e build here without building it
	$(CC) -Ofast -static -D LINUXK -D OSPROMPT="KERNEL ORACLE$$" -D MODPATH=$(srctree)/l2e/model.bin -D TOKPATH=$(srctree)/l2e/tokenizer.bin $(srctree)/l2e/run.c  -lm  -o $(obj)/l2e_bin
	
obj-$(CONFIG_L2E_OS_DRIVER) += l2e.o
l2e-objs += l2e_kern.o l2e_blob.o
