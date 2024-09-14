# Project
PROJECT = bolota

# Directories and Paths
SRCDIR     := src
BUILDDIR   := build
TARGET     := $(BUILDDIR)/$(PROJECT)

# Tools
CC    = cc
GDB   = gdb --args
RM    = rm -f
MKDIR = mkdir -p
TOUCH = touch

# Flags
CFLAGS  = -Wall --std=gnu89
LDFLAGS =
LIBS    =
