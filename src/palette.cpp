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

#include "stdafx.h"
#include "palette.h"
#include "math.h"


/*
 *	CPaletteSingle
 */
struct CPaletteSingle : CPalette
{
  CPaletteSingle(COLORREF _col) { col = _col; }

  COLORREF Color(_double v) { return col; }

  void Add(_double v, _double x, _double y, _double z) { }

  uint Id() { return col; }

  COLORREF col;
};

/*
 *	CPaletteLinear
 */
struct triplet
{
  _double  a,b,c;
};

//
#define COLORREF_TO_TRIPLET(c) { GetRValue(c)/255.,  \
                                 GetGValue(c)/255.,  \
                                 GetBValue(c)/255. } 

//
#define HASH(v) ( srand(RAND_MAX*v + rand()), rand() )

//
typedef multimap<_double,triplet>  color_map;

//
struct CPaletteLinear : CPalette
{
  CPaletteLinear() { hsv = false; }

  COLORREF Color(_double v);

  void Add(_double v, _double x, _double y, _double z);

  uint Id();

  //
  COLORREF FromHSV(_double h, _double s, _double v);
  COLORREF FromRGB(_double r, _double g, _double b);

  bool       hsv;
  color_map  items;  
};

//
COLORREF CPaletteLinear::Color(_double v)
{
  color_map::iterator it;
  triplet q;

  assert(items.size());

  //
  it = (items.size() > 1) ? items.lower_bound(v) : items.begin();

  if (it == items.begin())
  {
    q = items.begin()->second;
  }
  else
  if (it == items.end())
  {
    q = it->second;
  }
  else
  {
    _double    v1   = it->first;
    triplet & more = it->second; it--;
    _double    v0   = it->first;
    triplet & less = it->second;
  
    _double f = (v - v0)/(v1 - v0);
    q.a = less.a + f * (more.a - less.a);
    q.b = less.b + f * (more.b - less.b);
    q.c = less.c + f * (more.c - less.c);
  }

  return hsv ? FromHSV(q.a,q.b,q.c) : FromRGB(q.a,q.b,q.c);
}

void CPaletteLinear::Add(_double v, _double x, _double y, _double z)
{
  triplet q = { x,y,z };

  assert( 0.0<=v && v<=1.0 );
  assert( 0.0<=x && x<=1.0 );
  assert( 0.0<=y && y<=1.0 );
  assert( 0.0<=z && z<=1.0 );

  items.insert( color_map::value_type(v,q) );
}

uint CPaletteLinear::Id()
{
  color_map::iterator it, ite;
  uint val = 0x3456;

  srand(0x789A);

  for (it=items.begin(), ite=items.end(); it!=ite; it++)
    val = HASH(it->first) ^ 
          HASH(it->second.a) ^ 
          HASH(it->second.b) ^
          HASH(it->second.c);

  return val;
}

//
COLORREF CPaletteLinear::FromHSV(_double h, _double s, _double v)
{
	uint   n;
  _double f;
  _double vs, vsf;

	if (s == 0) // gray
		return FromRGB(v,v,v);

	h  *= 6;
  n   = floor(h);
  f   = h - n;
  vs  = v * s;
  vsf = vs * f;

  switch (n)
  {
  case 0:  return FromRGB(v,       v-vs+vsf, v-vs);
  case 1:  return FromRGB(v-vsf,   v,        v-vs);
  case 2:  return FromRGB(v-vs,    v,        v-vs+vsf);
  case 3:  return FromRGB(v-vs,    v-vsf,    v);
  case 4:  return FromRGB(v-vs+vsf,v-vs,     v);
  default: return FromRGB(v,       v-vs,     v-vsf);
  }
}

//
COLORREF CPaletteLinear::FromRGB(_double r, _double g, _double b)
{
  return RGB(0xff*r,0xff*g,0xff*b);
}


//
//
CPalette * PaletteSingle(COLORREF col)
{
	return new CPaletteSingle(col);
}

