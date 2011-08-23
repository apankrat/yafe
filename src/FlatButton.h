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

#ifndef AFX_FLATBUTTON_H__6EE68D61_A73C_416D_9BF5_B5E1B4CED86D__INCLUDED_
#define AFX_FLATBUTTON_H__6EE68D61_A73C_416D_9BF5_B5E1B4CED86D__INCLUDED_

//
class CFlatButton : public CButton
{
public:
	CFlatButton();

public:

	//{{AFX_VIRTUAL(CFlatButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

public:
	virtual ~CFlatButton();

protected:
  CFont   m_cFont;
  CString m_sLabel;
	//{{AFX_MSG(CFlatButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif

