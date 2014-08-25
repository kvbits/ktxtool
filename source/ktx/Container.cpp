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

#include "Container.h"
#include <ktxtool.h>
#include <assert.h>
#include <Types.h>
#include <PixelData.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>




#define KTX_ENDIANNESS_NUMBER 0x04030201
#define KTX_IDENTIFIER "«KTX 11»\r\n\x1A\n"





using namespace std;





Container::Container()
{
	m_pCompression = NULL;
}

void Container::Init(int w, int h, int elementCount, int faceCount)
{
	//set the ma
	memcpy(m_header.identifier, KTX_IDENTIFIER, 12);

	assert(HasValidIdentifier() == true);

	//Set the endianess number
	m_header.endianness = KTX_ENDIANNESS_NUMBER;

	m_header.pixelWidth = w;
	m_header.pixelHeight = h;

	m_header.pixelDepth = 0; //only 2d/cube textures supported

	m_header.numberOfArrayElements = elementCount;
	m_header.numberOfFaces = faceCount;
	m_header.numberOfMipmapLevels = 0;
	m_header.bytesOfKeyValueData = 0;

	m_elements.resize(elementCount);

	for (size_t i = 0; i < m_elements.size(); i++)
	{
		MipmapArray& mmps = m_elements[i];
		
		mmps.resize(1);

		mmps[0].faces.resize(faceCount);

		for (size_t f = 0; f < mmps[0].faces.size(); f++)
		{
			mmps[0].faces[f].pData = NULL;
		}

		mmps[0].w = m_header.pixelWidth;
		mmps[0].h = m_header.pixelHeight;
	}
}

bool Container::HasValidIdentifier() const
{
	return (memcmp(m_header.identifier, KTX_IDENTIFIER, 12) == 0);
}

void Container::SetFormat(Format format, ColorDepth depth, Compression* pComp)
{
	m_pCompression = pComp;
	m_format = format;
	m_depth = depth;
	
	assert(depth == COLOR_DEPTH_8BIT && "non 8bit color depth not supported (yet)");

	m_header.glType = KTXTOOL_GL_UNSIGNED_BYTE; //hardcoded for now
	m_header.glTypeSize = sizeof(uint8_t); //hardcoded aswell TODO!!

	m_comp = 0;

	switch (format)
	{
	case FORMAT_RGB:
		{
			m_header.glFormat = KTXTOOL_GL_RGB;
			m_header.glInternalFormat = KTXTOOL_GL_RGB8;
			m_comp = 3;
		}
		break;
	case FORMAT_RGBA:
		{
			m_header.glFormat = KTXTOOL_GL_RGBA;
			m_header.glInternalFormat = KTXTOOL_GL_RGBA8;
			m_comp = 4;
		}
		break;
	}

	assert(m_comp > 0);

	m_header.glBaseInternalFormat = m_header.glFormat;


	//check for a compression format, if so override the variables
	if (m_pCompression)
	{
		m_header.glType = 0;
		m_header.glTypeSize = 1;
		m_header.glFormat = 0;
	}
}

void Container::SetData(int elementIndex, int faceIndex, PixelData* pData, bool generateMipmaps)
{
	assert((size_t)elementIndex < m_elements.size());

	MipmapArray& mmps = m_elements[elementIndex]; 

	assert((size_t)faceIndex < mmps[0].faces.size());

	Face& face = mmps[0].faces[faceIndex];

	assert(m_comp == pData->GetComponentCount());

	face.pData = new uint8_t[pData->GetPixelCount() * m_comp];


	for (size_t i = 0; i < pData->GetPixelCount() * m_comp; i++)
	{
		((uint8_t*)face.pData)[i] = (pData->Get(i) * 255.f);
	}

}

