### variables.mk
### Common variables used throughout the project.
###
### Author: Nathan Campos <nathan@innoveworkshop.com>

# Project
PROJECT = bolota

# Environment
PLATFORM := $(shell uname -s)

# Directories and Paths
ROOT     := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
SRCDIR   := $(ROOT)/src/Bolota
BUILDDIR := $(ROOT)/build

# Tools
CXX   = g++
AR    = ar
GDB   = gdb --args
RM    = rm -f
MKDIR = mkdir -p
TOUCH = touch
MAKE  = make

# Handle OS X-specific tools.
ifeq ($(PLATFORM), Darwin)
	CXX = clang
	GDB = lldb
endif

# Flags
CFLAGS  = -Wall -Wno-psabi --std=c++11 -I$(ROOT)/shims/linux \
	-I$(ROOT)/src/Bolota
LDFLAGS =
LIBS    =
