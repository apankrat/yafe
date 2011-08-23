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

#ifndef AFX_DIALOGMAIN_H__2142F50F_0F11_40D4_A66B_601684160BA5__INCLUDED_
#define AFX_DIALOGMAIN_H__2142F50F_0F11_40D4_A66B_601684160BA5__INCLUDED_

#include "FlatButton.h"
#include "HistoryPane.h"
#include "ImageArea.h"
#include "LayoutManager.h"
#include "renderer.h"
#include "palette.h"

//
//
//
struct CSnapshot
{
  CSize     m_size;     //  actual size of the image
  CBitmap * m_bitmap;   
  CBounds   m_quad;     //  virtual rectangle
};

//
//
//
class CDialogMain : public CDialog, 
                    public CRenderCallback
{
public:
	CDialogMain(CWnd* pParent = NULL);	// standard constructor

	//{{AFX_DATA(CDialogMain)
	enum { IDD = IDD_MAIN };
	CImageArea	m_cProxy;
	CFlatButton	m_cHistoryUp;
	CFlatButton	m_cHistoryDown;
	CHistoryPane	m_cHistory;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CDialogMain)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
  virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult);
	//}}AFX_VIRTUAL

protected:

  void WipeClean();

  void Render();
  void RenderCancel();

  void RenderThread();
  
  uint RcbWidth();
  uint RcbHeight();
  bool RcbByLine();
  bool RcbLine(uint w, uint h, uint y, PixelType *);
  void RcbDone(uint w, uint h, PixelType * data);

  static DWORD WINAPI RenderThreadProxy(void *);

  void SetPalette(CPalette * in, CPalette * out);
  void SetDefaultPalette();

protected:

  COLORREF       m_colBack;
  CLayoutManager m_layout;
  CSize          m_szBeforeSize;
  bool           m_bActive;

  CFractal * m_pFractal;
  CPalette * m_pPaletteIn;
  CPalette * m_pPaletteOut;
  uint       m_uPaletteIdIn, 
             m_uPaletteIdOut;

  HANDLE     m_hRenderThread;
  bool       m_bRenderCancel;
  bool       m_bAntialias;

  CString    m_sFilename;

	//{{AFX_MSG(CDialogMain)
	virtual BOOL OnInitDialog();
	afx_msg void OnHistoryDown();
	afx_msg void OnHistoryUp();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
  afx_msg LRESULT OnHistoryClick(WPARAM, LPARAM);
  afx_msg LRESULT OnHistoryScroll(WPARAM, LPARAM);
  afx_msg LRESULT OnPictureZoom(WPARAM, LPARAM);
  afx_msg LRESULT OnPictureRClick(WPARAM, LPARAM);
  afx_msg LRESULT OnRenderProgress(WPARAM, LPARAM);
  afx_msg LRESULT OnRenderComplete(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif

