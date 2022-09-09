# README

This is the starting project structure for your WPL compiler project. It is based upon the Calculator project that we used in the notebooks. I have tried to make this as easy as possible for you te get going.
The first thing you should do is change the name of the project from **wplstarter** to **wpl-yourname** 

## Prerequisites

You should have successfully gotten the calculator example to build and run. I have not included the test target so you don't have to worry about the problems that arose there. You can certainly add a test target using whatever test framework you choose.

## Setting up

You need to copy some files from the working calculator example that on your platform into this project. First copy the working antlr4-runtime.a library into the antlr/lib directory. As an alternative, you can rebuild the library. The zipped archive for it is in the antlr directory. Remember to build the library outside of the project.

If you made changes for your platform to any of the .cmake files or CMakeLists.txt files, you will probably need to make them in this project as well.

## Structure

The structure of this project is the same as the final Calculator project. I have removed some directories that were not relevant to the WPL compiler. I have also commented out several things in the CMakeLists.txt files. The idea is that you will build one phase (library or target) at a time. If you read these files and the .cmake files you can uncomment and add as necessary for your design.

The main target here is the wplc compiler. The source for this is wplc.cpp in the src directory. I have changed the names and commented out things, but it is the driver for the Calculator compiler. You can change it as long as it accepts the same command line arguments.