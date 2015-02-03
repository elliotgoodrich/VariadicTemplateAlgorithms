cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Werror -std=c++1y -static")
if(MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.53.0 REQUIRED COMPONENTS unit_test_framework)

include_directories(../include/)
include_directories(${Boost_INCLUDE_DIRS})

set(SOURCES
	main.cpp
	algorithms.cpp
)

add_executable(unit_tests ${SOURCES})

target_link_libraries(unit_tests ${Boost_LIBRARIES})
