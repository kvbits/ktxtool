cmake_minimum_required(VERSION 2.6)

project(ktxtool)


set(SOURCES 
	source/ktxtool.cpp
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -D_DEBUG")

include_directories(source/)

#make
add_executable(ktxtool ${SOURCES})


#install
install(TARGETS ktxtool DESTINATION bin)

