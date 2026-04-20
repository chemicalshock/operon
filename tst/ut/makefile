#
# makefile for shocktest unit tests
#
UT_MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
repo_root_has_project_shape = $(if $(wildcard $(1)/src/inc),$(1))
REPO_ROOT_CANDIDATES := $(abspath $(UT_MAKEFILE_DIR)/../..) $(abspath $(UT_MAKEFILE_DIR)/../../..)
DETECTED_REPO_ROOT := $(firstword $(foreach dir,$(REPO_ROOT_CANDIDATES),$(if $(call repo_root_has_project_shape,$(dir)),$(dir))))
REPO_ROOT := $(if $(DETECTED_REPO_ROOT),$(DETECTED_REPO_ROOT),$(firstword $(REPO_ROOT_CANDIDATES)))

X86CPPTARGETSOL ?=

OPERON_LIBS := value_SUT.cpp.so context_SUT.cpp.so

# ------------------------------
# SUT targets
# ------------------------------

X86CPPTARGETSOL += value_SUT.cpp.so
value_SUT.cpp.so,SRCS := value.cpp
value_SUT.cpp.so,DEPS :=
value_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

X86CPPTARGETSOL += context_SUT.cpp.so
context_SUT.cpp.so,SRCS := context.cpp
context_SUT.cpp.so,DEPS :=
context_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

X86CPPTARGETSOL += operon_SUT.cpp.so
operon_SUT.cpp.so,SRCS := operon.cpp
operon_SUT.cpp.so,DEPS := \
		value_SUT.cpp.so \
		context_SUT.cpp.so
operon_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

# ------------------------------
# Algorithm targets
# ------------------------------

X86CPPTARGETSOL += blake2b_SUT.cpp.so
blake2b_SUT.cpp.so,SRCS := \
		algos/hash/blake2b/blake2b.cpp \
		algos/hash/blake2b/core.cpp
blake2b_SUT.cpp.so,DEPS := $(OPERON_LIBS)
blake2b_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

X86CPPTARGETSOL += md5_SUT.cpp.so
md5_SUT.cpp.so,SRCS := \
		algos/hash/md5/md5.cpp \
		algos/hash/md5/core.cpp
md5_SUT.cpp.so,DEPS := $(OPERON_LIBS)
md5_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

X86CPPTARGETSOL += constant_time_eq_SUT.cpp.so
constant_time_eq_SUT.cpp.so,SRCS := \
		algos/util/constant_time_eq/constant_time_eq.cpp
constant_time_eq_SUT.cpp.so,DEPS := $(OPERON_LIBS)
constant_time_eq_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

# ------------------------------
# Test targets
# ------------------------------

X86CPPTARGET += TEST_HASH_BLAKE2B
TEST_HASH_BLAKE2B,SRCS := test_hash_blake2b.cpp
TEST_HASH_BLAKE2B,USRLIBS := blake2b_SUT.cpp.so
TEST_HASH_BLAKE2B,CPPFLAGS = $(GLOBAL_CPPFLAGS)
TEST_HASH_BLAKE2B,LDFLAGS =

X86CPPTARGET += TEST_HASH_MD5
TEST_HASH_MD5,SRCS := test_hash_md5.cpp
TEST_HASH_MD5,USRLIBS := md5_SUT.cpp.so
TEST_HASH_MD5,CPPFLAGS = $(GLOBAL_CPPFLAGS)
TEST_HASH_MD5,LDFLAGS =

X86CPPTARGET += TEST_UTIL_CONSTANT_TIME_EQ
TEST_UTIL_CONSTANT_TIME_EQ,SRCS := test_util_constant_time_eq.cpp
TEST_UTIL_CONSTANT_TIME_EQ,USRLIBS := constant_time_eq_SUT.cpp.so
TEST_UTIL_CONSTANT_TIME_EQ,CPPFLAGS = $(GLOBAL_CPPFLAGS)
TEST_UTIL_CONSTANT_TIME_EQ,LDFLAGS =

# Include shocktest makefile from dep/shocktest symlink (no absolute path).
include $(REPO_ROOT)/dep/shocktest/src/tst/ut/makefile.shocktest.mk
