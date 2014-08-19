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


#include "TIFFInputFormat.h"
#include <tiffio.h>
#include <string.h>
#include <ktxtool.h>
#include <iostream>


using std::string;



static int RegisterTIFF()
{
	AddInputFormat(new TIFFInputFormat);

//	TIFF* tif = TIFFOpen("foo.tif", "r");


	return 0;
}

int ktxtoolTIFF = RegisterTIFF();


bool TIFFInputFormat::CheckExtension(const char* ext) const
{
	if (strcmp(ext, "tif") == 0 || strcmp(ext, "tiff") == 0) return true;

	return false;
}

PixelData* TIFFInputFormat::CreatePixelData(const char* filePath)
{
	return NULL;
}