CPalette * PaletteLinear(bool hsv)
{
  CPaletteLinear * p = new CPaletteLinear();
  p->hsv = hsv;
  return p;
}

CPalette * PaletteLinear(COLORREF c)
{
  CPalette * p = PaletteLinear(false);

  p->Add(0.5, GetRValue(c)/255., GetGValue(c)/255., GetBValue(c)/255.);

  return p;
}

CPalette * PaletteSimple(COLORREF c1, COLORREF c2)
{
  CPalette * p = PaletteLinear(false);

  p->Add(0.0, GetRValue(c1)/255., GetGValue(c1)/255., GetBValue(c1)/255.);
  p->Add(1.0, GetRValue(c2)/255., GetGValue(c2)/255., GetBValue(c2)/255.);
  
  return p;
}

/*
CPalette * PaletteDefault()
{
  CPaletteLinear * p = new CPaletteLinear();
  triplet q;

  p->hsv = false;
  q.v = 0.20; q.a = 16/255.;  q.b = 144/255.; q.c = 206/255.; p->items.push_back(q);
  q.v = 0.80; q.a = 32/255.; q.b = 62/255.; q.c = 78/255.; p->items.push_back(q);

/*
  p->hsv = false;
  q.v = 0.00; q.a = q.b = q.c = 0.3; p->items.push_back(q);
  q.v = 0.50; q.a = q.b = q.c = 0.9; p->items.push_back(q);
  q.v = 1.00; q.a = q.b = q.c = 0.3; p->items.push_back(q);

/*
  p->hsv = false;
  q.v = 0.00; q.a = 1.0; q.b = 0.0; q.c = 0.0;  p->items.push_back(q);
  q.v = 0.16; q.a = 1.0; q.b = 1.0; q.c = 0.0;  p->items.push_back(q);
  q.v = 0.33; q.a = 0.0; q.b = 1.0; q.c = 0.0;  p->items.push_back(q);
  q.v = 0.48; q.a = 0.0; q.b = 1.0; q.c = 1.0;  p->items.push_back(q);
  q.v = 0.66; q.a = 0.0; q.b = 0.0; q.c = 1.0;  p->items.push_back(q);
  q.v = 0.84; q.a = 1.0; q.b = 0.0; q.c = 1.0;  p->items.push_back(q);
  q.v = 1.00; q.a = 1.0; q.b = 0.0; q.c = 0.0;  p->items.push_back(q);
/*

  p->hsv = true;
  q.b = 1.0;
  q.c = 1.0;

  q.v = q.a = 0.00; p->items.push_back(q);
  q.v = q.a = 0.16; p->items.push_back(q);
  q.v = q.a = 0.33; p->items.push_back(q);
  q.v = q.a = 0.48; p->items.push_back(q);
  q.v = q.a = 0.66; p->items.push_back(q);
  q.v = q.a = 0.84; p->items.push_back(q);
  q.v = q.a = 1.00; p->items.push_back(q);

  return p;
}
*/

CPalette * PaletteFromMapFile(const char * filename)
{
  FILE * fh = fopen(filename, "rt");

  if (! fh)
    return 0;

  CPalette * p = PaletteLinear(false);
  uint i;

  for (i=0; i<257 && !feof(fh); i++)
  {
    char line[256];
    uint r,g,b;

    fgets(line, sizeof(line)-1, fh);
    if (sscanf(line, "%u %u %u", &r,&g,&b) != 3)
    {
      delete p; p=0;
      break;
    }

    p->Add(i/256., r/255., g/255., b/255.);
  }

  fclose(fh);
  return p;
}

/*
CPalette * PaletteLoad(const char * filename)
{
  FILE * fh = fopen(filename, "rt");
  char   line[80];

  while (! feof(fh))
  {
    char * p;
    
    if (! fgets(line, sizeof(line)-1, fh))
      break;

    p = line;
    while (*p && strchr(" \t\r\n", *p)) *p++; // skip whitespace

    $todo
  }
}
*/