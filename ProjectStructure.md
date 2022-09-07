# Calculator Example project structure

| Directory         | CMake Variable          | Purpose                       |
| :--               | :---          | :---                          |
| /                 | CMAKE_SOURCE_DIR  | Top-level directory           | 
| /cmake            |               | Contains files for global settings and other policies |
| /antlr            |               | ANTLR headers, the JAR file, and the runtime goes here |
| /antlr/include    | ANTLR_INCLUDE | All headers for the ANTLR runtime |
| /antlr/lib        | ANTLR_LIB     | ANTLR c++ runtime library (static)
| /src              |               | All of our code is here and in subdirectories |
| /src/cmake        |               | CMake files for all sources |
| /src/generated    | ANTLR_GENERATED_DIR | Generated files go here, mainly ANTLR generated files |
| /src/lexparse     |               | ANTLR grammar and scripts |
| /src/utility      | Utility       | Library/libraries of utility classes, etc. |
| /test             | Test root     | Root for all tests |
| /test/lexparse    |               | Tests for the scanner and parser |


## CMake files

| File              | Purpose |
| :--               | :---    |
| /CMakeLists.txt   | Top-level CMake file. Includes