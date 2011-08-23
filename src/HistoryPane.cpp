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
#include "HistoryPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XBORDER 3
#define YBORDER 3
#define YSPACE  2

//
// CHistoryPane
//
CHistoryPane::CHistoryPane()
{
  m_uTopItem = 0;
  m_uTrailingGap = 0;
}

CHistoryPane::~CHistoryPane()
{
}


BEGIN_MESSAGE_MAP(CHistoryPane, CStatic)
	//{{AFX_MSG_MAP(CHistoryPane)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
void CHistoryPane::Initialize(uint msgClick, uint msgScroll)
{
  CRect r;
  
  m_uMsgClick  = msgClick;
  m_uMsgScroll = msgScroll;

  GetClientRect(&r);
  m_uWidth = r.Width()-2*XBORDER;
}

void CHistoryPane::Push(const CHistoryItem & a)
{
  CItem  item;
  BITMAP bitmap;
  _double f;

  a.image->GetBitmap(&bitmap);
  f = m_uWidth / (_double)bitmap.bmWidth;

  ((CHistoryItem&)item) = a;
  item.w  = bitmap.bmWidth;
  item.h  = bitmap.bmHeight;
  item.h0 = bitmap.bmHeight * f;
  item.sel0 = a.sel * f;
  item.image0 = new CBitmap;
  item.image0->Attach(BitmapCreate(m_uWidth, item.h0));

  {
    CDC dc;
    CRect r(0,0,m_uWidth, item.h0);

    dc.CreateCompatibleDC(0);
    dc.SelectObject(item.image0);

    BitmapBlt(&dc, &r, item.image, 0);
  }

  m_vItems.push_back(item);
  RedrawWindow();

  while (CanScrollDown())
    Down();
}

void CHistoryPane::Pop()
{
  ASSERT(m_vItems.size());

  delete m_vItems.back().image;
  delete m_vItems.back().image0;
  m_vItems.pop_back();

  Update();
  RedrawWindow();
}

void CHistoryPane::Up()
{
  m_uTopItem--;	
  m_bSendScroll = true;
  RedrawWindow();
}

void CHistoryPane::Down()
{
  m_uTopItem++;	
  m_bSendScroll = true;
  RedrawWindow();
}

bool CHistoryPane::CanScrollUp()
{
  return m_uTopItem > 0;
}

bool CHistoryPane::CanScrollDown()
{
  return m_uTrailingGap == 0;
}

const CHistoryItem & CHistoryPane::Item(uint index)
{
  if (index == -1)
    index = m_vItems.size()-1;
    
  ASSERT(index < m_vItems.size());
  return m_vItems[index];
}

uint CHistoryPane::Size()
{
  return m_vItems.size();
}

//
uint CHistoryPane::HitTest(const CPoint & pt)
{
  uint i, y;

  if (pt.x < XBORDER || XBORDER+m_uWidth+XBORDER < pt.x)
    return -1;

  y = YBORDER;
  for (i=m_uTopItem; i<m_vItems.size(); i++)
  {
    if (y <= pt.y && pt.y <= y+m_vItems[i].h0)
      return i;

    y += m_vItems[i].h0 + YSPACE;
  }

  return -1;
}

void CHistoryPane::Update()
{
  RedrawWindow();
  
  while (m_uTopItem &&
         m_uTrailingGap >= m_vItems[m_uTopItem-1].h0)
  {
    m_uTopItem--;	
    RedrawWindow();
  }

  m_bSendScroll = true;
}

//
// CHistoryPane message handlers
//
void CHistoryPane::OnPaint() 
{
	CPaintDC dc(this);

  CRect r;
  CRgn  rgn;
  CPen  pen;
  uint  w, i;
  uint  y, gap;
  COLORREF colBack;
  
  GetClientRect(&r);
  colBack = GetSysColor(COLOR_3DLIGHT);

  //
  dc.FillSolidRect(0, 0, XBORDER, r.bottom, colBack);
  dc.FillSolidRect(r.right-XBORDER, 0, XBORDER, r.bottom, colBack);

  dc.FillSolidRect(XBORDER, 0, r.right-r.left-2*XBORDER, YBORDER, colBack);
  dc.FillSolidRect(XBORDER, r.bottom-YBORDER, r.right-r.left-2*XBORDER, YBORDER, colBack);

  dc.Draw3dRect(&r, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));

  //
  r.DeflateRect(XBORDER, YBORDER);
  rgn.CreateRectRgnIndirect(&r);
  dc.SelectClipRgn(&rgn, RGN_COPY);

  pen.CreatePen(PS_SOLID, 1, RGB(0xff,0x3f,0x00));

  //dc.SetROP2(R2_XORPEN);
  dc.SelectObject(&pen);
  dc.SelectStockObject(NULL_BRUSH);
  dc.SetStretchBltMode(HALFTONE);

  //
  w = m_uWidth;
  y = YBORDER;
  gap = 0;

  for (i=m_uTopItem; i<m_vItems.size(); i++)
  {
    CItem & info = m_vItems[i];
    CRect rTmp(0,0,0,0);

    rTmp = CRect(XBORDER, y, XBORDER+m_uWidth, y+info.h0);
    BitmapBlt(&dc, &rTmp, info.image0, 0);

    if (info.sel0 != rTmp)
    {
      rTmp = info.sel0 + CSize(0,y);
      dc.Rectangle(&rTmp);
    }

    dc.FillSolidRect(XBORDER, y+info.h0, m_uWidth, YSPACE, GetSysColor(COLOR_3DLIGHT));
    //dc.Draw3dRect(XBORDER, y, w, info.h0, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));

    y += info.h0 + YSPACE;
    if (y > r.bottom)
      break;
  }

  if (y < r.bottom)
  {
    gap = r.bottom - y;
    dc.FillSolidRect(XBORDER, y, m_uWidth, r.bottom-y, colBack);
  }

  if (m_bSendScroll ||
      gap != m_uTrailingGap)
  {
    m_uTrailingGap = gap;
    m_bSendScroll = false;
    GetParent()->PostMessage(m_uMsgScroll);
  }
}

void CHistoryPane::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_uClickItem = HitTest(point);	
	CStatic::OnLButtonDown(nFlags, point);
}

void CHistoryPane::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_uClickItem != -1 &&
      m_uClickItem == HitTest(point))
    
    GetParent()->PostMessage(m_uMsgClick, m_uClickItem, 0);
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CHistoryPane::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
  Update();
  RedrawWindow();
}
