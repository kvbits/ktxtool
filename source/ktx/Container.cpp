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

	switch (format)
	{
	case FORMAT_RGB:
		{
			m_header.glFormat = KTXTOOL_GL_RGB;
			m_header.glInternalFormat = KTXTOOL_GL_RGB8;
		}
		break;
	case FORMAT_RGBA:
		{
			m_header.glFormat = KTXTOOL_GL_RGBA;
			m_header.glInternalFormat = KTXTOOL_GL_RGBA8;
		}
		break;
	}

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

	const int compCount = m_format == FORMAT_RGBA ? 4 : 3;


	face.pData = new uint8_t[pData->GetPixelCount() * compCount];

	size_t i2 = 0;

	for (size_t i = 0; i < pData->GetPixelCount(); i++)
	{
		for (int c = 0; c < compCount; c++)
		{
			((uint8_t*)face.pData)[i + c] = (uint8_t)(pData->GetRaw(i)[c] * 255.f);

			i2++;
		}
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

	cout << m_header.numberOfMipmapLevels << endl;

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
				mmp.faces[f].pData = Downsample(upmmp.faces[0].pData, upmmp.w, upmmp.h);


				cout << mmp.w << " - " << mmp.h << endl;

			}
			
			
		}
	}
}


void* Container::Downsample(void* pData, int w, int h)
{

	void* pDataOut = new uint8_t[w * h];

	uint8_t* pixels = (uint8_t*)pData;

	string fileOut = "./mipmap.";
	fileOut += to_string((w / 2));
	fileOut += "x";
	fileOut += to_string((h / 2));
	fileOut += ".ppm";

	ofstream ppm(fileOut);	

	ppm << "P3" << endl;
	ppm << (w / 2) << " " << (h / 2) << endl;
	ppm << 255 << endl;


	size_t i2 = 0;

	for (size_t y = 0; y < h; y += 2)
	{
		for (size_t x = 0; x < w; x += 2) 
		{
			uint8_t* pixel = &pixels[(w * h)-((y * w) + (w - x))];
			
			ppm << (int)pixel[0] << " ";
			ppm << (int)pixel[1] << " ";
			ppm << (int)pixel[2] << "	";
			
			for (size_t c = 0; c < 4; c++)
			{
				((uint8_t*)pDataOut)[i2] = pixel[c];
				i2++;
			}
		}

		ppm << endl;
	}

	ppm.close();

	return pDataOut;
}

bool Container::Write(const char* filePath) const
{
	return false;
}

bool Container::Read(const char* filePath) const
{
	return false;
}




