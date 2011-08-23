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

#ifndef AFX_DIALOGGOTO_H__EC4FEB59_BFA6_4C88_BA88_C64B6DBDDCDC__INCLUDED_
#define AFX_DIALOGGOTO_H__EC4FEB59_BFA6_4C88_BA88_C64B6DBDDCDC__INCLUDED_

#include "afxmisc.h"
#include "resource.h"

//
// CDialogGoto dialog
//
class CDialogGoto : public CDialog
{
// Construction
public:
	CDialogGoto(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogGoto)
	enum { IDD = IDD_GOTO };
	CString	m_sx1;
	CString	m_sx2;
	CString	m_sy1;
	CString	m_sy2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogGoto)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
  
  CBounds m_quad;

// Implementation
protected:

  void Assign();
  bool Parse();

  CString Convert(_double v);
  bool Convert(CString val, uint id, _double & v);

	// Generated message map functions
	//{{AFX_MSG(CDialogGoto)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
#endif
