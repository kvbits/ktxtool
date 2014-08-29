/*
 * ktxtool, A conversion and compression tool for the KTX image format
 *
 * Copyright (C) 2014 Luis Jimenez, www.kvbits.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. 
 */


#ifndef __KTXTOOL_COMPRESSION_ETC1_INCLUDED
#define __KTXTOOL_COMPRESSION_ETC1_INCLUDED




#include <ktx/Compression/Compression.h>



class ETC1 : public Compression
{
public:

	
	ETC1();
	virtual ~ETC1();


	
	uint32_t GetBaseInternalFormat(Format format, ColorDepth depth);

	uint32_t GetInternalFormat(Format format, ColorDepth depth);

	uint32_t GetSize(int w, int h);

	uint32_t Compress(void* in, void* out, int w, int h, Format format, ColorDepth depth);


	const char* GetName() const { return "ETC1 - Ericsson Texture Compression"; }

};












#endif
