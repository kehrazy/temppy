CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -I./include -ftemplate-depth=2000 -O0 -static

EXAMPLES_DIR = examples
BUILD_DIR = build

EXAMPLES = $(BUILD_DIR)/basic_arithmetic $(BUILD_DIR)/variables $(BUILD_DIR)/complex
TESTS = $(BUILD_DIR)/test_lexer $(BUILD_DIR)/test_parser $(BUILD_DIR)/test_eval

.PHONY: all examples tests clean run-examples run-tests

all: examples tests

examples: $(EXAMPLES)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/basic_arithmetic: $(EXAMPLES_DIR)/basic_arithmetic.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/variables: $(EXAMPLES_DIR)/variables.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/complex: $(EXAMPLES_DIR)/complex.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

run-examples: examples
	@echo "Running Basic Arithmetic Example:"
	@./$(BUILD_DIR)/basic_arithmetic
	@echo ""
	@echo "Running Variables Example:"
	@./$(BUILD_DIR)/variables
	@echo ""
	@echo "Running Complex Example:"
	@./$(BUILD_DIR)/complex

clean:
	rm -rf $(BUILD_DIR)
