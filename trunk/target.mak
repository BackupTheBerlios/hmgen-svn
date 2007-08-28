TARGET ?= linux-gcc

ifeq ($(TARGET),linux-gcc)
CC = gcc
CC_DEP = $(CC)
AR ?= ar
RANLIB ?= ranlib
STRIP ?= strip
PKG_CONFIG ?= pkg-config
STD_FLAGS=-std=c99
WARN_FLAGS=-Wall -W
OPT_FLAGS=-O3
DEBUG_FLAGS=-g
DEF_CFLAGS = $(STD_FLAGS) $(WARN_FLAGS) $(OPT_FLAGS) $(DEBUG_FLAGS)
DEF_LDFLAGS = -lm $(DEBUG_FLAGS)
GTK_CFLAGS = `$(PKG_CONFIG) --cflags gtk+-2.0 gthread-2.0`
GTK_LDFLAGS = `$(PKG_CONFIG) --static --libs gtk+-2.0 gthread-2.0`
DEP_FLAGS = -MM
AR_FLAGS = cru
EXESUF =
OBJSUF = .o
endif

ifeq ($(TARGET),linux-suncc)
CC = suncc
CC_DEP = $(CC)
AR ?= ar
RANLIB ?= ranlib
STRIP ?= strip
PKG_CONFIG ?= pkg-config
STD_FLAGS=-Xc
WARN_FLAGS=
OPT_FLAGS=-xO5
DEBUG_FLAGS=-g
DEF_CFLAGS = $(STD_FLAGS) $(WARN_FLAGS) $(OPT_FLAGS) $(DEBUG_FLAGS)
DEF_LDFLAGS = -lm $(DEBUG_FLAGS)
GTK_CFLAGS = `$(PKG_CONFIG) --cflags gtk+-2.0 gthread-2.0 | sed 's/-pthread//'`
GTK_LDFLAGS = `$(PKG_CONFIG) --static --libs gtk+-2.0 gthread-2.0 | sed 's/-pthread//'`
DEP_FLAGS = -xM
AR_FLAGS = cru
EXESUF =
OBJSUF = .o
endif
