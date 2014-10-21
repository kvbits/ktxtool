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
#include "Compression/Compression.h"





#define KTX_ENDIANNESS_NUMBER 0x04030201
#define KTX_IDENTIFIER "\xABKTX 11»\r\n\x1A\n"





using namespace std;





Container::Container()
{
	m_pCompression = nullptr;
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


	m_mipmaps.resize(1);

	MipmapLevel& mmp = m_mipmaps[0];


	mmp.w = m_header.pixelWidth;
	mmp.h = m_header.pixelHeight;


	mmp.elems.resize(elementCount);

	for (size_t e = 0; e < mmp.elems.size(); e++)
	{
		mmp.elems[e].resize(faceCount);

		for (size_t f = 0; f < mmp.elems[e].size(); f++)
		{
			mmp.elems[e][f].pData = nullptr;
		}

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
	m_header.numberOfMipmapLevels = 1;

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

		m_header.glBaseInternalFormat = m_pCompression->GetBaseInternalFormat(format, depth);
		m_header.glInternalFormat = m_pCompression->GetInternalFormat(format, depth);
	}
}

void Container::SetData(int elementIndex, int faceIndex, PixelData* pData)
{
	assert((size_t)elementIndex < m_mipmaps[0].elems.size()); 

	assert((size_t)faceIndex < m_mipmaps[0].elems[elementIndex].size());

	Face& face = m_mipmaps[0].elems[elementIndex][faceIndex];

	assert(m_comp == pData->GetComponentCount());

	face.pData = new uint8_t[pData->GetPixelCount() * m_comp];


	for (size_t i = 0; i < (size_t)pData->GetPixelCount() * m_comp; i++)
	{
		((uint8_t*)face.pData)[i] = (pData->Get(i) * 255.f);
	}

}

void Container::GenerateMipmaps()
{
	auto DumpMipmap = [this](MipmapLevel& mmp)
	{
		string fileOut = "./mipmap.";
		fileOut += to_string(mmp.w);
		fileOut += "x";
		fileOut += to_string(mmp.h);
		fileOut += ".ppm";

		WriteFaceToPPM(mmp, 0, 0, fileOut.c_str());
	};


	bool dumpMipmaps = GetOption('d')->IsDefined();

	assert(m_mipmaps.size() == 1);

	if (!IsSqrPowerOf2())
	{
		cout << "KTX Container: Unable to generate mipmaps (non square power of 2)" << endl;
		return;
	}

	m_header.numberOfMipmapLevels = 1 + floor(log10((float)m_header.pixelWidth) / log10(2.0f));

	//resize the mipmap array
	m_mipmaps.resize(m_header.numberOfMipmapLevels);

	int refW = m_mipmaps[0].w;
	int refH = m_mipmaps[0].h;

	//make sure that the reference mipmap has valid face
	assert(m_mipmaps[0].elems.size() == 1);
	assert(m_mipmaps[0].elems[0].size() >= 1);

	Face& refFace = m_mipmaps[0].elems[0][0];
		
	assert(refFace.pData != nullptr);
	


	for (size_t m = 0; m < m_mipmaps.size(); m++)
	{
		MipmapLevel& mmp = m_mipmaps[m];

		if (m > 0)
		{
			MipmapLevel& upmmp = m_mipmaps[m - 1];

			
			mmp.w = (refW >> m);
			mmp.h = (refH >> m);

			mmp.elems.resize(upmmp.elems.size());

			for (size_t e = 0; e < mmp.elems.size(); e++)
			{
				mmp.elems[e].resize(upmmp.elems[e].size());

				for (size_t f = 0; f < mmp.elems[e].size(); f++)
				{
					mmp.elems[e][f].pData = Downsample(upmmp.elems[e][f].pData, upmmp.w, upmmp.h);
				}
				
			}
		}

		if (dumpMipmaps) DumpMipmap(mmp);

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


void Container::WriteFaceToPPM(MipmapLevel& mmp, int elemIndex, int faceIndex, const char* filePath)
{
	ofstream ppm(filePath);	

	size_t w = mmp.w;
	size_t h = mmp.h;

	assert((size_t)elemIndex < mmp.elems.size());

	assert((size_t)faceIndex < mmp.elems[elemIndex].size());

	uint8_t* pixels = (uint8_t*)mmp.elems[elemIndex][faceIndex].pData;

	ppm << "P3" << endl;
	ppm << (int)w << " " << (int)h << endl;
	ppm << 255 << endl;


	for (size_t y = (h-1); y < h; y--)
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

	cout << "Face writen to " << filePath << endl;
}

bool Container::Write(const char* filePath) const
{	

	ofstream file(filePath, ofstream::out | ofstream::trunc | ofstream::binary);
	
	if (!file.is_open())
	{
		cout << "Couldn't open '" << filePath << "' for writing" << endl;
		file.close();
		return false;
	}

	file.write((const char*)&m_header, sizeof(Header));

	assert(m_header.numberOfMipmapLevels != 0);



	//dummy 4byte value for padding
	uint32_t dummy = 0;


	//buffer to hold down compressed data
	char* pBuffer = nullptr;
	
	//only allocate it if there's a compression defined
	if (m_pCompression)
	{
		//allocating just the largest mipmap would be enough for the smallest one
		pBuffer = (char*)malloc(m_pCompression->GetSize((int)m_header.pixelWidth, (int)m_header.pixelHeight));
	}


	if (m_pCompression)
	{
		cout << "Compressing with " << m_pCompression->GetName() << endl;
	}


	for (size_t m = 0; m < m_mipmaps.size(); m++)
	{
		const MipmapLevel& mmp = m_mipmaps[m];
	
		uint32_t imgSize = (mmp.w * mmp.h) * m_comp;


		//if compressed set the fixed size 
		if (m_pCompression)
		{
			imgSize = m_pCompression->GetSize(mmp.w, mmp.h);
		}


		file.write((const char*)&imgSize, sizeof(uint32_t));

		for (size_t e = 0; e < mmp.elems.size(); e++)
		{
			for (size_t f = 0; f < mmp.elems[e].size(); f++)
			{

				const Face& face = mmp.elems[e][f];

				const char* pData = (char*)face.pData;
				size_t size = imgSize;
				
				//if ha compression then write the compressed data instead
				if (m_pCompression)
				{
					pData = pBuffer;
					size = m_pCompression->Compress(face.pData, pBuffer, mmp.w, mmp.h, m_format, m_depth);

					assert(imgSize == size);
				}

				file.write(pData, size);
			}
		}

		int mipmapPadding = 3 - ((imgSize + 3) % 4);

		file.write((const char*)&dummy, mipmapPadding);
	}

	file.close();

	if (pBuffer)
	{
		assert(m_pCompression != nullptr);
		free(pBuffer);
	}

	
	cout << "Writing ktx container to " << filePath << endl;

	return true;
}

bool Container::Read(const char* filePath) const
{
	return false;
}




