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
#include "LayoutManager.h"

//
CLayoutManager::CLayoutManager(CWnd * pWnd)
{
	ASSERT(pWnd);
	m_pWnd = pWnd;
	m_rClip.CreateRectRgn(0,0,0,0);
	m_dwMinWidth = 0;
	m_dwMinHeight = 0;
	m_bActive = FALSE;
}

void CLayoutManager::Initialize(UINT dwMinWidth, UINT dwMinHeight, BOOL bSnap)
{
	CRect r;

	m_pWnd->GetClientRect(&r);
	m_szOrg = r.Size();

	m_dwMinWidth = dwMinWidth;
	m_dwMinHeight = dwMinHeight;
	m_bSnapToEdges = bSnap;
}

void CLayoutManager::AddControl(int id, int edges)
{
	CControlInfo info;

	info.pWnd = m_pWnd->GetDlgItem(id);
	ASSERT(info.pWnd);

	info.pWnd->GetWindowRect(&info.rOrg);
	m_pWnd->ScreenToClient(&info.rOrg);
	info.flags = edges;

	info.pWnd->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);

	m_vControls.push_back(info);
}

void CLayoutManager::SetFlags(int id, int edges)
{
	uint i;

	for (i=0; i<m_vControls.size(); i++)
		if (m_vControls[i].pWnd->GetDlgCtrlID() == id)
		{
			m_vControls[i].flags = edges;
			break;
		}
}

void CLayoutManager::Offset(int cx, int cy)
{
	CWnd * pWnd;
	CRect r;
	CSize delta(cx,cy);

	for (pWnd = m_pWnd->GetWindow(GW_CHILD); 
	     pWnd;
	     pWnd = pWnd->GetNextWindow())
	{
		pWnd->GetWindowRect(&r);
		m_pWnd->ScreenToClient(&r);
		r += delta;
		pWnd->MoveWindow(&r);
	}
}

void CLayoutManager::Deactivate()
{
	m_bActive = FALSE;
}

void CLayoutManager::Remember()
{
	CRect r;
	uint i;
	
	m_pWnd->GetClientRect(&r);
	m_szOrg = r.Size();

	for (i=0; i<m_vControls.size(); i++)
	{
		CControlInfo & info = m_vControls[i];

		info.pWnd->GetWindowRect(&info.rOrg);
		m_pWnd->ScreenToClient(&info.rOrg);
	}

	m_bActive = TRUE;
}

BOOL CLayoutManager::OnWndMsg(UINT message, 
                              WPARAM wParam, LPARAM lParam, 
                              LRESULT * pResult,
			      BOOL bPeek)
{
	int i;
	
	if (! m_bActive)
		return FALSE;

	switch (message)
	{
	case WM_GETMINMAXINFO:
		if (! bPeek)
		{
			MINMAXINFO * pMMI = (LPMINMAXINFO) lParam;
			pMMI->ptMinTrackSize.x = m_dwMinWidth;
			pMMI->ptMinTrackSize.y = m_dwMinHeight;
		}
		break;

	case WM_NCCALCSIZE:
		if (wParam)
			OnNcCalcSize(bPeek, (LPNCCALCSIZE_PARAMS)lParam, pResult);
		break;

	case WM_ERASEBKGND:
		OnEraseBkgnd(bPeek, (HDC)wParam);
		break;

	case WM_SIZE:
		if (! bPeek)
			OnSize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_MOVING:
		if (bPeek)
		{
			OnMoving(wParam, (RECT*)lParam); 
			*pResult = TRUE;
			return TRUE;
		}
		break;

	case WM_ENTERSIZEMOVE:
	case WM_EXITSIZEMOVE:
		if (! bPeek)
			break;
		for (i=0; i<m_vControls.size(); i++)
			m_vControls[i].pWnd->SendMessage(message, wParam, lParam);
		break;
	}

	return FALSE;
}

