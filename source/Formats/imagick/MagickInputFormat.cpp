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


#include "MagickInputFormat.h"
#include <string.h>
#include <ktxtool.h>
#include <iostream>
#include <PixelData.h>
#include <fstream>
#include <wand/MagickWand.h>



using namespace std;




/* Simple exception reporting */
static void DumpWandError(MagickWand* wand)
{
	char* strDesc;

	ExceptionType type;
	strDesc = MagickGetException(wand, &type);

	cerr << "ImageMagick " << strDesc << endl;

	MagickRelinquishMemory(strDesc);
}










static int RegisterMagick()
{
	AddInputFormat(new MagickInputFormat);

	return 0;
}

int ktxtoolMagick = RegisterMagick();


bool MagickInputFormat::CheckExtension(const char* ext) const
{
	//Reads everything TODO add format query
	return true;
}

PixelData* MagickInputFormat::CreatePixelData(const char* filePath)
{
	PixelData* pData = NULL;

	
	MagickWandGenesis();

	MagickWand* wand = NewMagickWand();
	MagickBooleanType status = MagickReadImage(wand, filePath);

	if (status != MagickTrue)
	{
		DumpWandError(wand);
		DestroyMagickWand(wand);
		MagickWandTerminus();
		return NULL;
	}

	int w = MagickGetImageWidth(wand);
	int h = MagickGetImageHeight(wand);

	Format format = MagickGetImageAlphaChannel(wand) == MagickFalse ? FORMAT_RGB : FORMAT_RGBA;
	const char* strFormat = NULL;

	switch (format)
	{
	case FORMAT_RGB:  strFormat = "RGB"; break;
	case FORMAT_RGBA: strFormat = "RGBA"; break;
	};

	assert(strFormat != NULL);


	//Now create the pixel data
	pData = new PixelData(w, h, format);

	status = MagickExportImagePixels(wand, 0, 0, w, h, strFormat,  FloatPixel, pData->GetData());

	if (status != MagickTrue)
	{
		delete pData;

		DumpWandError(wand);
		DestroyMagickWand(wand);
		MagickWandTerminus();
		return NULL;
	}

	DestroyMagickWand(wand);
	MagickWandTerminus();


	return pData;
}
