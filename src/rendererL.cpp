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
#include "DialogParamsL.h"
#include <math.h>
#include <stdio.h>

struct CFractalLyapunov : CFractal
{
  CFractalLyapunov();

  EFractal Id()                         { return eFractalLyapunov; }
  
  CBounds  GetBounds()                  { return bounds; }
  void     SetBounds(const CBounds & b) { bounds = b; }

  EQuality GetQuality()                 { return quality; }
  void     SetQuality(EQuality q)       { quality = q; }

  CPalette * DefaultPalette(bool in);

  void Render(CRenderCallback * cb);

  //
  string    sequence;
  CBounds   bounds;
  EQuality  quality;
};

//
#define ABS_LESS(v, limit) ((-(limit) < (v)) && ((v) < (limit)))
#define ABS_MORE(v, limit) (((v) < -(limit)) || ((limit) < (v)))
#define ABS(v)             (((v) < 0) ? -(v) : (v))

//
// Calculator
//
CFractalLyapunov::CFractalLyapunov()
{
  quality = eQualityDraft;
  bounds  = CBounds(0.0, 4.0, 0.0, 4.0);

//	bounds = CBounds(3.675, 4.0, 2.725, 3.050);
}

CPalette * CFractalLyapunov::DefaultPalette(bool in)
{
  CPalette * p;

  if (in)
  {
    p = PaletteSingle(0x1B1C1F);
  }
  else
  {
    p = PaletteLinear(false);
    p->Add(0.00, HEXRGB(0x191400));
    p->Add(1.00, HEXRGB(0xFFE119)); //0xFFD40A));
  }

  return p;
}

void CFractalLyapunov::Render(CRenderCallback * cb)
{
  assert(cb);

  /*

    (1) Initialization    x = x0
                          For i=1 to INIT:  x = rx(1-x)

    (2) Derivation        total = 0        
        of the            For i=1 to NLYAP: x = rx(1-x) 
        exponent            total = total + Log|r-2rx|/Log 2
                          exponent = total / NLYAP 
  */

  uint   w = cb->RcbWidth(),
         h = cb->RcbHeight();

  _double da = (bounds.x2-bounds.x1) / (w-1);
  _double db = (bounds.y2-bounds.y1) / (h-1);
  _double a, b;
  _double norm;

  uint   nseq = sequence.size();
  uint   na, nb;
  uint   i, j, n, y, data_n;

  _double  * fseq;

  const uint ta_max = 512;   // precision back-buffer
  _double    ta[ta_max];
  uint       ta_idx;

  uint   n_init = 50;
  uint   n_main;

  bool   full = ! cb->RcbByLine();

  PixelType * data = NULL,
            * line = NULL;

  //
  assert(w>1 && h>1);

  data_n = full ? w*h : w;
  data = new PixelType[data_n];
  if (! data)
    return;

  memset(data, 0xff, data_n * sizeof(PixelType)); // PixelInside

  //
  switch (quality)
  {
  case eQualityDraft:  n_main = 50; break;
  case eQualityNormal: n_main = 500; break;
  case eQualityHigh:   n_main = 5000; break;
  default:
    assert(0);
    return;
  }

  n = n_init + n_main;
  norm = 1 / (log(2.) * n_main);

  //
  fseq = new _double[nseq];

  for (i=0, j=0, na=0; i<n; i++)
  {
    if (i >= n_init &&
        sequence[j] == 'a') na++;

    if (++j == nseq) j = 0;
  }

  //
  nb = n_main - na;

  //
  // do 'bottom-up' to accomodate BMP's line ordering
  //
  line = data;

  if (full)
    line += w*h;

  for (y=h, b=bounds.y2; y>0; y--, b-=db)
  {
    uint x;
    _double log_b = log(ABS(b));

    if (full)
      line -= w;
    else
      memset(data, 0xff, data_n * sizeof(PixelType)); // PixelInside

    // fill-in fseq's /b/s
    for (j=0; j<nseq; j++)
      if (sequence[j] != 'a')
        fseq[j] = b;

    for (x=0, a=bounds.x1; x<w; x++, a+=da)
    {
      _double f = 0.5, t = 1.0;

      // fill-in fseq's /a/s
      for (j=0; j<nseq; j++)
        if (sequence[j] == 'a')
          fseq[j] = a;

      // warm-up
      for (i=0, j=0; i<n_init; i++)
      {
        _double r = fseq[j];

        if (++j == nseq) j = 0;

        f *= r*(1-f);
      }

      // main cycle
      ta_idx = 0;    

      for (; i<n; i++)
      {
        _double r = fseq[j];

        if (++j == nseq) j = 0;

        f *= r*(1-f);
        t *= (1-f-f);

        if (ABS_LESS(t, 1e-10) ||
            ABS_MORE(t, 1e+10))
        { 
          if (ta_idx < ta_max)
          { 
            ta[ta_idx++] = t; t = 1.0;
          }
        }
      }

      _double res = log(ABS(t)) + na*log(ABS(a)) + nb*log_b;

      for (i=0; i<ta_idx; i++)
        res += log(ABS(ta[i]));

      //
      // -- render the pixel --
      //
      if (res < 0)
      {
        PixelType val;

        res *= norm;
        res = 1 - exp(5*res);

        /* midtone correction via hyperbolic arctan */
        res = exp( 5*(res - 0.5) );
        res = res / (res + 1); // == ((res - 1)/(res + 1) + 1)/2;

        //
        val = PixelMax * res;
        line[x] = (val & PixelMax);
      }
    }

    if (! cb->RcbLine(w,h,h-y,line))
      break;
  }

  delete [] fseq;

  if (full && y == 0)
  {
    cb->RcbDone(w, h, data);
    return;
  }

  delete [] data;
}

CFractal * InstantiateLyapunov()
{
  CDialogParamsL dlg(AfxGetApp()->m_pMainWnd);

  if (dlg.DoModal() != IDOK)
    return 0;

  CFractalLyapunov * f = new CFractalLyapunov();
  f->sequence = (const char*)dlg.m_sSeq;
  return f;
}