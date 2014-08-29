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


#include "ETC1.h"
#include <iostream>
#include <string>
#include "rg_etc1.h"
#include <assert.h>
#include <cstring>





using namespace rg_etc1;
using namespace std;



	
ETC1::ETC1()
{
	m_quality = QUALITY_DRAFT;
}

ETC1::~ETC1()
{
}

uint32_t ETC1::GetBaseInternalFormat(Format format, ColorDepth depth)
{
	return KTXTOOL_GL_RGB;
}

uint32_t ETC1::GetInternalFormat(Format format, ColorDepth depth)
{
	return KTXTOOL_GL_ETC1_RGB8_OES;
}

uint32_t ETC1::Compress(void* in, void* out, int w, int h, Format format, ColorDepth depth)
{
	pack_etc1_block_init();

	//only 8bit allowed
	if (depth != COLOR_DEPTH_8BIT)
	{
		cerr << "ETC1 Only 8bit per channle supported." << endl;
		return 0;
	}

	etc1_pack_params params;

	switch (m_quality)
	{
	case QUALITY_DRAFT:
	case QUALITY_LOW:
		params.m_quality = cLowQuality;
		break;
	case QUALITY_MEDIUM:
		params.m_quality = cMediumQuality;
		break;
	case QUALITY_HIGH:
	default:
		params.m_quality = cHighQuality;
		break;
	}


	m_quality = QUALITY_DRAFT;

	params.m_dithering = true;

	int offset = 0;

	uint32_t block[16];


	int c = format == FORMAT_RGBA ? 4 : 3;

	auto At = [](int w, int h, int x, int y) -> int
	{
		return ((w * h) - ((y * w) + (w - x)));
	};



	for (int y = 0; y < h; y += 4)
	{
		for (int x = 0; x < w; x += 4)
		{
			for (int ix = 0; ix < 4; ix++)
			{
				for (int iy = 0; iy < 4; iy++)
				{
					int i1 = At(4, 4, ix, 3 - iy);
					int i2 = At(w, h, x + ix, y + iy);

					uint8_t* p1 = (uint8_t*)&block[i1];
					uint8_t* p2 = &((uint8_t*)in)[i2 * 4];

					memcpy(p1, p2, c);

					p1[3] = 255;
				}
			}

			pack_etc1_block(((char*)out) + offset, block, params);

			offset += 8;
		}
	}

	assert((uint32_t)offset == GetSize(w, h));

	return offset;
}

uint32_t ETC1::GetSize(int w, int h)
{
	//TODO check for non-power of two dimensions
	
	auto Count = [&](int dim) -> int
	{
		return dim <= 4 ? 1 : dim / 4;
	};

	int blockW = Count(w);
	int blockH = Count(h);

	int totalBlocks = blockW * blockH;

	//ETC1 stores each 4x4 pixel block as 8 bytes
	return 8 * totalBlocks;
}