void Container::GenerateMipmaps()
{

	if (!IsSqrPowerOf2())
	{
		cout << "KTX Container: Unable to generate mipmaps (non square power of 2)" << endl;
		return;
	}

	m_header.numberOfMipmapLevels = 1 + floor(log10((float)m_header.pixelWidth) / log10(2.0f));

	for (size_t e = 0; e < m_elements.size(); e++)
	{
		//make sure this only has the original mipmal
		assert(m_elements[e].size() == 1);

		int refW = m_elements[e][0].w;
		int refH = m_elements[e][0].h;
		Face& refFace = m_elements[e][0].faces[0];
			
		assert(refFace.pData != NULL);

		m_elements[e].resize(m_header.numberOfMipmapLevels);

		for (size_t m = 1; m < m_elements[e].size(); m++)
		{
			MipmapLevel& upmmp = m_elements[e][m - 1];

			MipmapLevel& mmp = m_elements[e][m];
			
			mmp.w = (refW >> m);
			mmp.h = (refH >> m);

			mmp.faces.resize(upmmp.faces.size());

			for (size_t f = 0; f < mmp.faces.size(); f++)
			{
				mmp.faces[f].pData = Downsample(upmmp.faces[f].pData, upmmp.w, upmmp.h);
			}
			
			/*string fileOut = "./mipmap.";
			fileOut += to_string(upmmp.w);
			fileOut += "x";
			fileOut += to_string(upmmp.h);
			fileOut += ".ppm";

			WriteFaceToPPM(upmmp, 0, fileOut.c_str());*/

	
		}

		
	}
}


static void GetAvgPx(uint8_t* pixels, uint8_t* out, int comp, int w, int h, int x, int y)
{
	//uint8_t pixel[4];
	//uint8_t* pixel = &pixels[((w * h)-((y * w) + (w - x))) * comp];

	auto GetAvgComp = [&](int c) -> uint8_t
	{
		//counts how many pixels have been added
		int against = 1;

		float avg = 0.0f;

		//return float as the avg is done in a normalized space
		auto At = [&](int ix, int iy)
		{
			int fx = x + ix;
			int fy = y + iy;

			if (fx >= w || fx < 0 || fy >= h || fy < 0)
			{
				return;
			}

			uint8_t* pixel = &pixels[((w * h) - ((fy * w) + (w - fx))) * comp];

			against++;

			avg += pixel[c] / 255.f;
		};

		At(0, 0);

		At(1, 1);  At(-1, -1);
		At(0, 1);  At(0, -1);
		At(1, 0);  At(-1, 0);
		At(1, -1); At(-1, 1);

		return (avg / against) * 255.f;
	};


	//allow maximum 4 components per pixel
	for (int c = 0; c < comp; c++)
	{
		out[c] = GetAvgComp(c);
	}
}

void* Container::Downsample(void* pData, int w, int h)
{
	int w2 = w / 2;
	int h2 = h / 2;

	void* pDataOut = new uint8_t[(w2 * h2) * m_comp];

	uint8_t* pixels = (uint8_t*)pData;


	for (int y = 0; y < h; y += 2)
	{
		for (int x = 0; x < w; x += 2) 
		{
			uint8_t* to = &((uint8_t*)pDataOut)[((w2 * h2)-(((y / 2) * w2) + (w2 - (x / 2)))) * m_comp];  
	
			GetAvgPx(pixels, to, m_comp, w, h, x, y);	
		}

	}

	return pDataOut;
}


void Container::WriteFaceToPPM(MipmapLevel& mmp, int faceIndex, const char* filePath)
{
	ofstream ppm(filePath);	

	size_t w = mmp.w;
	size_t h = mmp.h;

	assert((size_t)faceIndex < mmp.faces.size());

	uint8_t* pixels = (uint8_t*)mmp.faces[faceIndex].pData;

	ppm << "P3" << endl;
	ppm << (int)w << " " << (int)h << endl;
	ppm << 255 << endl;


	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++) 
		{
			uint8_t* pixel = &pixels[((w * h) - ((y * w) + (w - x))) * m_comp];
			
			ppm << (int)pixel[0] << " ";
			ppm << (int)pixel[1] << " ";
			ppm << (int)pixel[2] << "	";
		}

		ppm << endl;
	}

	ppm.close();

	cout << "Face writed to " << filePath << endl;
}

bool Container::Write(const char* filePath) const
{
	return false;
}

bool Container::Read(const char* filePath) const
{
	return false;
}




