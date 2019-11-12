ifdef STATIC
LIBTYPE :=
LIB_SUFFIX := .a
else
LIBTYPE := -shared
CXXFLAGS += -fPIC
LIB_SUFFIX := .so
endif

CXXFLAGS += -Wall -Werror
CXXFLAGS += -I ./include/empool

LDFLAGS += -pthread

TARGET := libempool$(LIB_SUFFIX)

TEST_DIR := test
EXAMPLE_DIR := examples

HDRDIR := include
SRCDIR := src
OBJDIR := src
HDRS := $(wildcard $(HDRDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

LINT = ./cpplint.py

#### Rules ####
all: $(TARGET)

ifdef STATIC
$(TARGET): $(OBJS)
	$(AR) rcs $@ $^
else
$(TARGET): $(OBJS)
ifeq ($(shell uname),Darwin)
	$(LINK.cpp) $(LIBTYPE) -Wl  -o $@.1.0 $^ $(LDFLAGS)
else
	$(LINK.cpp) $(LIBTYPE) -Wl,-soname,$@ -o $@.1.0 $^ $(LDFLAGS)
endif
	ln -fs $@.1.0 $@
endif


clean:
	rm -fv $(OBJS) $(TARGET).1.0 $(TARGET)