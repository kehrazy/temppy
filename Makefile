CXX ?= g++
CPPFLAGS ?= -I./include
CXXFLAGS ?= -std=c++23 -O0 -Wall -Wextra -Wpedantic -ftemplate-depth=2000
LDFLAGS ?=

BUILD_DIR := build
EXAMPLES_DIR := examples
TESTS_DIR := tests
EXAMPLE_BUILD_DIR := $(BUILD_DIR)/examples
TEST_BUILD_DIR := $(BUILD_DIR)/tests

EXAMPLE_NAMES := basic_arithmetic variables complex language_features control_flow simple_demo
TEST_NAMES := test_eval_features test_lexer test_string_literals test_control_flow

EXAMPLES := $(addprefix $(EXAMPLE_BUILD_DIR)/,$(EXAMPLE_NAMES))
TESTS := $(addprefix $(TEST_BUILD_DIR)/,$(TEST_NAMES))

.PHONY: all examples tests clean run-examples run-tests

all: examples tests

examples: $(EXAMPLES)

tests: $(TESTS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(EXAMPLE_BUILD_DIR): | $(BUILD_DIR)
	mkdir -p $(EXAMPLE_BUILD_DIR)

$(TEST_BUILD_DIR): | $(BUILD_DIR)
	mkdir -p $(TEST_BUILD_DIR)

$(EXAMPLE_BUILD_DIR)/%: $(EXAMPLES_DIR)/%.cpp | $(EXAMPLE_BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

$(TEST_BUILD_DIR)/%: $(TESTS_DIR)/%.cpp | $(TEST_BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

run-examples: examples
	@for exe in $(EXAMPLES); do \
		echo "Running $$(basename $$exe):"; \
		./$$exe; \
		echo ""; \
	done

run-tests: tests
	@for exe in $(TESTS); do \
		echo "Running $$(basename $$exe):"; \
		./$$exe; \
	done

clean:
	rm -rf $(BUILD_DIR)
