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

#ifndef _FRACT_RENDERED_H_
#define _FRACT_RENDERED_H_

#include "stdafx.h"
#include "palette.h"

//
enum EQuality
{
  eQualityUnknown,

  eQualityDraft,
  eQualityNormal,
  eQualityHigh,
  eQualityUltra
};

//
enum EFractal
{
  eFractalUnknown,

  eFractalLyapunov,
  eFractalMandelbrot,
  eFractalNewton
};

//
typedef uint16 PixelType;
#define PixelInside 0x8000
#define PixelMax    0x7FFF

//
struct CRenderCallback
{
  //
  virtual uint RcbWidth() = 0;
  virtual uint RcbHeight() = 0;
  virtual bool RcbByLine() = 0; // whether RcbDone is called or not
  //
  virtual bool RcbLine(uint w, uint h, uint y, PixelType *) = 0;
  virtual void RcbDone(uint w, uint h, PixelType *) = 0;

protected:
  virtual ~CRenderCallback() {}
};

//
struct CFractal
{
  virtual EFractal Id() = 0;

  virtual CBounds  GetBounds() = 0;
  virtual void     SetBounds(const CBounds &) = 0;

  virtual EQuality GetQuality() = 0;
  virtual void     SetQuality(EQuality) = 0;

  virtual CPalette * DefaultPalette(bool in) = 0;

  virtual void Render(CRenderCallback * cb) = 0;
  
  virtual ~CFractal() {}

protected:

  CFractal() {}
  CFractal(const CFractal &);
  void operator = (const CFractal &);
};

//
CFractal * InstantiateMandelbrot();
CFractal * InstantiateLyapunov();
CFractal * InstantiateNewton();

#endif
