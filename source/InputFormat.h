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

#ifndef __KTXTOOL_INPUTFORMAT_INCLUDED
#define __KTXTOOL_INPUTFORMAT_INCLUDED





class PixelData;





/** Input format abstract class */
class InputFormat 
{
public:
	
	virtual ~InputFormat() {}
	

	/** Checks if a given filename belongs to this format. Since a format 
	 *  could have several extensions this method allows the program to 
	 *  check to either one or multiple extensions on a per format basis.
	 *
	 *  return If the filename belongs to this format */
	virtual bool CheckExtension(const char* fileName) const = 0;





	/** Creates a new pixel data object from the file path provided.
	 *
	 *  return NULL if failed */
	virtual PixelData* CreatePixelData(const char* filePath) = 0;





	/** Name of the format ie, EIF - Example Image Format. */
	virtual const char* GetName() const = 0;


};













#endif
