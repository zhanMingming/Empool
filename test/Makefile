TARGET := tpool.test
PROJECT_ROOT := ..
DEPEND_LIB := $(wildcard $(PROJECT_ROOT)/libtpool.*)

CXXFLAGS += -I ../include -std=c++11
CXXFLAGS += -Wall -Werror

LDFLAGS += -pthread
LDFLAGS += -lgtest_main -lgtest
LDFLAGS += -L .. -ltpool

SRCDIR := .
OBJDIR := .
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)


#### Rules ####
all: $(TARGET)

$(TARGET): $(OBJS) $(DEPEND_LIB)
	g++ -o $@ $(OBJS) $(LDFLAGS)

ifeq ($(shell uname),Darwin)
	DYLD_LIBRARY_PATH=$(PROJECT_ROOT) ./$@
else
	LD_LIBRARY_PATH=$(PROJECT_ROOT) ./$@
endif

clean:
	rm -fv $(OBJS) $(TARGET)

#include  $(PROJECT_ROOT)/Makefile.rules
