# semantic listfile
#
include(Semantic)
include(Symbol)
include(ANTLR)
# include(Utility)

add_library(semantic_lib OBJECT
  ${SEMANTIC_SOURCES}
)

##############################################
# Modify for your implementation.
##############################################
add_dependencies(semantic_lib 
  lexparse_lib
#   utility_lib
)

##############################################
# Modify for your implementation.
##############################################
# include_directories(semantic_lib
#   ${ANTLR_INCLUDE}
#   ${ANTLR_GENERATED_DIR}
#   ${SYMBOL_INCLUDE}
#   ${SEMANTIC_INCLUDE}
#   ${UTILITY_INCLUDE}
# )
