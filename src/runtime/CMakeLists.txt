# Runtime library CMakeLists.txt

include(Runtime)

add_library(calc_runtime OBJECT
  ${RUNTIME_SOURCES}
)
