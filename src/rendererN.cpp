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
#include <complex>

struct CFractalNewton : CFractal
{
  CFractalNewton();

  EFractal Id()                         { return eFractalNewton; }
  
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
#define ABS(v)               (((v) < 0) ? -(v) : (v))
#define ABS_LESS(v, limit)   (ABS(v) < (limit))
#define ABS_MORE(v, limit)   (ABS(v) > (limit))


//
// Calculator
//
CFractalNewton::CFractalNewton()
{
  quality = eQualityNormal;
  bounds  = CBounds(-2.0, 2.0, -2.0, 2.0);
}

CPalette * CFractalNewton::DefaultPalette(bool in)
{
  CPalette * p = PaletteLinear(false);

  if (in)
  {
    p->Add(0.5, 60/255.,  60/255., 60/255.);
  }
  else
  {
/*
    p->Add(0.0,  0.1, 0.1,  0.2);
    p->Add(0.1,  0.5, 0.5,  1.0);
    p->Add(0.2,  0.7, 0.7,  1.0);
    p->Add(0.3,  0.3, 0.3,  0.7);
    p->Add(0.6,  1.0, 0.8,  0.0);
    p->Add(0.65, 0.1, 0.1,  0.3);
    p->Add(1.0,  0.1, 0.1,  0.3);
*/
    p->Add(0.0,  0.1, 0.1,  0.2);
    p->Add(0.15, 0.7, 0.7,  1.0);
    p->Add(0.3,  0.1, 0.1,  0.2);
    p->Add(0.45, 0.3, 0.3,  0.7);
    p->Add(0.6,  1.0, 0.8,  0.0);
    p->Add(0.65, 0.1, 0.1,  0.3);
    p->Add(1.0,  0.1, 0.1,  0.3);
  }

  return p;
}

void CFractalNewton::Render(CRenderCallback * cb)
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
  case eQualityDraft:  n_main = 64; break;
  case eQualityNormal: n_main = 128; break;
  case eQualityHigh:   n_main = 256; break;
  case eQualityUltra:  n_main = 512; break;
  default:
    assert(0);
    return;
  }

  //
  // do 'bottom-up' to accomodate BMP's line ordering
  //
  if (full)
    line += w*h;

  //
  for (y=h, b=bounds.y2; y>0; y--, b-=db)
  {
    uint x;

    if (full)
      line -= w;

    for (x=0, a=bounds.x1; x<w; x++, a+=da)
    {
      _double zx=a, zy=b;

      line[x] = PixelInside;

      for (i=0; i<n_main; i++)
      {
        _double zx2 = zx*zx;
        _double zy2 = zy*zy;
        _double zxn, zyn;
        
        _double n = 1/( 3 * (zx2+zy2) * (zx2+zy2) );

        zxn = (zx/3 - (zx2-zy2)*n);
        zyn = (zy/3 + 2*zx*zy*n);

        if (zxn*zxn + zyn*zyn < 1e-6)
        {
          // converged
          _double f = PixelMax * (i % 128)/128.;
          line[x] = PixelMax & (PixelType)f;
          break;
        }

        zx -= zxn;
        zy -= zyn;
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

CFractal * InstantiateNewton()
{
  return new CFractalNewton();
}