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

#ifndef _LAYOUT_MANAGER_H_
#define _LAYOUT_MANAGER_H_

/*
 *  which edges of the control to move when parent window is resized
 */
#define LME_LEFT      0x01
#define LME_RIGHT     0x02
#define LME_TOP       0x04
#define LME_BOTTOM    0x08
#define LME_ALL       0x0F

#define LME_CENTER    0x10
#define LMR_SIZE      0x80   /* redraw on size change */

//
class CLayoutManager
{
public:

	CLayoutManager(CWnd * pWnd);

	void Initialize(UINT dwMinWidth, UINT dwMinHeight, BOOL bSnapToEdges);
	void AddControl(int id, int edges);
	void SetFlags(int id, int edges);

	void Offset(int cx, int cy);

	void Deactivate();
	void Remember();

	BOOL OnWndMsg(UINT, WPARAM, LPARAM, LRESULT *, BOOL bPeek);

protected:

	void OnSize(UINT w, UINT h);

	void OnNcCalcSize(BOOL bPeek, NCCALCSIZE_PARAMS * pNcsp, LRESULT * pRes);
	void OnEraseBkgnd(BOOL bPeek, HDC hDC);

	void OnMoving(UINT fwSide, LPRECT pRect);

protected:

	struct CControlInfo
	{
		CWnd * pWnd;
		CRect  rOrg;
		int    flags;
	};

protected:

	CWnd * m_pWnd;
	BOOL   m_bActive;
	CSize  m_szOrg;
	DWORD  m_dwMinHeight, m_dwMinWidth;
	CRect  m_rBefore;
	CRgn   m_rClip;
	BOOL   m_bSnapToEdges;

	vector<CControlInfo> m_vControls;
};

#endif