//
void CLayoutManager::OnSize(UINT w, UINT h)
{
	CRect r, rOrg;
	uint i;
	int dx, dy;
	HDWP hDwp;
	uint uFlags;

	dx = w - m_szOrg.cx;
	dy = h - m_szOrg.cy;

	hDwp = ::BeginDeferWindowPos(m_vControls.size());

	/* move controls around */
	for (i=0; i<m_vControls.size(); i++)
	{
		CControlInfo & info = m_vControls[i];

		info.pWnd->GetWindowRect(&rOrg);
		m_pWnd->ScreenToClient(&rOrg);

		r = info.rOrg;
		
		if (info.flags & LME_TOP)    r.top += dy;
		if (info.flags & LME_BOTTOM) r.bottom += dy;
		if (info.flags & LME_LEFT)   r.left += dx;
		if (info.flags & LME_RIGHT)  r.right += dx;

		if (info.flags & LME_CENTER)
		{
			// preserve aspect ratio and center in target area
			_double f = info.rOrg.Width() / (_double)info.rOrg.Height();
			int w = r.Width();
			int h = r.Height();

			if (h*f > w) { dx = 0; dy = h - w/f; }
			else         { dx = w - h*f; dy = 0; }

			r.DeflateRect(dx/2, dy/2);
			if (dx & 1)
				r.left++;

			if (dy & 1)
				r.top++;
		}

		if (r == rOrg)
			continue;

		uFlags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREPOSITION;

		if ( (info.flags & LMR_SIZE) && (r.Size() != rOrg.Size()) )
			uFlags |= SWP_NOCOPYBITS;

		::DeferWindowPos(hDwp, info.pWnd->m_hWnd, 0,
			r.left, r.top, r.Width(), r.Height(),
			uFlags);
	}

	EndDeferWindowPos(hDwp);
}

void CLayoutManager::OnNcCalcSize(BOOL bPeek, NCCALCSIZE_PARAMS * pNcsp, LRESULT * pResult)
{
	if (bPeek)
	{
		m_rBefore = pNcsp->rgrc[2];
		return;
	}

	if (*pResult)
		return;
	
	::IntersectRect(&pNcsp->rgrc[1], &pNcsp->rgrc[0], &m_rBefore);
	pNcsp->rgrc[2] = pNcsp->rgrc[1];

	*pResult = WVR_VALIDRECTS;
}

void CLayoutManager::OnEraseBkgnd(BOOL bPeek, HDC hDC)
{
	CDC * pDC = CDC::FromHandle(hDC);

	if (bPeek)
	{
		CWnd * pWnd;
		CRect r;
		CRgn rNew, rTmp;

		/* save */
		::GetClipRgn(pDC->m_hDC, (HRGN)m_rClip.m_hObject);

		/* refine */
		rNew.CreateRectRgn(0,0,0,0);
		for (pWnd = m_pWnd->GetWindow(GW_CHILD); 
		     pWnd;
		     pWnd = pWnd->GetNextWindow())
		{
			if (! pWnd->IsWindowVisible())
				continue;

			pWnd->GetWindowRect(&r);
			m_pWnd->ScreenToClient(&r);
			rTmp.CreateRectRgnIndirect(&r);	
			rNew.CombineRgn(&rNew, &rTmp, RGN_OR);
			rTmp.DeleteObject();
		}
		pDC->SelectClipRgn(&rNew, RGN_DIFF);
	}
	else
	{
		/* restore */
		pDC->SelectClipRgn(&m_rClip, RGN_COPY);
	}
}

void CLayoutManager::OnMoving(UINT fwSide, LPRECT pRect)
{
	#define THRESHOLD  5

	if (! m_bSnapToEdges || (GetKeyState(VK_CONTROL) & 0x8000))
		return;

	// stick to screen edges (a'la WinAmp)
	CRect rect_now;
	m_pWnd->GetWindowRect(&rect_now);

	int w = GetSystemMetrics(SM_CXFULLSCREEN) + 1;
	int h = GetSystemMetrics(SM_CYFULLSCREEN) + GetSystemMetrics(SM_CYCAPTION) + 1;
	int hdir = pRect->right - rect_now.right;
	int vdir = pRect->top - rect_now.top;

	// horizontal
	if (abs(pRect->left) < THRESHOLD)
	{
		int d = -pRect->left;

		if (hdir > 0)
			d += THRESHOLD;

		pRect->right += d;
		pRect->left  += d;
	}
	else
	if (abs(w - pRect->right) < THRESHOLD)
	{
		int d = w - pRect->right;

		if (hdir < 0)
			d -= THRESHOLD;

		pRect->left  += d;
		pRect->right += d;
	}

	// vertical
	if (abs(pRect->top) < THRESHOLD)
	{
		int d = -pRect->top;

		if (vdir > 0)
			d += THRESHOLD;

		pRect->top    += d;
		pRect->bottom += d;
	}
	else
	if (abs(h - pRect->bottom) < THRESHOLD)
	{
		int d = h - pRect->bottom;

		if (vdir < 0)
			d -= THRESHOLD;

		pRect->top    += d;
		pRect->bottom += d;
	}
}