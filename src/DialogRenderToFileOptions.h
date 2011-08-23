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

#ifndef AFX_DIALOGRENDERTOFILEOPTIONS_H__0D421CA1_FCA9_444A_B9B1_7D0B881BE6E8__INCLUDED_
#define AFX_DIALOGRENDERTOFILEOPTIONS_H__0D421CA1_FCA9_444A_B9B1_7D0B881BE6E8__INCLUDED_

//
// CDialogRenderToFileOptions dialog
//
#include "resource.h"

class CDialogRenderToFileOptions : public CDialog
{
// Construction
public:
	CDialogRenderToFileOptions(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CDialogRenderToFileOptions)
	enum { IDD = IDD_RTF_OPTIONS };
	CString	m_sFilename;
	CString	m_sFilesize;
	CString	m_sSize;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(CDialogRenderToFileOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
public:

  unsigned m_nSize;

protected:

	//{{AFX_MSG(CDialogRenderToFileOptions)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeSize();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif

