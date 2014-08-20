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









/** This class is not meant for speed neither has a small memory footprint
 *  as is intended to run on desktop computers, convert and compress
 *  your textures for runtime usage in GL or even D3D. */
class PixelData
{
protected:


	Pixel*  m_pData;

public:
	
	
	/** Lossless as it gets (up to 32bits ofcourse) */
	struct Pixel
	{
		float r;
		float b;
		float g;
		float a;
	};

	
	enum Format
	{
		FORMAT_RGB,
		FORMAT_RGBA
	};

	enum ColorDepth
	{
		COLOR_DEPTH_8BIT,
		COLOR_DEPTH_16BIT,
		COLOR_DEPTH_24BIT,
		COLOR_DEPTH_32BIT
	};
	

	

	
	
	inline PixelData()
	{
		m_pData = NULL;
	}

	inline ~PixelData()
	{
		Free();
		
	}

	inline void Allocate(size_t N)
	{
		m_pData = new Pixel[N];
	}

	inline void Free()
	{
		if (m_pData)
		{
			delete m_pData;
			m_pData = NULL;
		}
	}


};









#endif
