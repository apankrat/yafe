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
 *	write like this anymore. Nor do I indent with 2 spaces now.
 */

#include "stdafx.h"
#include "afxmisc.h"

HBITMAP BitmapCreate(uint w, uint h)
{
  HBITMAP    hDib = 0;
  BITMAPINFO bmp_info = { 0 };
  void     * p_data;
  HDC        hDC = 0;

  bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmp_info.bmiHeader.biWidth = w;
  bmp_info.bmiHeader.biHeight = h;
  bmp_info.bmiHeader.biPlanes = 1;
  bmp_info.bmiHeader.biBitCount = 24;

  hDC = GetDC(HWND_DESKTOP);
  if (! hDC)
    return 0;

  hDib = CreateDIBSection(hDC, &bmp_info, DIB_RGB_COLORS, &p_data, 0, 0);
  if (! hDib)
  {
    ReleaseDC(HWND_DESKTOP, hDC);
    return 0;
  }

  return hDib;
}

void BitmapBlt(CDC * pDC, CRect * pTarget, CBitmap * pBitmap, CRect * pSource)
{
  CDC    dcTemp;
  int    iMode;

  dcTemp.CreateCompatibleDC(0);
  dcTemp.SelectObject(pBitmap);
  
  iMode = pDC->SetStretchBltMode(HALFTONE);
  if (! pSource)
  {
    BITMAP bmp;

    pBitmap->GetBitmap(&bmp);
    pDC->StretchBlt(pTarget->left, pTarget->top, pTarget->Width(), pTarget->Height(),
                    &dcTemp,
                    0,0,bmp.bmWidth,bmp.bmHeight,
                    SRCCOPY);
  }
  else
  {
    pDC->StretchBlt(pTarget->left, pTarget->top, pTarget->Width(), pTarget->Height(),
                    &dcTemp,
                    pSource->left, pSource->top, pSource->Width(), pSource->Height(),
                    SRCCOPY);
  }

  pDC->SetStretchBltMode(iMode);
}

//
//
//
uint BitmapFileSize(uint w, uint h)
{
  return 54 + h * ((3*w+3) & ~3);
}

bool BitmapFileSave(HBITMAP h, const char * filename)
{
  BITMAPFILEHEADER    hdr;
  LPBITMAPINFOHEADER  lpbi;
  DIBSECTION          dibSection;
  DWORD dwDummy;
  DWORD dwSize;
  BOOL r;

  SetLastError(ERROR_INVALID_PARAMETER);
  if (! h)
    return false;

  GetObject(h, sizeof(dibSection), &dibSection);
  lpbi = &dibSection.dsBmih;

  if (lpbi->biBitCount < 9)
    return false;

  HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  if(! hFile)
    return false;

  // Fill in the fields of the file header 
  dwSize = lpbi->biSizeImage;

  hdr.bfType       = ((WORD) ('M' << 8) | 'B');  // is always "BM"
  hdr.bfSize       = dwSize + sizeof(hdr) + sizeof(BITMAPINFOHEADER);
  hdr.bfReserved1  = 0;
  hdr.bfReserved2  = 0;
  hdr.bfOffBits    = (DWORD) (sizeof(hdr) + sizeof(BITMAPINFOHEADER));

  // Write the file header 
  r = WriteFile(hFile, &hdr, sizeof(hdr), &dwDummy, 0);

  // Write bitmap header 
  r &= WriteFile(hFile, &dibSection.dsBmih, sizeof(dibSection.dsBmih), &dwDummy, 0);

  // Write the DIB header and the bits 
  r &= WriteFile(hFile, dibSection.dsBm.bmBits, dwSize, &dwDummy, 0);

  CloseHandle(hFile);

  if (! r)
    DeleteFile(filename);

  return (bool)r;
}

//
//
//
struct CBitmapFile
{
  CString name;
  HANDLE  file;
  uint    w, h;

  uint8 * rgb;
  uint    rgbl;

  CBitmapFile()  { file = INVALID_HANDLE_VALUE; w = h = 0; rgb = 0; }
  ~CBitmapFile() { delete [] rgb; }
};

