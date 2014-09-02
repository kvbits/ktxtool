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
#include <Types.h>






/** This class is not meant for speed neither has a small memory footprint
 *  as is intended to run on desktop computers, convert and compress
 *  your textures for runtime usage in GL or even D3D. */
class PixelData
{
protected:


	float*  m_pData;
	int     m_w;
	int     m_h;
	int     m_compCount;

	/** The format it's just a hint as the pixel data has 4 components, 
	 *  regardless of this. */
	Format  m_format;

public:
	
	


	
	/** Inlined as this is just a container class */
	inline PixelData(int w, int h, Format format)
	{
		switch (format)
		{
		case FORMAT_RGB:  m_compCount = 3; break;
		case FORMAT_RGBA: m_compCount = 4; break;
		}

		m_format = format;
		m_w = w;
		m_h = h;
		m_pData = new float[(w * h) * m_compCount];
	}

	/** Deletes the data if not null*/
	inline ~PixelData()
	{
		if (m_pData)
		{
			delete[] m_pData;
			m_pData = nullptr;
		}
	}

	
	/*inline Pixel& GetAt(int x, int y)
	{
		size_t i = (m_w * m_h) - ((y * m_w) + (m_w - x));


		return m_pData[i];
	}*/

	inline float& Get(int i)
	{
		return m_pData[i];
	}

	inline float* GetPixel(int i)
	{
		return m_pData + (i * m_compCount);
	}

	inline Format GetFormat() const { return m_format; }

	inline void* GetData() { return m_pData; }

	inline int GetWidth() const { return m_w; }
	inline int GetHeight() const { return m_h; }
	inline int GetPixelCount() { return m_w * m_h; }
	inline int GetComponentCount() const { return m_compCount; }

};









#endif
