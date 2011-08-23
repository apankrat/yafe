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

#ifndef AFX_DIALOGRENDERTOFILE_H__E37B0F43_5A96_42B3_A313_B42307C3033D__INCLUDED_
#define AFX_DIALOGRENDERTOFILE_H__E37B0F43_5A96_42B3_A313_B42307C3033D__INCLUDED_

//
// CDialogRenderToFile dialog
//
#include "resource.h"
#include "renderer.h"

class CDialogRenderToFile : public CDialog,
                            public CRenderCallback
{
public:
	CDialogRenderToFile(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CDialogRenderToFile)
	enum { IDD = IDD_RENDER_TO_FILE };
	CProgressCtrl	m_cProgress;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CDialogRenderToFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

  //
  //  CRenderCallback
  //
  uint RcbWidth()  { return m_szImage.cx; }
  uint RcbHeight() { return m_szImage.cy; }
  bool RcbByLine() { return true; }

  bool RcbLine(uint w, uint h, uint y, PixelType *);
  void RcbDone(uint w, uint h, PixelType *) { assert(0); }

public:
  
  CFractal * m_pFractal;
  CPalette * m_pPaletteIn;
  CPalette * m_pPaletteOut;
  CSize      m_szImage;
  CString    m_sFilename;

  HANDLE     m_hThread;
  bool       m_bCancel;
  uint       m_iLine;

  HBMFILE    m_hFile;

protected:

  static DWORD WINAPI RenderThreadProxy(void *);

	//{{AFX_MSG(CDialogRenderToFile)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif

