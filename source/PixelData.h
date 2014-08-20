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

#ifndef __KTXTOOL_PIXELDATA_INCLUDED
#define __KTXTOOL_PIXELDATA_INCLUDED





#include <assert.h>
#include <string>




/** This class is not meant for speed neither has a small memory footprint
 *  as is intended to run on desktop computers, convert and compress
 *  your textures for runtime usage in GL or even D3D. */
class PixelData
{
public:

	/** Lossless as it gets (up to 32bits ofcourse) */
	struct Pixel
	{
		float r;
		float b;
		float g;
		float a;
	};


protected:


	Pixel*  m_pData;
	int     m_w;
	int     m_h;


public:
	
	


	/** The format it's just a hint as the pixel data has 4 components, 
	 *  regardless of this. */
	enum Format
	{
		FORMAT_RGB,
		FORMAT_RGBA
	};

	
	/** Inlined as this is just a container class */
	inline PixelData(int w, int h, Format format)
	{
		m_w = w;
		m_h = h;
		m_pData = new Pixel[w * h];
	}

	/** Deletes the data if not null*/
	inline ~PixelData()
	{
		if (m_pData)
		{
			delete m_pData;
			m_pData = NULL;
		}
	}

	
	inline Pixel& GetAt(int x, int y)
	{
		size_t i = (m_w * m_h) - ((y * m_w) + (m_w - x));


		return m_pData[i];
	}

};









#endif
