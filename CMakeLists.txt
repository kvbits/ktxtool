# 
# ktxtool, A convertion/compression tool for the KTX image format
#
# Copyright (C) 2014 Luis Jimenez, www.kvbits.com
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. 
#

cmake_minimum_required(VERSION 2.6)

project(ktxtool)


#dependencies
find_package(libPNG REQUIRED)


set(SOURCES 
	source/ktxtool.cpp
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -D_DEBUG")

include_directories(source/)

#make
add_executable(ktxtool ${SOURCES})


#install
install(TARGETS ktxtool DESTINATION bin)

