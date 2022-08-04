cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi-gcc.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(
  minipeg
  C
  CXX
  ASM
)

add_subdirectory(src/f423-drivers f423)
add_subdirectory(src/f746-drivers f746)
