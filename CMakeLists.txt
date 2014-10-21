# 
# ktxtool, A conversion and compression tool for the KTX image format
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


enable_testing()


set(TEST_IMG ${CMAKE_SOURCE_DIR}/tests/data/brick_rgb.tiff)

#for testing purposes just an small image to reduce compression times
set(TEST_IMG_SMALL ${CMAKE_SOURCE_DIR}/tests/data/grid_rgb.gif)


add_test(standard                        ktxtool ${TEST_IMG} out.ktx)
add_test(standard-with-compression       ktxtool -c ${TEST_IMG_SMALL} out.ktx)
add_test(multiple-faces                  ktxtool ${TEST_IMG},${TEST_IMG},${TEST_IMG} out.ktx)
add_test(multiple-faces-with-compression ktxtool -c ${TEST_IMG_SMALL},${TEST_IMG_SMALL},${TEST_IMG_SMALL} out.ktx)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/)


option(WITH-PPM "Build with non-lib dependant .ppm support" true)
option(WITH-TBB "Build with multithread support (for compression) using Intel's libTBB" false) 
option(WITH-TIFF "Build with libtiff for standalone .tiff support" false)
option(WITH-IMAGEMAGICK "Support for any* input formats" false)


set(INCLUDES source/)

set(SOURCES 
	source/ktxtool.cpp
	source/ktx/Container.cpp
	source/ktx/Compression/ETC1/ETC1.cpp
	source/ktx/Compression/ETC1/rg_etc1.cpp
)

set(LIBRARIES)







#multithread support
if(WITH-TBB)

	add_definitions(-DKTXTOOL_TBB)

	find_package(TBB REQUIRED)
	set(INCLUDES ${INCLUDES} ${TBB_INCLUDE_DIRS})
	set(LIBRARIES ${LIBRARIES} ${TBB_LIBRARIES})


endif(WITH-TBB)





#with imagemagick
if(WITH-IMAGEMAGICK)


	add_definitions(-DKTXTOOL_MAGICK)
	
	find_package(ImageMagick REQUIRED)
	find_package(ImageMagick COMPONENTS MagickWand REQUIRED)	

	set(SOURCES ${SOURCES} source/Formats/imagick/MagickInputFormat.cpp)
	set(INCLUDES ${INCLUDES} ${ImageMagick_INCLUDE_DIRS})
	set(LIBRARIES ${LIBRARIES} ${ImageMagick_LIBRARIES})

else(WITH-IMAGEMAGICK)
	#else because with imagemagick we dont need specific support for other formats

	if(WITH-PPM)

		set(SOURCES ${SOURCES} source/Formats/ppm/PPMInputFormat.cpp)

	endif(WITH-PPM)


	#With libtiff support
	if(WITH-TIFF)
		find_package(TIFF REQUIRED)

		set(SOURCES ${SOURCES} source/Formats/tiff/TIFFInputFormat.cpp)
		set(INCLUDES ${INCLUDES} ${TIFF_INCLUDE_DIRS})
		set(LIBRARIES ${LIBRARIES} ${TIFF_LIBRARY})

	endif(WITH-TIFF)


endif(WITH-IMAGEMAGICK)





set(CMAKE_CXX_FLAGS " -std=c++0x ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb -D_DEBUG")

include_directories(source/ ${INCLUDES})




add_executable(ktxtool ${SOURCES})

target_link_libraries(ktxtool ${LIBRARIES})

#install
install(TARGETS ktxtool DESTINATION bin)

