#
# test/Makefile
#
ext := c
cc := gcc
cflags := -g3 -Wall -Wextra

include ../Makesuite.mk

#
# Tests
#
mup_libs := $(shell sdl2-config --libs)
mup_obj_deps := mup utils
mup_testobj_deps := misc_test_utils
$(eval $(call RUN_TEST,mup,$(build)))

rec_dex_obj_deps := rec_dex utils
$(eval $(call RUN_TEST,rec_dex,$(build)))

save_dirs_obj_deps := save_dirs utils categ
$(eval $(call RUN_TEST,save_dirs,$(build)))

#
# Test modules
#
$(eval $(call BUILD_DIR,misc_test_utils,test))

