#
# ------------
# Makesuite.mk
# ------------
# Description: Makefile build, test function suite for C projects
# Repo:		   
# Author:      jvalcher
# License: 	   MIT
#



###########
# Private #
###########

define CHECK_BUILD
$(if $(filter dev prod test,$(1)),,\
  $(error Missing 'build=prod|dev|test'))
endef

define CHECK_GLOBAL_DEF
$(if $(filter undefined,$(origin $(1))),\
  	$(error Empty '$(1)' global variable not found, '$(1) :='))
endef
define CHECK_GLOBAL_SET
$(if \
	$(or \
		$(filter undefined,$(origin $(1))),\
		$(strip $($(1)))
	),,\
		$(error Global variable '$(1)' not set, e.g. '$(1) := $(2)'))
endef

define RESOLVE_VAR
$(1)_$(2) := \
  $(if $(filter undefined,$(origin $(1)_$(2))), \
      $($(2)), \
      $($(1)_$(2)))
endef
define RESOLVE_BUILD_CFLAGS
$(1)_cflags += \
  $(if $(filter undefined,$(origin $(1)_$(2))), \
      $($(2)), \
      $($(1)_$(2)))
endef

define GET_GLOBALS_EXT_CC_CFLAGS_LIBS
$(eval $(call CHECK_GLOBAL_SET,ext,c))
$(eval $(call CHECK_GLOBAL_SET,cc,gcc))
$(eval $(call RESOLVE_VAR,$(1),ext))
$(eval $(call RESOLVE_VAR,$(1),cc))
$(eval $(call RESOLVE_VAR,$(1),cflags))
$(eval $(call RESOLVE_VAR,$(1),libs))
endef

define GET_GLOBALS_BUILD_CFLAGS
$(if $(filter prod,$(2)),\
	$(eval $(call RESOLVE_BUILD_CFLAGS,$(1),cflags_prod)))
$(if $(filter dev,$(2)),\
	$(eval $(call RESOLVE_BUILD_CFLAGS,$(1),cflags_dev)))
endef

define INC_DEPS
ifndef __DEPS_INCLUDED
__DEPS_INCLUDED := 1
-include $(wildcard ../obj/$(1)/*.d)
-include $(wildcard ../obj/$(1)/*/*.d)
endif
endef

define CREATE_BUILD_CLEAN_ALL_TARGS
ifndef __BUILD_CLEAN_TARGS_CREATED
__BUILD_CLEAN_TARGS_CREATED := 1

build_targs :=
build_clean_targs :=

.PHONY: build_all clean_all
.SECONDEXPANSION:
build_all: $$$$(build_targs)
clean_all: $$$$(build_clean_targs)

endif
endef

define CREATE_TEST_CLEAN_TARGS
ifndef __TEST_CLEAN_TARGS_CREATED
__TEST_CLEAN_TARGS_CREATED := 1

test_targs :=
clean_test_targs :=

.PHONY: run_all clean_test_all
.SECONDEXPANSION:
run_all: $$$$(test_targs)
clean_test_all: $$$$(clean_test_targs)

endif
endef

define SET_VERBOSITY
ifndef __VERBOSITY_SET
__VERBOSITY_SET := 1
v ?= 1
ifeq ($(v),0)
q := @
MAKEFLAGS += --no-print-directory
else
q :=
endif
endif
endef



##########
# Public #
##########


# ------------------------------------
# BUILD_SRC - build source file module
# ------------------------------------
#
# Parameters:
#
# 	$(1) = module name  (src/$(1).$(ext))
#  	$(2) = module build type  (dev, prod)
#
# Globals:
#
#	# Default
#   ext := c
# 	cc := gcc
#	cflags := -Wall -Wextra
#	cflags_prod := -O2
#	cflags_dev := -g
#
#	# Per-module
#	$(1)_ext := cpp
#	$(1)_cc := clang
#	$(1)_cflags := -Wall -Wextra -Wpedantic
#	$(1)_cflags_prod := -O1
#	$(1)_cflags_dev := -Og -g3
#
define BUILD_SRC

