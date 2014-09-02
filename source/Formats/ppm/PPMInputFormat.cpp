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


#include "PPMInputFormat.h"
#include <string.h>
#include <ktxtool.h>
#include <iostream>
#include <PixelData.h>
#include <fstream>
#include <vector>






using namespace std;







static int RegisterPPM()
{
	AddInputFormat(new PPMInputFormat);

	return 0;
}

int ktxtoolPPM = RegisterPPM();


bool PPMInputFormat::CheckExtension(const char* ext) const
{
	if (strcmp(ext, "ppm") == 0) return true;

	return false;
}

PixelData* PPMInputFormat::CreatePixelData(const char* filePath)
{
	PixelData* pData = nullptr;

	int w = -1;
	int h = -1;
	float maxval = -1.f;

	ifstream ppm(filePath);

	//reads a line ignoring comments
	auto ReadLine = [&]() -> string
	{
		string line;

		while (!ppm.eof())
		{
			getline(ppm, line);

			if (line.size() && line[0] != '#')
			{
				return line;
			}
		}

		assert(false);
		return line;
	};

	if (ReadLine() != "P3")
	{
		cerr << "Invalid PPM format, failed to read the magick string" << endl;
		ppm.close();
		return NULL;
	}

	//read the dimmesion
	string dimm = ReadLine();

	size_t offset = dimm.find_first_of(' ');

	w = stoi(dimm.substr(0, offset));
	h = stoi(dimm.substr(offset + 1, -1));

	if (w <= 0 && h <= 0)
	{
		cerr << "Invalid PPM format, dimmension is invalid" << endl;
		return NULL;
	}


	//read the maximum color value
	maxval = stof(ReadLine());

	if (maxval > 65536.0 || maxval <= 0.f)
	{
		cerr << "Invalid PPM format, the maximuym color value is invalid" << endl;
		return NULL;
	}
	
	//read pixel data
	pData = new PixelData(w, h, FORMAT_RGB);

	int i = 0;

	//fixes the row order to match the standards
	auto GetIndex = [&]() -> int
	{
		//Removed, suspected as a bug	
		/*int x = i % w;
		int y = (i - x) / w;
		return ((w * h) - ((y * w) + (w - x))) * 3;*/

		return i * 3;
	};
	
	while (i < pData->GetPixelCount())
	{
		if (ppm.eof()) break;

		float* pixel = &pData->Get(GetIndex());

		//Read a pixel component and normalizes it
		auto ReadComp = [&]() -> float
		{
			float comp;

			ppm >> comp;

			return comp / maxval;
		};

		pixel[0] = ReadComp();
		pixel[1] = ReadComp();
		pixel[2] = ReadComp();
				
		i++;
	}

	assert(i == pData->GetPixelCount());
	if (i != pData->GetPixelCount())
	{
		cerr << "Invalid PPM format, unexpected end of file while reading pixels" << endl;
		delete pData;
		return NULL;
	}

	return pData;
}
