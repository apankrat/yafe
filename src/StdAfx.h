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

#ifndef AFX_STDAFX_H__5CC3AC41_4CEB_446D_8D23_A5FA61EDB0EB__INCLUDED_
#define AFX_STDAFX_H__5CC3AC41_4CEB_446D_8D23_A5FA61EDB0EB__INCLUDED_

#define VC_EXTRALEAN
#define WINVER 0x0502

#include <afxwin.h>
#include <afxext.h>
#include <afxdtctl.h>
#include <afxcmn.h>

//
// -- custom basic types --
//
typedef unsigned char     uint8;
typedef unsigned short    uint16;
typedef unsigned long     uint32;
typedef unsigned __int64  uint64;
typedef unsigned int      uint;
typedef double            _double;

//
// -- stl imports --
//
#include <vector>
using std::vector;

#include <string>
using std::string;

#include <map>
using std::multimap;

#include <cassert>

#pragma warning (disable : 4786 4244 4355 4018 4800)

#define CPalette   CFractalPalette

#include "afxmisc.h"

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_STDAFX_H__5CC3AC41_4CEB_446D_8D23_A5FA61EDB0EB__INCLUDED_)
