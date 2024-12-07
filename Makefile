.SECONDEXPANSION:

CPP=g++
CPPFLAGS=-g -std=gnu++11 -Iinclude -fPIC

SOURCES=$(shell find src -type f -name '*.cpp')
OBJECTS=$(patsubst src/%.cpp,build/%.o,$(SOURCES))

TEST_SOURCES=$(shell find test/src -type f -name '*.cpp')
TEST_OBJECTS=$(patsubst test/src/%.cpp,build/test/%.o,$(TEST_SOURCES))
TEST_OBJECTS+=build/test/contrib/gmock/gmock-all.o
TEST_OBJECTS+=build/test/contrib/gtest/gtest-all.o
TEST_OBJECTS+=build/test/contrib/gtest/gtest_main.o

TEST_CPPFLAGS=-Itest/include -Itest/contrib/include

all: build/libbadiff.so build/badiff.test

build/%.o: src/%.cpp | $$(@D)/.f
	$(CPP) $(CPPFLAGS) -c -o $@ $^

build/libbadiff.so: $(OBJECTS)
	$(CPP) $(CPPFLAGS) -shared -o $@ $^

build/test/%.o: test/src/%.cpp | $$(@D)/.f
	$(CPP) $(CPPFLAGS) $(TEST_CPPFLAGS) -c -o $@ $^

build/test/contrib/gtest/%.o: test/contrib/src/gtest/%.cc | $$(@D)/.f
	$(CPP) $(CPPFLAGS) $(TEST_CPPFLAGS) -c -o $@ $^

build/test/contrib/gmock/%.o: test/contrib/src/gmock/%.cc | $$(@D)/.f
	$(CPP) $(CPPFLAGS) $(TEST_CPPFLAGS) -c -o $@ $^

build/badiff.test: $(OBJECTS) $(TEST_OBJECTS)
	$(CPP) $(CPPFLAGS) -o $@ $^ -lstdc++

.PRECIOUS: %/.f
%/.f:
	mkdir -p $(dir $@)
	touch $@	
