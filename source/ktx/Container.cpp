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




#define KTX_ENDIANNESS_NUMBER 0x04030201
#define KTX_IDENTIFIER "«KTX 11»\r\n\x1A\n"




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

void Container::SetData(int elementIndex, int faceIndex, PixelData* pData)
{
	assert((size_t)elementIndex < m_elements.size());

	MipmapArray& mmps = m_elements[elementIndex]; 

	assert((size_t)faceIndex < mmps[0].faces.size());

	Face& face = mmps[0].faces[faceIndex];

	face.pData = NULL;	
}





