#
# Makefile
#

#
# Suppress Make, compiler output with v=0
#
v ?= 1
ifeq ($(v),0)
q := @
MAKEFLAGS += --no-print-directory --silent
else
q :=
endif

#
# data/ghost/dex/*.dex  ->  src/rec_dex/dex_*.c
#
create_dex_srcs:
	make -C src/ghost_data create_dex_sources
clean_dex_srcs:
	make -C src/ghost_data clean_dex_sources

#
# src/ objects
#
build_objs:
	make -C src build_all build=$(build)

build_objs_categ:
	make -C src build_categ build=$(build)
build_objs_mup:
	make -C src build_mup build=$(build)
build_objs_rec_dex:
	make -C src build_rec_dex build=$(build)
build_objs_save_dirs:
	make -C src build_save_dirs build=$(build)
build_objs_utils:
	make -C src build_utils build=$(build)

clean_objs:
	make -C src clean_all build=$(build)

clean_objs_categ:
	make -C src clean_categ build=$(build)
clean_objs_mup:
	make -C src clean_mup build=$(build)
clean_objs_rec_dex:
	make -C src clean_rec_dex build=$(build)
clean_objs_save_dirs:
	make -C src clean_save_dirs build=$(build)
clean_objs_utils:
	make -C src clean_utils build=$(build)

#
# test/ objects
#
build_testobjs:
	make -C src build_all build=test

build_testobjs_misc_test_utils:
	make -C test build_misc_test_utils build=test

clean_testobjs:
	make -C test clean_all build=test

clean_testobjs_misc_test_utils:
	make -C test clean_misc_test_utils build=test

#
# Tests
#
test_all:
	$(q)make -C test run_all build=$(build)

test_rex_dex:
	$(q)make -C test test_rec_dex build=$(build)
test_mup:
	$(q)make -C test test_mup build=$(build)
test_save_dirs:
	$(q)make -C test test_save_dirs build=$(build)

clean_test_all: clean_testobjs clean_test_bins

clean_test_bins:
	$(q)make -C test clean_test_all build=test

clean_test_mup:
	$(q)make -C test clean_test_mup build=test
clean_test_rec_dex:
	$(q)make -C test clean_test_rec_dex build=test
clean_test_save_dirs:
	$(q)make -C test clean_test_save_dirs build=test
