.SUFFIXES:

project 	:= mprisserver-example
files 		:=
main_files	:= main.cpp
test_files	:=
test_name	:=
platform 	:= linux
CC 			:= gcc
CXX 		:= g++
CFLAGS 		:= -I./external/include -std=c11 -Wall -Wextra -pedantic -Wno-unused-parameter
CXXFLAGS 	:= -I./external/include -std=c++20 -Wall -Wextra -pedantic -Wno-unused-parameter
LDLIBS 		:= -lfmt $(shell pkg-config --libs sdbus-c++)
libs_test 	:=
PREFIX		:= /usr/local
DESTDIR		:=
VPATH 		:= src:example

buildtype := debug
outdir := debug
ifeq ($(buildtype),debug)
    outdir := debug
    CFLAGS += -g -O0 -fno-inline-functions -DDEBUG
    CXXFLAGS += -g -O0 -fno-inline-functions -DDEBUG
else ifeq ($(buildtype),release)
    outdir := release
    CFLAGS += -O3 -DNDEBUG
    CXXFLAGS += -O3 -DNDEBUG
else ifeq ($(buildtype),reldeb)
	outdir := reldeb
	CFLAGS += -g -Og -DDEBUG
	CXXFLAGS += -g -Og -DDEBUG
else
	$(error error: invalid value for buildtype)
endif

objs 		:= $(patsubst %,$(outdir)/%.o,$(files))
objs_main	:= $(patsubst %,$(outdir)/%.o,$(main_files))
objs_test 	:= $(patsubst %,$(outdir)/%.o,$(test_files))
flags_deps 	= -MMD -MP -MF $(@:.o=.d)

all: $(outdir)/$(project)

test: $(outdir)/$(test_name)

install:

uninstall:

clean:
	rm -rf $(outdir)

$(outdir)/$(project): $(outdir) $(objs) $(objs_main)
	$(CXX) $(objs) $(objs_main) -o $@ $(LDLIBS)

$(outdir)/$(test_name): $(outdir) $(objs) $(objs_test)
	$(CXX) $(objs) $(objs_test) -o $@ $(LDLIBS) $(libs_test)

$(outdir):
	mkdir -p $(outdir)

-include $(outdir)/*.d

$(outdir)/%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) $(flags_deps) -c $< -o $@

$(outdir)/%.c.o: %.c
	$(CC) $(CFLAGS) $(flags_deps) -c $< -o $@

.PHONY: clean install uninstall
