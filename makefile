# --------------------------------------------------------------------
#
# !\file makefile
# !\brief operon project configuration for makeshift.
# !\author Colin J.D. Stewart
#
# --------------------------------------------------------------------

PROJECT ?= operon

BIN_NAME ?= $(PROJECT)
LIB_BASENAME ?= operon

BUILD_EXE ?= 1
BUILD_STATIC ?= 0
BUILD_SHARED ?= 1

BUILD_M32 ?= 0
BUILD_M64 ?= 1

CXX_STD ?= c++17
MODE ?= debug

USER_CPPFLAGS :=
USER_CXXFLAGS := 
USER_LDFLAGS :=
USER_LDLIBS :=

include dep/makeshift/makeshift.mk