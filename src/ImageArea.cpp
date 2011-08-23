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
#include "ImageArea.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//
//
struct CRectTrackerEx : CRectTracker
{
  void AdjustRect(int nHandle, LPRECT _pRect)
  {
    CRect * pRect = (CRect*)_pRect;

    CRectTracker::AdjustRect(nHandle, pRect);

    pRect->left   = min(max(pRect->left, m_rBounds.left), m_rBounds.right);
    pRect->right  = min(max(pRect->right, m_rBounds.left), m_rBounds.right);

    pRect->top    = min(max(pRect->top, m_rBounds.top), m_rBounds.bottom);
    pRect->bottom = min(max(pRect->bottom, m_rBounds.top), m_rBounds.bottom);

    if (! (GetKeyState(VK_CONTROL) & 0x8000))
    {
      pRect->bottom = pRect->top + pRect->Width(); //pRect->right - pRect->left;

      if (pRect->bottom > m_rBounds.bottom)
      {
        int delta = pRect->bottom - m_rBounds.bottom;
        pRect->bottom -= delta;
        pRect->right -= delta;
      }

      if (pRect->bottom < 0)
      {
        pRect->right += pRect->bottom;
        pRect->bottom = 0;
      }
    }
  }

  CRect m_rBounds;
};

//
//
//
CImageArea::CImageArea()
{
  m_bSizing = false;
  m_uMsgZoom = 0;
}

CImageArea::~CImageArea()
{
}


BEGIN_MESSAGE_MAP(CImageArea, CStatic)
	//{{AFX_MSG_MAP(CImageArea)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
void CImageArea::Initialize(uint msgZoom, uint msgRClick)
{
  m_uMsgZoom = msgZoom;
  m_uMsgRClick = msgRClick;
}

//
BOOL CImageArea::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult)
{
  switch (message)
  {
  case WM_ENTERSIZEMOVE: m_bSizing = true; break;
  case WM_EXITSIZEMOVE: m_bSizing = false; Resize(); break;
  case WM_KEYUP: GetParent()->PostMessage(message, wParam, lParam); break;
  }

  return CStatic::OnWndMsg(message, wParam, lParam, pResult);
}

void CImageArea::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
  CRect r;

  GetClientRect(&r);

  if (m_bSizing)
  {
    COLORREF colBack, colFore;

    colBack = GetSysColor(COLOR_3DFACE);
    colFore = colBack + RGB(15,15,15);

    dc.Draw3dRect(&r, colBack, colBack);

    dc.SelectStockObject(BLACK_PEN);

    r.right--;
    r.bottom--;
    
    dc.MoveTo(r.left, r.top+10);
    dc.LineTo(r.left, r.top);
    dc.LineTo(r.left+10, r.top);

    dc.MoveTo(r.right, r.top+10);
    dc.LineTo(r.right, r.top);
    dc.LineTo(r.right-10, r.top);

    dc.MoveTo(r.right, r.bottom-10);
    dc.LineTo(r.right, r.bottom);
    dc.LineTo(r.right-10, r.bottom);

    dc.MoveTo(r.left, r.bottom-10);
    dc.LineTo(r.left, r.bottom);
    dc.LineTo(r.left+10, r.bottom);

    r.right++;
    r.bottom++;

    //
    r.DeflateRect(1,1);
    dc.FillSolidRect(&r, colFore);

    //
    CString text;
    
    dc.SetTextColor(RGB(0,0,0));
    dc.SelectObject(GetParent()->GetFont());

    text.Format("%u x %u", r.Width()+2, r.Height()+2);
    dc.DrawText(text, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  }	
  else
  {
    dc.BitBlt(0,0,m_szBitmap.cx,m_szBitmap.cy,
              &m_dc, 0,0, SRCCOPY);
  }
}

void CImageArea::Resize()
{
  CRect   r;
  CBitmap bitmap;
  
  if (! m_dc.m_hDC)
  {
    m_dc.CreateCompatibleDC(0);
    m_dc.SetStretchBltMode(HALFTONE);
  }

  GetClientRect(&r);

  bitmap.Attach(BitmapCreate(r.Width(), r.Height()));
  
  if (m_bitmap.m_hObject)
  {
    CDC dcDst;

    dcDst.CreateCompatibleDC(0);
    dcDst.SelectObject(&bitmap);
    dcDst.SetStretchBltMode(HALFTONE);

    dcDst.StretchBlt(0,0,r.Width(),r.Height(),
                     &m_dc,
                     0,0,m_szBitmap.cx,m_szBitmap.cy,
                     SRCCOPY);
  }

  m_bitmap.DeleteObject();
  m_bitmap.Attach(bitmap.Detach());
  m_dc.SelectObject(&m_bitmap);

  m_szBitmap = r.Size();
  Invalidate(0);
}

CBitmap * CImageArea::Capture()
{
  CBitmap * bitmap = new CBitmap;
  CDC       dcTemp;
  CGdiObject * objTemp;
 
  bitmap->Attach(BitmapCreate(m_szBitmap.cx, m_szBitmap.cy));

  dcTemp.CreateCompatibleDC(0);
  objTemp = dcTemp.SelectObject(bitmap);
  dcTemp.SetStretchBltMode(HALFTONE);

  dcTemp.BitBlt(0,0,m_szBitmap.cx,m_szBitmap.cy,
                &m_dc,
                0,0,
                SRCCOPY);

  dcTemp.SelectObject(objTemp);
  return bitmap;
}

//
void CImageArea::OnLButtonDown(UINT nFlags, CPoint point) 
{
  CRectTrackerEx  tracker;

  tracker.m_nStyle = CRectTracker::dottedLine;
  GetClientRect(&tracker.m_rBounds);

  if (! tracker.TrackRubberBand(this, point, TRUE))
  {
	  CStatic::OnLButtonDown(nFlags, point);
    return;
  }

  m_rZoom = tracker.m_rect;
  m_rZoom.NormalizeRect();
  GetParent()->SendMessage(m_uMsgZoom);
}

void CImageArea::OnRButtonUp(UINT nFlags, CPoint point) 
{
  GetParent()->SendMessage(m_uMsgRClick, point.x, point.y);
	CStatic::OnRButtonUp(nFlags, point);
}
