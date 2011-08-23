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

#ifndef AFX_DIALOGABOUT_H__D9A519CF_980F_47EA_8486_A1444738C6FB__INCLUDED_
#define AFX_DIALOGABOUT_H__D9A519CF_980F_47EA_8486_A1444738C6FB__INCLUDED_

#include "resource.h"

//
// CDialogAbout dialog
//
class CDialogAbout : public CDialog
{
// Construction
public:
	CDialogAbout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogAbout)
	enum { IDD = IDD_ABOUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAbout)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGABOUT_H__D9A519CF_980F_47EA_8486_A1444738C6FB__INCLUDED_)
