.SECONDEXPANSION:

CPP=g++
CPPFLAGS=-std=c++11 -Iinclude

SOURCES=$(shell find src -type f -name '*.cpp')
OBJECTS=$(patsubst src/%.cpp,build/%.o,$(SOURCES))

TEST_SOURCES=$(shell find test/src -type f -name '*.cpp')
TEST_OBJECTS=$(patsubst test/src/%.cpp,build/test/%.o,$(TEST_SOURCES))
TEST_OBJECTS+=build/test/contrib/gmock/gmock-all.o
TEST_OBJECTS+=build/test/contrib/gtest/gtest-all.o
TEST_OBJECTS+=build/test/contrib/gtest/gtest_main.o

TEST_CPPFLAGS=-Itest/include -Itest/contrib/include

all: build/.f build/libbadiff.so build/badiff.test

build/%: $$(@D)/.f

build/%.o: src/%.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $*

build/libbadiff.so: $(OBJECTS)
	$(CPP) $(CPPFLAGS) -shared -o $@ $*

build/test/%.o: test/src/%.cpp
	$(CPP) $(CPPFLAGS) $(TEST_CPPFLAGS) -c -o $@ $*

build/test/contrib/gtest/%.o: test/contrib/src/gtest/%.cc
	$(CPP) $(CPPFLAGS) $(TEST_CPPFLAGS) -c -o $@ $*

build/test/contrib/gmock/%.o: test/contrib/src/gmock/%.cc
	$(CPP) $(CPPFLAGS) $(TEST_CPPFLAGS) -c -o $@ $*

build/badiff.test: $(OBJECTS) $(TEST_OBJECTS)
	$(CPP) $(CPPFLAGS) -o $@ $*

.PRECIOUS: %/.f
%/.f:
	mkdir -p $(dir $@)
	touch $@	