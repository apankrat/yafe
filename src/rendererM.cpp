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
#include "renderer.h"
#include <math.h>
#include <stdio.h>

struct CFractalMandelbrot : CFractal
{
  CFractalMandelbrot();

  EFractal Id()                         { return eFractalMandelbrot; }
  
  CBounds  GetBounds()                  { return bounds; }
  void     SetBounds(const CBounds & b) { bounds = b; }

  EQuality GetQuality()                 { return quality; }
  void     SetQuality(EQuality q)       { quality = q; }

  CPalette * DefaultPalette(bool in);

  void Render(CRenderCallback * cb);

  //
  CBounds   bounds;
  EQuality  quality;
};

//
// Calculator
//
CFractalMandelbrot::CFractalMandelbrot()
{
  quality = eQualityNormal;
  bounds  = CBounds(-2.0, 1.0, -1.5, 1.5);
}

CPalette * CFractalMandelbrot::DefaultPalette(bool in)
{
  CPalette * p = PaletteLinear(false);

  if (in)
  {
    p->Add(0.5, 60/255.,  60/255., 60/255.);
  }
  else
  {
    p->Add(0.0,      60/255., 60/255.,  60/255.);
    p->Add(32/255.,     1.0,     0.0,      0.0);
    p->Add(64/255.,     1.0,     1.0,      0.0);
    p->Add(96/255.,     1.0,     1.0,      1.0);
    p->Add(160/255.,    1.0, 120/255.,     0.0);
    p->Add(192/255.,    1.0,     0.0,      0.0);
    p->Add(1.0,      60/255., 60/255.,  60/255.);
  }

  return p;
}

void CFractalMandelbrot::Render(CRenderCallback * cb)
{
  assert(cb);

  uint   w = cb->RcbWidth(),
         h = cb->RcbHeight();

  _double da = (bounds.x2-bounds.x1) / (w-1);
  _double db = (bounds.y2-bounds.y1) / (h-1);
  _double a, b;
  uint   i, y, n_main;

  bool   full = ! cb->RcbByLine();

  PixelType * data = 0,
            * line = 0;

  //
  assert(w>1 && h>1);

  //
  data = line = new PixelType [full ? w*h : w];
  if (! data)
    return;

  //
  switch (quality)
  {
  case eQualityDraft:  n_main = 128; break;
  case eQualityNormal: n_main = 1024; break;
  case eQualityHigh:   n_main = 4096; break;
  case eQualityUltra:  n_main = 8192; break;
  default:
    assert(0);
    return;
  }

  //
  // do 'bottom-up' to accomodate BMP's line ordering
  //
  if (full)
    line += w*h;

  for (y=h, b=bounds.y2; y>0; y--, b-=db)
  {
    uint x;

    if (full)
      line -= w;

    for (x=0, a=bounds.x1; x<w; x++, a+=da)
    {
      _double zx = 0;
      _double zy = 0;

      line[x] = PixelInside;

      for (i=0; i<n_main; i++)
      {
        _double zx2 = zx*zx;
        _double zy2 = zy*zy;
        
        if (zx2 + zy2 >= 4.0)
        {
          _double f;
          
          if (i>1) i--;
          f = PixelMax * (i % 256)/255.;
          line[x] = PixelMax & (PixelType)f;
          break;
        }

        zy = 2*zx*zy + b;
        zx = zx2 - zy2 + a;
      }      
    }

    if (! cb->RcbLine(w,h,h-y,line))
      break;
  }

  //
  if (full && y == 0)
  {
    cb->RcbDone(w,h,data);
    return;
  }
  
  delete [] data;
}

CFractal * InstantiateMandelbrot()
{
  return new CFractalMandelbrot();
}