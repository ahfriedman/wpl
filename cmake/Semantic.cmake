# Semantic analysis component module

set (SEMANTIC_DIR ${CMAKE_SOURCE_DIR}/src/semantic)
set (SEMANTIC_INCLUDE ${SEMANTIC_DIR}/include)

set (SEMANTIC_SOURCES
  ${SEMANTIC_DIR}/SemanticVisitor.cpp
  ${SEMANTIC_DIR}/PropertyManager.cpp
)