# Top-level CMake file for this the Calculator example

cmake_minimum_required(VERSION 3.20.0)
project(expressions 
  LANGUAGES CXX C
  VERSION 0.1
  DESCRIPTION "Reference compiler to compile a simple expression language to LLVM IR"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(NoInSourceBuilds)

include(ProjectGlobals) # Platform independent variables
include(platform_settings)     # Platform specific variables

add_subdirectory(src bin)

set (CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
# install(
#   TARGETS 
#     calculator
#   DESTINATION install
# )
# include(Install)

### Testing with CTEST
enable_testing()
add_subdirectory(test)

### Examples
add_subdirectory(examples)

### Sandbox
add_subdirectory(sandbox EXCLUDE_FROM_ALL)