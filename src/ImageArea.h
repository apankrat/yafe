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

#ifndef AFX_IMAGEAREA_H__85408ADD_34CF_44DF_B64E_1C8DA330131A__INCLUDED_
#define AFX_IMAGEAREA_H__85408ADD_34CF_44DF_B64E_1C8DA330131A__INCLUDED_

//
// CImageArea window
//
class CImageArea : public CStatic
{
public:
	CImageArea();

public:

	//{{AFX_VIRTUAL(CImageArea)
	//}}AFX_VIRTUAL

public:
	virtual ~CImageArea();

  void Initialize(uint msgZoom, uint msgRClick);
  void Resize();

  CBitmap * Capture();

protected:
public:

  CSize   m_szBitmap;
  CRect   m_rZoom;
  
  bool    m_bSizing;
  CDC     m_dc;
  CBitmap m_bitmap;

  uint    m_uMsgZoom, m_uMsgRClick;

  
	//{{AFX_MSG(CImageArea)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif

