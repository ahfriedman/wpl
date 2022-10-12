include(FetchContent)
FetchContent_Declare (
  tabulate
  GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
  GIT_TAG        v1.4
)

FetchContent_MakeAvailable(tabulate)


message("HI ${tabulate_SOURCE_DIR} THERE ${tablulate_INCLUDE_DIR}")
list(APPEND CMAKE_MODULE_PATH ${tabulate_SOURCE_DIR}/include)
# include(CTest)
# include(Catch)
# list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

# # Comment out the next line to change the command line options
# # See: https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md
# # Also, run ctest --extra-verbose
# set(CMAKE_CTEST_ARGUMENTS "--verbose; --output-on-failure")
