/*
 *	This file is a part of Yet Another Fractal Explorer package.
 *	Copyright (c) 2003-2011 Alex Pankratov. All rights reserved.
 *
 *	http://swapped.cc/yafe
 */

/*
 *	The program is distributed under terms of BSD license. 
 *	You can obtain the copy of the license by visiting:
 *
 *	http://www.opensource.org/licenses/bsd-license.php
 */

/*
 *	Note that this is an old code dating back to 2003. I do not 
 *	write like this anymore nor do I indent with 2 spaces now :)
 */

#ifndef _FRACT_PALETTE_H_
#define _FRACT_PALETTE_H_

#include "stdafx.h"

//
#define HEXRGB(rgb) \
	(((rgb) >> 16) & 0xff)/255., \
	(((rgb) >>  8) & 0xff)/255., \
	(((rgb) >>  0) & 0xff)/255.

//
struct CPalette
{
  virtual COLORREF Color(_double v) = 0;

  virtual void Add(_double v, _double x, _double y, _double z) = 0;

  virtual uint Id() = 0;

  virtual ~CPalette() {}

protected:

  CPalette() {}
  CPalette(const CPalette &);
  void operator = (const CPalette &);
};

//
CPalette * PaletteSingle(COLORREF col);
CPalette * PaletteLinear(bool hsv);

//
CPalette * PaletteLinear(COLORREF col);
CPalette * PaletteLinear(COLORREF col1, COLORREF col2);
CPalette * PaletteFromMapFile(const char * filename);

CPalette * PaletteLoad(const char * filename);

#endif
