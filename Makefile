.SECONDEXPANSION:

SOURCE_DIRS += src
SOURCES += $(foreach dir,$(shell find $(SOURCE_DIRS) -type d),$(wildcard $(dir)/*.cpp)
SOURCES += $(foreach dir,$(shell find $(SOURCE_DIRS) -type d),$(wildcard $(dir)/*.cc)
INCLUDE_DIRS += include

CPPFLAGS += -std=c++11
CPPFLAGS += $(foreach dir,$(INCLUDE_DIRS),-I$(dir))

TEST_SOURCE_DIRS += test/src
TEST_SOURCE_DIRS += test/contrib/src
TEST_SOURCES += $(foreach dir,$(shell find $(TEST_SOURCE_DIRS) -type d),$(wildcard $(dir)/*.cpp)
TEST_SOURCES += $(foreach dir,$(shell find $(TEST_SOURCE_DIRS) -type d),$(wildcard $(dir)/*.cc)

TEST_INCLUDE_DIRS += test/include
TEST_INCLUDE_DIRS += test/contrib/include
