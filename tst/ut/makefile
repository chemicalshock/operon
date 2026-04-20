#
# makefile for shocktest unit tests
#
UT_MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
repo_root_has_project_shape = $(if $(wildcard $(1)/src/inc),$(1))
REPO_ROOT_CANDIDATES := $(abspath $(UT_MAKEFILE_DIR)/../..) $(abspath $(UT_MAKEFILE_DIR)/../../..)
DETECTED_REPO_ROOT := $(firstword $(foreach dir,$(REPO_ROOT_CANDIDATES),$(if $(call repo_root_has_project_shape,$(dir)),$(dir))))
REPO_ROOT := $(if $(DETECTED_REPO_ROOT),$(DETECTED_REPO_ROOT),$(firstword $(REPO_ROOT_CANDIDATES)))

X86CPPTARGETSOL ?=

# ------------------------------
# SUT targets
# ------------------------------

# parsers

X86CPPTARGETSOL += acf_parser_SUT.cpp.so
acf_parser_SUT.cpp.so,SRCS := parser/acf_parser.cpp
acf_parser_SUT.cpp.so,DEPS :=
acf_parser_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

X86CPPTARGETSOL += acf_writer_SUT.cpp.so
acf_writer_SUT.cpp.so,SRCS := parser/acf_writer.cpp
acf_writer_SUT.cpp.so,DEPS :=
acf_writer_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

X86CPPTARGETSOL += xml_parser_SUT.cpp.so
xml_parser_SUT.cpp.so,SRCS := parser/xml_parser.cpp
xml_parser_SUT.cpp.so,DEPS :=
xml_parser_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

X86CPPTARGETSOL += xml_writer_SUT.cpp.so
xml_writer_SUT.cpp.so,SRCS := parser/xml_writer.cpp
xml_writer_SUT.cpp.so,DEPS :=
xml_writer_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

# core

X86CPPTARGETSOL += config_SUT.cpp.so
config_SUT.cpp.so,SRCS := config.cpp
config_SUT.cpp.so,DEPS := \
	path_SUT.cpp.so \
	\
	acf_parser_SUT.cpp.so \
	xml_parser_SUT.cpp.so
config_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

X86CPPTARGETSOL += path_SUT.cpp.so
path_SUT.cpp.so,SRCS := path.cpp
path_SUT.cpp.so,DEPS :=
path_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

X86CPPTARGETSOL += loader_SUT.cpp.so
loader_SUT.cpp.so,SRCS := loader.cpp
loader_SUT.cpp.so,DEPS :=
loader_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

X86CPPTARGETSOL += writer_SUT.cpp.so
writer_SUT.cpp.so,SRCS := writer.cpp
writer_SUT.cpp.so,DEPS := \
	acf_writer_SUT.cpp.so \
	xml_writer_SUT.cpp.so
writer_SUT.cpp.so,CPPFLAGS = $(GLOBAL_CPPFLAGS)

# ------------------------------
# Test targets
# ------------------------------

#
# smoke test
#
X86CPPTARGET += TEST_POLYCONF_MVP
TEST_POLYCONF_MVP,SRCS := test_polyconf_mvp.cpp
TEST_POLYCONF_MVP,USRLIBS := \
	config_SUT.cpp.so \
	loader_SUT.cpp.so
TEST_POLYCONF_MVP,CPPFLAGS = $(GLOBAL_CPPFLAGS)
TEST_POLYCONF_MVP,LDFLAGS =

#
# core tests
#
X86CPPTARGET += TEST_CONFIG
TEST_CONFIG,SRCS := test_config.cpp
TEST_CONFIG,USRLIBS := \
	config_SUT.cpp.so \
	loader_SUT.cpp.so
TEST_CONFIG,CPPFLAGS = $(GLOBAL_CPPFLAGS)
TEST_CONFIG,LDFLAGS =

X86CPPTARGET += TEST_PATH
TEST_PATH,SRCS := test_path.cpp
TEST_PATH,USRLIBS := \
	config_SUT.cpp.so \
	loader_SUT.cpp.so
TEST_PATH,CPPFLAGS = $(GLOBAL_CPPFLAGS)
TEST_PATH,LDFLAGS =

X86CPPTARGET += TEST_LOADER
TEST_LOADER,SRCS := test_loader.cpp
TEST_LOADER,USRLIBS := \
	config_SUT.cpp.so \
	loader_SUT.cpp.so
TEST_LOADER,CPPFLAGS = $(GLOBAL_CPPFLAGS)
TEST_LOADER,LDFLAGS =

X86CPPTARGET += TEST_WRITER
TEST_WRITER,SRCS := test_writer.cpp
TEST_WRITER,USRLIBS := \
	config_SUT.cpp.so \
	loader_SUT.cpp.so \
	writer_SUT.cpp.so
TEST_WRITER,CPPFLAGS = $(GLOBAL_CPPFLAGS)
TEST_WRITER,LDFLAGS =

#
# parser tests
#
X86CPPTARGET += TEST_ACF_PARSER
TEST_ACF_PARSER,SRCS := test_acf_parser.cpp
TEST_ACF_PARSER,USRLIBS := \
	config_SUT.cpp.so \
	loader_SUT.cpp.so
TEST_ACF_PARSER,CPPFLAGS = $(GLOBAL_CPPFLAGS)
TEST_ACF_PARSER,LDFLAGS =

X86CPPTARGET += TEST_XML_PARSER
TEST_XML_PARSER,SRCS := test_xml_parser.cpp
TEST_XML_PARSER,USRLIBS := \
	config_SUT.cpp.so \
	loader_SUT.cpp.so
TEST_XML_PARSER,CPPFLAGS = $(GLOBAL_CPPFLAGS)
TEST_XML_PARSER,LDFLAGS =


# Include shocktest makefile from dep/shocktest symlink (no absolute path).
include $(REPO_ROOT)/dep/shocktest/src/tst/ut/makefile.shocktest.mk
