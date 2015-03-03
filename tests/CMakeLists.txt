cmake_minimum_required(VERSION 2.6)

if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Werror -std=c++1y")
endif()

if(MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

find_package(Boost 1.55.0 REQUIRED COMPONENTS unit_test_framework)

if(WIN32)
	set(Boost_USE_STATIC_LIBS        ON)
	set(Boost_USE_MULTITHREADED      ON)
	set(Boost_USE_STATIC_RUNTIME    OFF)
else()
	add_definitions(-DBOOST_TEST_DYN_LINK)
endif()

include_directories(../include/)
include_directories(${Boost_INCLUDE_DIRS})

set(SOURCES
	main.cpp
	algorithms.cpp
)

add_executable(unit_tests ${SOURCES})
target_link_libraries(unit_tests ${Boost_LIBRARIES})
add_test(unit_tests unit_tests)
