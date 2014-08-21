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

#ifndef __KTXTOOL_TYPES_INCLUDED
#define __KTXTOOL_TYPES_INCLUDED






#define KTXTOOL_GL_RGB 6407
#define KTXTOOL_GL_RGBA 6408

#define KTXTOOL_GL_RGB8 32849
#define KTXTOOL_GL_RGBA8 32856

#define KTXTOOL_GL_UNSIGNED_BYTE 5121
#define KTXTOOL_GL_UNSIGNED_SHORT 5123
#define KTXTOOL_GL_UNSIGNED_INT 5125



enum Format
{
	FORMAT_RGB,
	FORMAT_RGBA
};

enum ColorDepth
{
	COLOR_DEPTH_8BIT,
	COLOR_DEPTH_16BIT,
	COLOR_DEPTH_24BIT,
	COLOR_DEPTH_32bit
};








#endif
