if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Werror -std=c++1y")
endif()

if(MINGW)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif()

include_directories(../include/)

add_executable(concat concat.cpp)
add_executable(reverse_polish reverse_polish.cpp)
add_executable(safe_mean safe_mean.cpp)
add_executable(switch_bools switch_bools.cpp)
