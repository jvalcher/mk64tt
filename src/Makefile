#
# src/Makefile
#
ext := c
cc := gcc
cflags := -Wall -Wextra
cflags_dev := -g3
cflags_prod := -O2

include ../Makesuite.mk

$(eval $(call BUILD_SRC,categ,$(build)))

$(eval $(call BUILD_DIR,rec_dex,$(build)))

mupen64plus_cflags := -Wall -Wextra $(shell sdl2-config --cflags)
$(eval $(call BUILD_SRC,mup,$(build)))

$(eval $(call BUILD_SRC,save_dirs,$(build)))

$(eval $(call BUILD_DIR,utils,$(build)))

