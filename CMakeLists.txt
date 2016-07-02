cmake_minimum_required (VERSION 2.6)
project (qmread)
#add_executable(qmr main.cpp lmfile.cpp histogram.cpp)
add_executable(qmr main.cpp)

target_link_libraries(qmr lm)

# set(CMAKE_BUILD_TYPE Release)

# The version number.
#set (Tutorial_VERSION_MAJOR 1)
#set (Tutorial_VERSION_MINOR 0)

# CMAKE_CXX_FLAGS="O2"


add_library(lm SHARED lmfile.cpp histogram.cpp)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests tests.cpp)
target_link_libraries(runTests lm ${GTEST_LIBRARIES} pthread)

add_test(all runTests)
