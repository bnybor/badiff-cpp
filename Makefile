.SECONDEXPANSION:

CPP=g++
CPPFLAGS=-std=c++11 -Iinclude

SOURCES=$(shell find src -type f -name '*.cpp')
OBJECTS=$(patsubst src/%.cpp,build/%.o,$(SOURCES))

TEST_SOURCES=$(shell find test/src -type f -name '*.cpp')
TEST_OBJECTS=$(patsubst test/src/%.cpp,build/test/%.o,$(TEST_SOURCES))

TEST_CPPFLAGS=-Itest/include -Itest/contrib/include

all: build/libbadiff.so

build/%.o: src/%.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $*

build/libbadiff.so: $(OBJECTS)
	$(CPP) $(CPPFLAGS) -shared -o $@ $*

build/test/%.o: test/src/%.cpp
	$(CPP) $(CPPFLAGS)  -c -o $@ $*

build/test/contrib/%.o: test/contrib/src/%.cpp
	$(CPP) $(CPPFLAGS)  -c -o $@ $*

build/badiff.test: $(OBJECTS) $(TEST_OBJECTS)
	$(CPP) $(CPPFLAGS) -o $@ $*
