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
#include "FlatButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// CFlatButton
//
CFlatButton::CFlatButton()
{
}

CFlatButton::~CFlatButton()
{
}


BEGIN_MESSAGE_MAP(CFlatButton, CButton)
	//{{AFX_MSG_MAP(CFlatButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
// CFlatButton message handlers
//
void CFlatButton::DrawItem(LPDRAWITEMSTRUCT pDI) 
{
  CDC * pDC = CDC::FromHandle(pDI->hDC);
  CRect r;

  if (! m_cFont.m_hObject)
  {
    CFont * pFont = GetFont();
    LOGFONT lFont;

    pFont->GetLogFont(&lFont);
    m_cFont.CreatePointFont(10*(1-lFont.lfHeight), "Webdings", 0);
    GetWindowText(m_sLabel);
  }

  r = pDI->rcItem;

  pDC->Draw3dRect(&r, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
  r.DeflateRect(1,1);

  pDC->FillSolidRect(&r, GetSysColor(COLOR_3DLIGHT));

  //
  if (pDI->itemState & ODS_SELECTED)
  {
    pDC->Draw3dRect(&r, GetSysColor(COLOR_3DLIGHT), GetSysColor(COLOR_3DLIGHT));
    r += CSize(0,1);
  }
  else
  {
    pDC->Draw3dRect(&r, GetSysColor(COLOR_3DHILIGHT), GetSysColor(COLOR_3DSHADOW));
  }

  r.DeflateRect(1,1);

  pDC->SelectObject(&m_cFont);
  pDC->SetTextColor(GetSysColor( (pDI->itemState & ODS_DISABLED) ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
  pDC->SetBkMode(TRANSPARENT);
  pDC->DrawText(m_sLabel, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

BOOL CFlatButton::PreTranslateMessage(MSG* pMsg) 
{
  switch (pMsg->message)
  {
  case WM_LBUTTONDBLCLK: pMsg->message = WM_LBUTTONDOWN; break;
  case WM_MBUTTONDBLCLK: pMsg->message = WM_MBUTTONDOWN; break;
  case WM_RBUTTONDBLCLK: pMsg->message = WM_RBUTTONDOWN; break;
  } 	
  return CButton::PreTranslateMessage(pMsg);
}

