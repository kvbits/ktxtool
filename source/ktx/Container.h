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

#ifndef __KTXTOOL_CONTAINER_INCLUDED
#define __KTXTOOL_CONTAINER_INCLUDED






#include <string.h>
#include <inttypes.h>





/** KTX Container class that's able to write and read a ktx file */
class Container
{
public:

	
	/** Reference: http://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/#1 */ 
	struct Header
	{
		char     identifier[12];
		uint32_t endianness;
		uint32_t glType;
		uint32_t glTypeSize;
		uint32_t glFormat;
		uint32_t glInternalFormat;
		uint32_t glBaseInternalFormat;
		uint32_t pixelWidth;
		uint32_t pixelHeight;
		uint32_t pixelDepth;
		uint32_t numberOfArrayElements;
		uint32_t numberOfFaces;
		uint32_t numberOfMipmapLevels;
		uint32_t bytesOfKeyValueData;

		Header()
		{
			memset(this, 0, sizeof(Header));
		}


	};


	/** Mipmap level */
	struct MipmapLevel
	{
		
	};



protected:


	Header    m_header;



public:



	Container();



	/** Checks the header for the identifier, assuming this was loaded from file */
	bool HasValidIdentifier() const;

	
	/** Initialized the header for write operations */
	void Init(int w, int h);



	/** Checks if the dimmension is square and power of 2 */
	inline bool IsSqrPowerOf2()
	{
		const int w = m_header.pixelWidth;
		const int h = m_header.pixelHeight;

		assert(w != 0 && h !=0);

		if (w != h) return false;

		return w > 0 && !(w & (w − 1));
	}

};














#endif
