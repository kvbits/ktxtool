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

#ifdef KTXTOOL_TBB

#include <tbb/blocked_range2d.h>
#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>

using namespace tbb;

#endif




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


#define At(w, h, x, y) ((w * h) - (((y) * w) + (w - (x))))


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


	/*m_quality = QUALITY_DRAFT;

	params.m_dithering = true;*/


	int c = format == FORMAT_RGBA ? 4 : 3;

	int bw = w / 4;
	int bh = h / 4;


#ifdef KTXTOOL_TBB
	
	
	class EncodeBlocks
	{
		uint8_t* in;
		uint8_t* out;
		int c;
		int w;
		int h;
		int bw;
		int bh;
		etc1_pack_params* params;

	public:	


		void operator()(const blocked_range2d<int>& r) const
		{
			


			for (int by = r.rows().begin(); by != r.rows().end(); by++)
			{
				for (int bx = r.cols().begin(); bx != r.cols().end(); bx++) 
				{
					int x = bx * 4;
					int y = by * 4;
					
									
					int offset = At(bw, bh, bx, (bh-1) - by) * 8;


					uint32_t block[16];



					for (int ix = 0; ix < 4; ix++)
					{
						for (int iy = 0; iy < 4; iy++)
						{
							int i1 = At(4, 4, ix, 3 - iy);
							int i2 = At(w, h, x + ix, y + iy);

							uint8_t* p1 = (uint8_t*)&block[i1];
							uint8_t* p2 = &((uint8_t*)in)[i2 * c];

							memcpy(p1, p2, c);

							p1[3] = 255;
						}
					}

					pack_etc1_block(((char*)out) + offset, block, *params);

				}
        	}
		}

		EncodeBlocks(void* _in, void* _out, int _bw, int _bh, int _w, int _h, int _c, etc1_pack_params* _params)
		{
			in = (uint8_t*)_in;
			out = (uint8_t*)_out;
			c = _c;
			w = _w;
			h = _h;
			bw = _bw;
			bh = _bh;
			params = _params;
		}

	};


	parallel_for(blocked_range2d<int>(0, bw, 1, 0, bh, 1), EncodeBlocks(in, out, bw, bh, w, h, c, &params));


#else

	uint32_t block[16];


	int size = (int)GetSize(w, h);


	for (int by = 0; by < bh; by++)
	{
		for (int bx = 0; bx < bw; bx++)
		{
			int x = bx * 4;
			int y = by * 4;
			
			
			int offset = At(bw, bh, bx, (bh-1) - by) * 8;


			for (int ix = 0; ix < 4; ix++)
			{
				for (int iy = 0; iy < 4; iy++)
				{
					int i1 = At(4, 4, ix, 3 - iy);
					int i2 = At(w, h, x + ix, y + iy);

					uint8_t* p1 = (uint8_t*)&block[i1];
					uint8_t* p2 = &((uint8_t*)in)[i2 * c];

					memcpy(p1, p2, c);

					p1[3] = 255;
				}
			}


			pack_etc1_block(((char*)out) + offset, block, params);

		}
	}

#endif

	//assert((uint32_t)offset == GetSize(w, h));

	return GetSize(w, h);
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




