INCDIR   := /opt/comptel/eventlink/sdk/include
APP      := app_aggr.so
LIBDIR   := /opt/comptel/eventlink/sdk/lib
LIBS     :=  

# Source code

CSRCS    := $(wildcard *.c)
CXXSRCS  := $(wildcard *.cc)

# Compiler and linker opts

arch     := $(shell uname -a)
ifeq (AIX,$(findstring AIX,$(arch)))
CPPFLAGS := -D__aix -I$(INCDIR)
CC       := xlc_r
CFLAGS   := -qpic -q64
CXX      := xlC_r
CXXFLAGS := $(CFLAGS) -qrtti
LD       := $(CXX)
LDFLAGS  := -G -qmkshrobj -brtl -q64 -L$(LIBDIR)
endif
ifeq (HP-UX,$(findstring HP-UX,$(arch)))
CPPFLAGS := -I$(INCDIR)
CC       := aCC
CFLAGS   := -Ae +z +DD64 -mt
CXX      := $(CC)
CXXFLAGS := -AA +z +DD64 -mt
LD       := $(CXX)
LDFLAGS  := +DD64 -b -mt -L$(LIBDIR)
endif
ifeq (Linux,$(findstring Linux,$(arch)))
CPPFLAGS := -D_REENTRANT -I$(INCDIR)
CC       := gcc
CFLAGS   := -Wall -fPIC -pthread -pipe
CXX      := g++
CXXFLAGS := $(CFLAGS)
LD       := $(CXX)
LDFLAGS  := -shared -static-libgcc -pthread -L$(LIBDIR)
LDLIBS   := `$(CXX) -print-file-name=libstdc++.so`
ifneq ($(PROJECTSTATE),production)
LDFLAGS  += -rdynamic
endif
endif
ifeq (SunOS,$(findstring SunOS,$(arch)))
CPPFLAGS := -I$(INCDIR)
CC       := cc
CFLAGS   := -xarch=v9 -KPIC -mt
CXX      := CC
CXXFLAGS := $(CFLAGS)
LD       := $(CXX)
LDFLAGS  := $(CXXFLAGS) -G -L$(LIBDIR)
endif
LDLIBS   += -lnodebase $(foreach lib,$(LIBS),$(addprefix -l,$(lib)))

ifeq (production,$(PROJECTSTATE))
CPPFLAGS += -DNDEBUG
CFLAGS   += -O
CXXFLAGS += -O
LDFLAGS  += -s -O
else
CPPFLAGS += -D_DEBUG
CFLAGS   += -g
CXXFLAGS += -g
LDFLAGS  += -g
endif

# Rules

all: $(APP)

clean:
	$(RM) *.o $(APP)

%.so %.sl: $(CSRCS:.c=.o) $(CXXSRCS:.cc=.o)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: all clean

