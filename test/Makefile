CC=gcc
CXX=g++
CFLAGS= -Og -g
CPPFLAGS= -MT $@ -MMD -MP -MF build/$*.d -I ../stage3/include
CXXFLAGS= -Og -g -std=c++11 -fno-exceptions -fno-rtti
SRCS=slabtest.cpp slab.cpp
OBJS=$(patsubst %.cpp, build/%.o, $(patsubst %.c, build/%.o, $(SRCS)))
DEPS=$(patsubst %.cpp, build/%.d, $(patsubst %.c, build/%.d, $(SRCS)))
all: test

slab.cpp: ../stage3/mem/slab.cpp
	ln -s -f $< $@

test: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

build/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -c -o $@

build/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -c -o $@

clean:
	rm -fr test slab.cpp build/

-include $(DEPS)
