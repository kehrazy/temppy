cmake_minimum_required(VERSION 3.20)

project(temppy VERSION 0.1.0 LANGUAGES CXX)

option(TEMPPY_BUILD_EXAMPLES "Build temppy example programs" ON)
option(TEMPPY_BUILD_TESTS "Build temppy test programs" ON)

add_library(temppy INTERFACE)
add_library(temppy::temppy ALIAS temppy)

target_include_directories(temppy
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(temppy INTERFACE cxx_std_23)

function(temppy_apply_build_flags target_name)
  target_link_libraries(${target_name} PRIVATE temppy::temppy)
  target_compile_features(${target_name} PRIVATE cxx_std_23)

  if(MSVC)
    target_compile_options(${target_name} PRIVATE /W4)
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -ftemplate-depth=2000)
  endif()
endfunction()

if(TEMPPY_BUILD_EXAMPLES)
  set(TEMPPY_EXAMPLES
    basic_arithmetic
    complex
    control_flow
    language_features
    simple_demo
    variables
  )

  foreach(example_name IN LISTS TEMPPY_EXAMPLES)
    add_executable(${example_name} examples/${example_name}.cpp)
    temppy_apply_build_flags(${example_name})
  endforeach()
endif()

include(CTest)

if(BUILD_TESTING AND TEMPPY_BUILD_TESTS)
  set(TEMPPY_TESTS
    test_control_flow
    test_eval_features
    test_lexer
    test_string_literals
  )

  foreach(test_name IN LISTS TEMPPY_TESTS)
    add_executable(${test_name} tests/${test_name}.cpp)
    temppy_apply_build_flags(${test_name})
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
endif()
