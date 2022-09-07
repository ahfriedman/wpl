# Symbol component module
include(LLVM)
set (SYMBOL_DIR ${CMAKE_SOURCE_DIR}/src/symbol)
set (SYMBOL_INCLUDE 
  ${SYMBOL_DIR}/include
  ${LLVM_INCLUDE_DIR}
)

set (SYMBOL_SOURCES
  ${SYMBOL_DIR}/Scope.cpp
  ${SYMBOL_DIR}/STManager.cpp
)