HBMFILE BitmapFileInit(uint w, const char * filename)
{
  HBMFILE h = new CBitmapFile;
  DWORD n;
  
  h->name = filename;
  h->w = w;
  h->h = 0;
  h->file = CreateFile(filename, 
                       GENERIC_WRITE, 
                       0, 0, 
                       CREATE_ALWAYS, 
                       FILE_ATTRIBUTE_NORMAL, 
                       0);
  if(! h->file)
  {
    delete h;
    return 0;
  }

  // skip headers for now
  n = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  if (n != SetFilePointer(h->file, n, 0, FILE_BEGIN))
  {
    CloseHandle(h->file);
    DeleteFile(h->name);
    delete h;
    return 0;
  }

  // single line buffer
  n = (3*w+3) & ~3;
  h->rgbl = n;
  h->rgb = new uint8[n];
  memset(h->rgb, 0, n);

/*
  BITMAPFILEHEADER  bmfh = { 0 };
  BITMAPINFOHEADER  bmih = { 0 };

  bmfh.bfType      = ((WORD) ('M' << 8) | 'B');  // is always "BM"
  bmfh.bfSize      = sizeof(hdr) + sizeof(BITMAPINFOHEADER);
  bmfh.bfOffBits   = (DWORD) (sizeof(hdr) + sizeof(BITMAPINFOHEADER));

  bmih.biSize     = 40;
  bmih.biWidth    = w; 
  bmih.biHeight   = 0; 
  bmih.biPlanes   = 1; 
  bmih.biBitCount = 24; 

  r  = WriteFile(hFile, &bmfh, sizeof(bmfh), &dwDummy, 0);
  r &= WriteFile(hFile, &bmih, sizeof(bmih), &dwDummy, 0);

  if (! r)
  {
    CloseHandle(h->h);
    DeleteFile(h->name);
    delete h;
    return 0;
  }
*/
  return h;
}

bool BitmapFileLine(HBMFILE h, COLORREF * col)
{
  uint  i, j;
  DWORD bytes;
    
  assert(h && col);

  for (i=j=0; i<h->w; i++, col++)
  {
    h->rgb[j++] = GetBValue(*col);
    h->rgb[j++] = GetGValue(*col);
    h->rgb[j++] = GetRValue(*col);
  }

  h->h++;
   
  return WriteFile(h->file, h->rgb, h->rgbl, &bytes, 0);
}

void BitmapFileDone(HBMFILE h)
{
  BITMAPFILEHEADER  bmfh = { 0 };
  BITMAPINFOHEADER  bmih = { 0 };
  DWORD dwEnd, dwSize, dw;

  //
  assert(h);

  // remember where we are at
  dwEnd = SetFilePointer(h->file, 0, 0, FILE_CURRENT);

  // rewind to the beginning of the file
  SetFilePointer(h->file, 0, 0, FILE_BEGIN);

  dwSize = h->rgbl * h->h;

  bmfh.bfType      = ((WORD) ('M' << 8) | 'B');             // BM
  bmfh.bfSize      = dwSize + sizeof(bmfh) + sizeof(bmih);
  bmfh.bfOffBits   = (DWORD) (sizeof(bmfh) + sizeof(bmih)); // 54

  bmih.biSize      = sizeof(bmih); // 40
  bmih.biWidth     = h->w; 
  bmih.biHeight    = h->h; 
  bmih.biPlanes    = 1; 
  bmih.biBitCount  = 24; 
  bmih.biSizeImage = dwSize;

  WriteFile(h->file, &bmfh, sizeof(bmfh), &dw, 0);
  WriteFile(h->file, &bmih, sizeof(bmih), &dw, 0);

  SetFilePointer(h->file, dwEnd, 0, FILE_BEGIN);
  CloseHandle(h->file);

  delete h;
}


/*
HBITMAP BitmapConvert(HBITMAP h, int bits)
{
  HBITMAP    hDib = 0;
  BITMAP     bmp      = { 0 };
  BITMAPINFO bmp_info = { 0 };
  HDC        hDC = 0;
  int        r;
  void     * p_data;

  if (! GetObject(h, sizeof(bmp), &bmp))
    return 0;

  if (bmp.bmBitsPixel == bits)  // already in required format
    return h;

  bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmp_info.bmiHeader.biWidth = bmp.bmWidth;
  bmp_info.bmiHeader.biHeight = bmp.bmHeight;
  bmp_info.bmiHeader.biPlanes = 1;
  bmp_info.bmiHeader.biBitCount = bits;

  hDC = GetDC(HWND_DESKTOP);
  if (! hDC)
    return 0;

  hDib = CreateDIBSection(hDC, &bmp_info, DIB_RGB_COLORS, &p_data, 0, 0);
  if (! hDib)
  {
    ReleaseDC(HWND_DESKTOP, hDC);
    return 0;
  }

  r = GetDIBits(hDC, h, 0, bmp_info.bmiHeader.biHeight, p_data, &bmp_info, DIB_RGB_COLORS);
  ReleaseDC(HWND_DESKTOP, hDC);

  if (r != bmp_info.bmiHeader.biHeight)
  {
    DeleteObject(hDib);
    return 0;
  }

  return hDib;
}
*/