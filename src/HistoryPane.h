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

#ifndef AFX_HISTORYPANE_H__869BD7D5_0C69_42D4_9C5A_27D25DAE8436__INCLUDED_
#define AFX_HISTORYPANE_H__869BD7D5_0C69_42D4_9C5A_27D25DAE8436__INCLUDED_

//
struct CHistoryItem
{
  CBitmap * image;
  CBounds   quad;
  CRect     sel;
  uint      palIn, palOut; // palletes
};

//
class CHistoryPane : public CStatic
{
public:
	CHistoryPane();

	//{{AFX_VIRTUAL(CHistoryPane)
	//}}AFX_VIRTUAL

public:

	virtual ~CHistoryPane();

  void Initialize(uint msgClick, uint msgScroll);
  void Push(const CHistoryItem & );
  void Pop();

  void Up();
  void Down();

  bool CanScrollUp();
  bool CanScrollDown();

  const CHistoryItem & Item(uint index);
  uint  Size();

protected:
  
  uint HitTest(const CPoint & pt);
  void Update();

protected:

  struct CItem : CHistoryItem
  {
    uint      w, h, h0;
    CRect     sel0;
    CBitmap * image0;
  };

protected:

  uint          m_uWidth;
  uint          m_uMsgClick, m_uMsgScroll;
  vector<CItem> m_vItems;
  uint          m_uTopItem;
  uint          m_uClickItem;
  uint          m_uTrailingGap;
  bool          m_bSendScroll;

	//{{AFX_MSG(CHistoryPane)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_HISTORYPANE_H__869BD7D5_0C69_42D4_9C5A_27D25DAE8436__INCLUDED_)
