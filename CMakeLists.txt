cmake_minimum_required(VERSION 3.2)
project(type-racer)

set(CMAKE_BUILD_TYPE Debug)

# Use our modified FindSDL2* modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${type-racer_SOURCE_DIR}/cmake")
set(BIN_DIR ${type-racer_SOURCE_DIR}/bin)

# Bump up warning levels type-racerropriately for clang, gcc & msvc and build in debug mode
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++17")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
endif()

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

include_directories("./src")
file (GLOB type-racer_SOURCES src/*.cc)

add_executable(type-racer ${type-racer_SOURCES})
target_link_libraries(type-racer ${CURSES_LIBRARY})
install(TARGETS type-racer RUNTIME DESTINATION ${BIN_DIR})
