enable_testing()

include(CheckCXXCompilerFlag)

# CMAKE_CXX_FLAGS="O2"
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11" )

cmake_minimum_required (VERSION 2.6)
project (qmread)

# add_executable(qmr main.cpp)
add_executable(qmrdump dump.cpp)

target_link_libraries(qmrdump lm )

# set(CMAKE_BUILD_TYPE Release)

add_library(lm SHARED lmfile.cpp histogram.cpp)

# Locate GTest
find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
 
# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests tests.cpp)

add_test(all runTests)


#set(Boost_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED OFF)  
#set(Boost_USE_STATIC_RUNTIME ON) 
#find_package(Boost 1.56.0 COMPONENTS filesystem program_options) 

#if(Boost_FOUND)
	#  include_directories(${Boost_INCLUDE_DIRS})
	#  target_link_libraries(runTests lm ${GTEST_LIBRARIES} pthread)
	#endif()



