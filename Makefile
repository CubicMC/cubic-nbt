TARGET_LIB ?= libcubic-nbt

TARGET_TESTS ?= glados

TARGET_SPEED ?= flash

CXX	?=	g++

BUILD_DIR := build
BUILD_DIR_TESTS := build_tests
BUILD_DIR_SPEED := build_speed

SRC_DIRS := src
TEST_DIRS := tests
SPEED_DIRS := speed

INC_DIRS := include
INC_TEST_DIRS :=
INC_SPEED_DIRS :=

TARGET_LIB_FLAG := -L. -l$(TARGET_LIB:lib%=%)

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')
SRCS_TESTS := $(shell find $(TEST_DIRS) -name '*.cpp')
SRCS_SPEED := $(shell find $(SPEED_DIRS) -name '*.cpp')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
OBJS_TESTS := $(SRCS_TESTS:%=$(BUILD_DIR_TESTS)/%.o)
OBJS_SPEED := $(SRCS_SPEED:%=$(BUILD_DIR_SPEED)/%.o)

DEPS := $(OBJS:.o=.d)

ifeq ($(shell ls ./libs),)
LIB_FOLDERS :=
else
LIB_FOLDERS := $(shell ls -d ./libs/*/)
endif

INC_DIRS += $(addsuffix include,$(LIB_FOLDERS)) $(SRC_DIRS)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

INC_TEST_DIRS += $(addsuffix include,$(LIB_FOLDERS)) $(TEST_DIRS)
INC_FLAGS_TESTS := $(addprefix -I,$(INC_TEST_DIRS))

INC_SPEED_DIRS += $(addsuffix include,$(LIB_FOLDERS)) $(SPEED_DIRS)
INC_FLAGS_SPEED := $(addprefix -I,$(INC_SPEED_DIRS))

CPPFLAGS := -MMD -MP

ifeq ($(MC_VERSION), 1.21)
CPPFLAGS += -DCUBIC_MC_VERSION=1.21 -DCUBIC_MC_PROTOCOL=767
else
$(error Minecraft version not supported or MC_VERSION env variable not set)
endif

SHARED := 0

CXXFLAGS := -Wall
CXXFLAGS += -Wextra
CXXFLAGS += -Wconversion
CXXFLAGS += -std=c++17
CXXFLAGS += -Wp,-U_FORTIFY_SOURCE
CXXFLAGS += -Wformat=2
CXXFLAGS += -Wcast-qual
CXXFLAGS += -Wdisabled-optimization
CXXFLAGS += -Werror=return-type
CXXFLAGS += -Winit-self
CXXFLAGS += -Winline
CXXFLAGS += -Wredundant-decls
CXXFLAGS += -Wshadow
CXXFLAGS += -Wundef
CXXFLAGS += -Wunreachable-code
CXXFLAGS += -Wwrite-strings
CXXFLAGS += -Wno-missing-field-initializers

ifeq ($(CXX), g++)
CXXFLAGS += -Wduplicated-branches
CXXFLAGS += -Wduplicated-cond
CXXFLAGS += -Werror=vla-larger-than=0
CXXFLAGS += -Wlogical-op
endif

AR := ar

ARFLAGS := rcsPv

LDFLAGS :=

ifeq ($(SHARED), 1)
CXXFLAGS += -fPIC
AR := $(CXX)
ARFLAGS := -shared -o
TARGET_LIB := $(TARGET_LIB).so
else
TARGET_LIB := $(TARGET_LIB).a
endif

ifeq ($(CUBIC_WRAP), 1)
CXXFLAGS += -DNBT_CUBIC_WRAP
endif

ifeq ($(NATIVE), 1)
CXXFLAGS += -pipe
CXXFLAGS += -march=native -mtune=native
endif

ifeq ($(DEBUG), 1)
CXXFLAGS += -Og -ggdb
else
CXXFLAGS += -O3 -DNDEBUG
endif

ifeq ($(LTO), 1)
CXXFLAGS += -flto
endif

ifeq ($(ASAN), 1)
CXXFLAGS += -fsanitize=address,leak,undefined
LDFLAGS += -fsanitize=address,leak,undefined
endif

# -fanalyzer is quite broken in g++, deactivate by default
ifeq ($(ANALYZER), 1)
ifeq ($(CXX), g++)
CXXFLAGS += -fanalyzer
CXXFLAGS += -Wno-analyzer-use-of-uninitialized-value
endif
endif

$(TARGET_LIB): $(BUILD_DIR)/$(TARGET_LIB)
	cp $(BUILD_DIR)/$(TARGET_LIB) $(TARGET_LIB)

$(BUILD_DIR)/$(TARGET_LIB): CPPFLAGS += $(INC_FLAGS)
$(BUILD_DIR)/$(TARGET_LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(TARGET_TESTS): $(BUILD_DIR_TESTS)/$(TARGET_TESTS)
	cp $(BUILD_DIR_TESTS)/$(TARGET_TESTS) $(TARGET_TESTS)

$(BUILD_DIR_TESTS)/$(TARGET_TESTS): CPPFLAGS += -DUNIT_TESTS=1 $(INC_FLAGS_TESTS) $(INC_FLAGS)
$(BUILD_DIR_TESTS)/$(TARGET_TESTS): CXXFLAGS += --coverage
$(BUILD_DIR_TESTS)/$(TARGET_TESTS): LDFLAGS += -lcriterion --coverage
$(BUILD_DIR_TESTS)/$(TARGET_TESTS): $(OBJS_TESTS) $(NEEDED_LIBS) $(TARGET_LIB)
	$(CXX) $(OBJS_TESTS) -o $@ $(LDFLAGS) $(TARGET_LIB_FLAG)

$(TARGET_SPEED): $(BUILD_DIR_SPEED)/$(TARGET_SPEED)
	cp $(BUILD_DIR_SPEED)/$(TARGET_SPEED) $(TARGET_SPEED)

$(BUILD_DIR_SPEED)/$(TARGET_SPEED): CPPFLAGS += $(INC_FLAGS_SPEED) $(INC_FLAGS)
$(BUILD_DIR_SPEED)/$(TARGET_SPEED): $(OBJS_SPEED) $(NEEDED_LIBS) $(TARGET_LIB)
	$(CXX) $(OBJS_SPEED) -o $@ $(LDFLAGS) $(TARGET_LIB_FLAG) -lz

$(BUILD_DIR_TESTS)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR_SPEED)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BUILD_DIR_TESTS)
	rm -rf $(BUILD_DIR_SPEED)

.PHONY: fclean
fclean: clean
	rm -f $(TARGET_LIB)
	rm -f $(TARGET_TESTS)
	rm -f $(TARGET_SPEED)

.PHONY: re
re: fclean
	$(MAKE) $(TARGET_LIB)

.PHONY: all
all: $(TARGET_LIB)

.PHONY: tests_run
tests_run: $(TARGET_TESTS)
	./$(TARGET_TESTS)

.PHONY: speedrun
speedrun: $(TARGET_SPEED)
	./$(TARGET_SPEED)

-include $(DEPS)
