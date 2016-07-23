enable_testing()

include(CheckCXXCompilerFlag)

#CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
#CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
#if(COMPILER_SUPPORTS_CXX11)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#elseif(COMPILER_SUPPORTS_CXX0X)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
#else()
#        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
#endif()

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -lboost_system" )

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

add_test(all runTests)


set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.56.0 COMPONENTS filesystem program_options) 

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(runTests lm ${GTEST_LIBRARIES} pthread ${Boost_LIBRARIES})
endif()