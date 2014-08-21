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
#include <assert.h>
#include <inttypes.h>
#include <vector>
#include <Types.h>





class Compression;
class PixelData;






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

	struct Face
	{
		void* pData;
		size_t size;
	};

	typedef std::vector<Face> FaceArray;

	/** Mipmap level */
	struct MipmapLevel
	{
		int w;
		int h;
		FaceArray faces; //usually just 1 if not a cubemap
	};

	
	typedef std::vector<MipmapLevel> MipmapArray;



	
	typedef std::vector<MipmapArray> ElementArray;


protected:


	Header        m_header;
	ElementArray  m_elements; //aka texture array

	Compression*  m_pCompression;
	Format        m_format;
	ColorDepth    m_depth;


public:



	Container();



	/** Checks the header for the identifier, assuming this was loaded from file */
	bool HasValidIdentifier() const;



	
	/** Initialized the header for write operations. 
	 *  Element and face count initialized the internal arrays
	 *  and header members. */
	void Init(int w, int h, int elementCount, int faceCount);




	/** Sets the format, color depth and compression. This must be called before
	 *  SetData. This method takes ownership of the compression object. */
	void SetFormat(Format format, ColorDepth depth, Compression* pComp = NULL);




	/** This allocates and sets the pixel data in its final format. 
	 *  The format and compression should be defined before calling 
	 *  this method. */
	void SetData(int elementIndex, int faceIndex, PixelData* pData, bool generateMipmaps = false);



	
	/** Generates mipmaps if the dimmensions allows it. Even tho some devices support
	 *  non-power of 2 mipmaps, this method does not support it to discourage bad art 
	 *  practices. */
	void GenerateMipmaps();

	

	
	/** Writes the ktx container to file. If there's an issue writing the file
	 *  it will return false */
	bool Write(const char* filePath) const;




	/** Reads the container from a file. This will call init and initialize 
	 *  everything up */
	bool Read(const char* filePath) const;





	/** Checks if the dimmension is square and power of 2 */
	inline bool IsSqrPowerOf2()
	{
		int w = m_header.pixelWidth;
		int h = m_header.pixelHeight;

		assert(w != 0 && h !=0);

		if (w != h) return false;

		return w > 0 && !(w & (w - 1));
	}







};














#endif
