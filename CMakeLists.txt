cmake_minimum_required (VERSION 2.6)
project (qmread)
add_executable(qmr main.cpp lmfile.cpp histogram.cpp)


# set(CMAKE_BUILD_TYPE Release)

# The version number.
#set (Tutorial_VERSION_MAJOR 1)
#set (Tutorial_VERSION_MINOR 0)
 
# configure a header file to pass some of the CMake settings
# to the source code
#configure_file (
#  "${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
#  "${PROJECT_BINARY_DIR}/TutorialConfig.h"
#  )

# CMAKE_CXX_FLAGS="O2"


add_library(lm SHARED lmfile.cpp histogram.cpp)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests tests.cpp)
target_link_libraries(runTests lmfile histogram ${GTEST_LIBRARIES} pthread)

