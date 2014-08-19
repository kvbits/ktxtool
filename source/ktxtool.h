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

#ifndef __KTXTOOL_INCLUDED
#define __KTXTOOL_INCLUDED




#include <string>

using std::string;




enum OptionFlags
{
	OPTION_DEFINED = 0x01,
	OPTION_EXPECTS_VALUE = 0x02,
	OPTION_REQUIRED = 0x08
};


struct Option
{
	char   id;
	int    flags;
	string value;
	string desc;

	Option()
	{
		id = char(0);
		flags = 0;
	}

	inline bool IsDefined() const
	{
		return (flags & OPTION_DEFINED) != 0;
	}

	inline bool ExpectsValue() const
	{
		return (flags & OPTION_EXPECTS_VALUE) != 0;
	}

	inline bool IsRequired() const
	{
		return (flags & OPTION_REQUIRED) != 0;
	}

	inline void MarkAsDefined()
	{
		flags |= OPTION_DEFINED; 
	}
};




/** Gets a proccesed option from it's CHAR id, ie. -f 
 *  Returns NULL if not found (should be asserted) */
Option* GetOption(char id);


/** Adds an option to be processed, this should be called ealry 
 *  in the main function */
Option& AddOption(char id, int flags, const char* desc);



/** Adds an input format to convert from into pixel data */
void AddInputFormat(InputFormat* pFormat);






#endif
