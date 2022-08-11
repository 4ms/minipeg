cmake_minimum_required(VERSION 3.14)
project(minipeg-tests)

# Add doctest as an interface library
add_library(DocTest INTERFACE)
target_include_directories(DocTest INTERFACE tests/)

# Add test executable
add_executable(tests doctest.cc test_envelope.cc)
# ../src/env_update.cc)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/../src)
target_compile_features(tests PRIVATE cxx_std_20)
target_link_libraries(tests DocTest)
