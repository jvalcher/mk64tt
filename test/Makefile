#
# test/Makefile
#

MAKEFLAGS += --no-print-directory

src_dir = ../src
bin_dir = ../bin
obj_dir = ../obj/test

cc = gcc
cflags = -g -Wall -Wextra
bin = $(bin_dir)/test_mk64tt
build = $(cc) $(cflags) -o $(bin)

all: \
	test_utils

test_utils: test_utils.c
	@make -C $(src_dir) test_utils
	@ $(build) $^  $(obj_dir)/utils.o
	@./$(bin)
