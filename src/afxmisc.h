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

#ifndef _AFX_MISC_EXTENSIONS_H_
#define _AFX_MISC_EXTENSIONS_H_

#include "stdafx.h"

//
struct CBounds
{
  _double x1, x2, y1, y2;

  CBounds() { x1=x2=y1=y2=0; }
  CBounds(_double _x1, _double _x2, _double _y1, _double _y2) { x1=_x1;y1=_y1;x2=_x2;y2=_y2; }

  CBounds operator * (_double v) { return CBounds(x1*v,y1*v,x2*v,y2*v); }
  CBounds operator / (_double v) { return CBounds(x1/v,y1/v,x2/v,y2/v); }

  CBounds operator + (const CBounds & q) { return CBounds(x1+q.x1,y1+q.y1,x2+q.x2,y2+q.y2); }
  CBounds operator - (const CBounds & q) { return CBounds(x1-q.x1,y1-q.y1,x2-q.x2,y2-q.y2); }
};

//
inline CRect operator * (const CRect & r, _double v)
{ return CRect(r.left*v, r.top*v, r.right*v, r.bottom*v); }

inline CRect operator / (const CRect & r, _double v)
{ return CRect(r.left/v, r.top/v, r.right/v, r.bottom/v); }

inline CRect operator + (const CRect & a, const CRect & b)
{ return CRect(a.left+b.left, a.top+b.top, a.right+b.right, a.bottom+b.bottom); }
 
inline CRect operator - (const CRect & a, const CRect & b)
{ return CRect(a.left-b.left, a.top-b.top, a.right-b.right, a.bottom-b.bottom); }

//
HBITMAP BitmapCreate(uint w, uint h);
void    BitmapBlt(CDC * pDC, CRect * pTarget, CBitmap * pBitmap, CRect * pSource);

//
typedef struct CBitmapFile * HBMFILE;

//
HBMFILE BitmapFileInit(uint w, const char * filename);
bool    BitmapFileLine(HBMFILE, COLORREF * col);
void    BitmapFileDone(HBMFILE);

//
uint    BitmapFileSize(uint w, uint h);
bool    BitmapFileSave(HBITMAP h, const char * filename);

#endif
