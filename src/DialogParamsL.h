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

#ifndef AFX_DIALOGPARAMSL_H__28D394E7_B6E9_4F62_9508_2C02575840DC__INCLUDED_
#define AFX_DIALOGPARAMSL_H__28D394E7_B6E9_4F62_9508_2C02575840DC__INCLUDED_

#include "resource.h"

//
// CDialogParamsL dialog
//
class CDialogParamsL : public CDialog
{
public:
	CDialogParamsL(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CDialogParamsL)
	enum { IDD = IDD_FRACTAL_L };
	CComboBox	m_cSeq;
	CString	m_sSeq;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CDialogParamsL)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	static vector<CString> defaults;

	//{{AFX_MSG(CDialogParamsL)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateSel();
	afx_msg void OnUpdateEdit();
	afx_msg void OnUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif

