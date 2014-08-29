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


#ifndef __KTXTOOL_COMPRESSION_INCLUDED
#define __KTXTOOL_COMPRESSION_INCLUDED




#include <inttypes.h>
#include <Types.h>








class Compression
{
public:

	enum Quality
	{
		QUALITY_DRAFT,
		QUALITY_LOW,
		QUALITY_MEDIUM,
		QUALITY_HIGH
	};


protected:


	Quality    m_quality;


public:

	virtual ~Compression() {}




	/** Returns the GL base internal format */
	virtual uint32_t GetBaseInternalFormat(Format format, ColorDepth depth) = 0;
	


	
	/** Returns the GL compressed internal format. */
	virtual uint32_t GetInternalFormat(Format format, ColorDepth depth) = 0;




	/** Compresses the image with the internal settings. 
	 *
	 *  Returns zero if failed, otherwise returns the output 
	 *  data size in bytes */
	virtual uint32_t Compress(void* in, void* out, int w, int h, Format format, ColorDepth depth) = 0;




	/** Sets the compression quality. The implementation is
	 *  in charge of reading this value and setting up the internal
	 *  parameters to archive the desired quality. Said that
	 *  this parameter actual quality might differ between implementations */
	inline void SetQuality(Quality level) { m_quality = level; }




	/** Gets the quality stored internally */
	inline Quality GetQuality() const { return m_quality; }




	/** Returns the expected compressed size by the dimmension,
	 *  If the size is not fixed then returns zero. */
	virtual uint32_t GetSize(int w, int h) = 0;




	/** The compression name as a string */
	virtual const char* GetName() const = 0;




};












#endif
