#
# src/Makefile
#

MAKEFLAGS += --no-print-directory

bin_dir = ../bin
obj_dir = ../obj
test_obj_dir = $(obj_dir)/test

cc = gcc
cflags = -Wall -Wextra
build = $(cc) $(cflags) -c
test_build = $(build) -g


#
# Build object files
#
test_utils: utils.c
	@$(test_build) $^ -o $(test_obj_dir)/utils.o
