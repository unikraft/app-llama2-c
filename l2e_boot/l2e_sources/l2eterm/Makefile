#
# Makefile
#
CC ?= gcc
LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= ${shell pwd}
CFLAGS ?= -O3 -D_DEFAULT_SOURCE -flto=auto -ffunction-sections -fdata-sections -I$(LVGL_DIR)/ -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wno-discarded-qualifiers -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare -std=c99
LDFLAGS ?= -static -lm
BIN = l2eterm

prefix ?= /usr
bindir ?= $(prefix)/bin

#Collect the files to compile
MAINSRC = ./l2eterm.c

#LVGL 
#include $(LVGL_DIR)/lvgl/lvgl.mk
#include $(LVGL_DIR)/$(LVGL_DIR_NAME)/demos/lv_demos.mk
#include $(LVGL_DIR)/$(LVGL_DIR_NAME)/examples/lv_examples.mk
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/core/lv_core.mk
#include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/lv_draw.mk
###
CSRCS += lv_draw_arc.c
CSRCS += lv_draw.c
CSRCS += lv_draw_img.c
CSRCS += lv_draw_label.c
CSRCS += lv_draw_line.c
CSRCS += lv_draw_mask.c
CSRCS += lv_draw_rect.c
CSRCS += lv_draw_transform.c
CSRCS += lv_draw_layer.c
CSRCS += lv_draw_triangle.c
CSRCS += lv_img_buf.c
CSRCS += lv_img_cache.c
CSRCS += lv_img_decoder.c
##
DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw
CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw"
##
CSRCS += lv_draw_sw.c
CSRCS += lv_draw_sw_arc.c
CSRCS += lv_draw_sw_blend.c
CSRCS += lv_draw_sw_dither.c
CSRCS += lv_draw_sw_gradient.c
CSRCS += lv_draw_sw_img.c
CSRCS += lv_draw_sw_letter.c
CSRCS += lv_draw_sw_line.c
CSRCS += lv_draw_sw_polygon.c
CSRCS += lv_draw_sw_rect.c
CSRCS += lv_draw_sw_transform.c
CSRCS += lv_draw_sw_layer.c
#
DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw
CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/draw/sw"
### 
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/extra/lv_extra.mk
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/font/lv_font.mk
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/hal/lv_hal.mk
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/misc/lv_misc.mk
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/widgets/lv_widgets.mk

#LVGL Drivers
#include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
###
LV_DRIVERS_DIR_NAME ?= lv_drivers
override CFLAGS := -I$(LVGL_DIR) $(CFLAGS)
CSRCS += $(wildcard $(LVGL_DIR)/$(LV_DRIVERS_DIR_NAME)/*.c)
CSRCS += $(wildcard $(LVGL_DIR)/$(LV_DRIVERS_DIR_NAME)/indev/*.c)
CSRCS += $(wildcard $(LVGL_DIR)/$(LV_DRIVERS_DIR_NAME)/display/*.c)
###

# Mouse Cursor
CSRCS +=$(LVGL_DIR)/mouse_cursor_icon.c 

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES

all: default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"
    
default: $(AOBJS) $(COBJS) $(MAINOBJ) 
	$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS)
	strip -s l2eterm

clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ) 
	find . -name '*~'
	find . -name '*~' -delete
	