$(eval $(call SET_VERBOSITY))
$(eval $(call CHECK_BUILD,$(2)))
$(eval $(call GET_GLOBALS_EXT_CC_CFLAGS_LIBS,$(1),$(2)))
$(eval $(call GET_GLOBALS_BUILD_CFLAGS,$(1),$(2)))
$(eval $(call CREATE_BUILD_CLEAN_ALL_TARGS))
$(eval $(call INC_DEPS,$(2)))

build_targs += build_$(1)
build_clean_targs += clean_$(1)

$(1)_inc_dir := ../inc/src
$(1)_obj_root := ../obj
$(1)_obj_dir := $$($(1)_obj_root)/$(2)

$(1)_src := $(1).$(ext)
$(1)_obj := $$($(1)_obj_dir)/$(1).o
$(1)_d := $$($(1)_obj:.o=.d)

.PHONY: build_$(1)
build_$(1): $$($(1)_obj)

$$($(1)_obj): $$($(1)_src) | $$($(1)_obj_dir)
	$(q)$$($(1)_cc) $$($(1)_cflags) -MMD -MP -I$$($(1)_inc_dir) -c $$< -o $$@

ifndef __OBJ_DIR_CREATED
__OBJ_DIR_CREATED := 1
$$($(1)_obj_dir):
	$(q)mkdir -p $$@
endif

.PHONY: clean_$(1)
clean_$(1):
	$(q)rm -f $$($(1)_obj) 
	$(q)rm -f $$($(1)_d)

endef



# ----------------------------------
# BUILD_DIR - build directory module
# ----------------------------------
#
# Parameters:
#
# 	$(1) = module name  (src/$(1)/*.$(ext))
#  	$(2) = module build type  (dev, prod)
#
# Globals:
#
#	# Default
#   ext := c
# 	cc := gcc
#	cflags := -Wall -Wextra
#	cflags_prod := -O3
#	cflags_dev := -g
#
#	# Custom
#	$(1)_ext := cpp
#	$(1)_cc := clang
#	$(1)_cflags := -Wall -Wextra -Wpedantic
#	$(1)_cflags_prod := -O1
#	$(1)_cflags_dev := -Og -g3
#
define BUILD_DIR

$(eval $(call SET_VERBOSITY))
$(eval $(call CHECK_BUILD,$(2)))
$(eval $(call GET_GLOBALS_EXT_CC_CFLAGS_LIBS,$(1),$(2)))
$(eval $(call GET_GLOBALS_BUILD_CFLAGS,$(1),$(2)))
$(eval $(call CREATE_BUILD_CLEAN_ALL_TARGS))
$(eval $(call INC_DEPS,$(2)))

build_targs += build_$(1)
build_clean_targs += clean_$(1)

$(1)_inc_dir := ../inc/src
$(1)_obj_root := ../obj
$(1)_obj_dir := $$($(1)_obj_root)/$(2)
$(1)_objs_dir := $$($(1)_obj_dir)/$(1)

$(1)_srcs := $$(wildcard $(1)/*.$(ext))
$(1)_obj := $$($(1)_obj_dir)/$(1).o
$(1)_objs := $$(patsubst $(1)/%.$(ext),$$($(1)_objs_dir)/%.o,$$($(1)_srcs))
$(1)_d := $$($(1)_obj:.o=.d)
$(1)_ds := $$($(1)_objs:.o=.d)

.PHONY: build_$(1)
build_$(1): $$($(1)_obj)

$$($(1)_obj): $$($(1)_objs)
	$(q)$$($(1)_cc) -r $$^ -o $$@

$$($(1)_objs_dir)/%.o: $(1)/%.$(ext) | $$($(1)_objs_dir)
	$(q)$$($(1)_cc) $$($(1)_cflags) -MMD -MP -I$$($(1)_inc_dir) -c $$< -o $$@

$$($(1)_objs_dir):
	$(q)mkdir -p $$@

.PHONY: clean_$(1)
clean_$(1):
	$(q)rm -f $$($(1)_objs)
	$(q)rm -f $$($(1)_obj)
	$(q)rm -f $$($(1)_ds)
	$(q)rm -f $$($(1)_d)
	$(q)rm -fd $$($(1)_objs_dir)

endef



# ------------------------------------
# RUN_TEST - build, run test
# ------------------------------------
#
# Parameters:
#
#	$(1) - module name  (src/$(1).$(ext) or src/$(1)/*.$(ext))
#  	$(2) = module build type  (dev, prod)
#
# Globals:
#
#	# Default
#   ext := c
# 	cc := gcc
#	cflags := -g -Wall -Wextra
#	libs := $(shell pkg-config --libs sdl2)
#	
#
#	# Per-test file
#	$(1)_ext := cpp
#	$(1)_cc := clang
#	$(1)_cflags := -g -Wall -Wextra $(shell sdl2-config --cflags)
#	$(1)_libs := $(shell pkg-config --libs sdl2)
#
#	# src/, test/ module dependencies
#	$(1)_obj_deps := file subproc
#	$(1)_test_deps := tools random
#
define RUN_TEST

$(eval $(call SET_VERBOSITY))
$(eval $(call CHECK_BUILD,$(2)))
$(eval $(call GET_GLOBALS_EXT_CC_CFLAGS_LIBS,$(1),$(2)))
$(eval $(call CREATE_TEST_CLEAN_TARGS))

test_targs += test_$(1)
clean_test_targs += clean_test_$(1)

$(1)_inc_dir := ../inc/src
$(1)_test_inc_dir := ../inc/test
$(1)_src_dir := ../src
$(1)_bin_dir := ../bin/test
$(1)_obj_root := ../obj
$(1)_obj_dir := ../obj/$(2)

$(1)_bin := $$($(1)_bin_dir)/$(1)

$(if $(filter undefined,$(origin $(1)_obj_deps)),, \
  $(1)_obj_dep_objs := \
	$(addprefix $$($(1)_obj_dir)/, \
	  $(addsuffix .o, $($(1)_obj_deps))))
$(if $(filter undefined,$(origin $(1)_testobj_deps)),, \
  $(1)_testobj_dep_objs := \
    $(addprefix $$($(1)_obj_root)/test/, \
      $(addsuffix .o, $($(1)_testobj_deps))))
#$$(info OBJ: $$($(1)_obj_dep_objs))
#$$(info TESTOBJ: $$($(1)_testobj_dep_objs))

.PHONY: test_$(1)
test_$(1): $(1)_build_obj_deps $(1)_build_testobj_deps | $$($(1)_bin_dir)
	$(q)$(cc) $$($(1)_cflags) -I$$($(1)_inc_dir) -I$$($(1)_test_inc_dir) \
		test_$(1).$(ext) \
		$$($(1)_obj_dep_objs) \
		$$($(1)_testobj_dep_objs) \
		-o $$($(1)_bin) \
		$$($(1)_libs)
	$(q)./$$($(1)_bin)

.PHONY: $(1)_build_obj_deps
$(1)_build_obj_deps:
	$(if $($(1)_obj_deps),\
		$(q)$(MAKE) -C $$($(1)_src_dir) $(addprefix build_,$($(1)_obj_deps)) build=$(2),)

.PHONY: $(1)_build_testobj_deps
$(1)_build_testobj_deps:
	$(if $($(1)_testobj_deps),\
		$(q)$(MAKE) $(addprefix build_,$($(1)_testobj_deps)) build=test,)

.PHONY: clean_test_$(1)
clean_test_$(1):
	$(q)rm -f $$($(1)_bin)

ifndef __TEST_BIN_CREATED
__TEST_BIN_CREATED := 1
$$($(1)_bin_dir):
	$(q)mkdir -p $$@
endif

endef

