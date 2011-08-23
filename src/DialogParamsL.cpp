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
 *	write like this anymore, nor do I indent with 2 spaces now.
 */

#include "stdafx.h"
#include "DialogParamsL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// CDialogParamsL dialog
//
vector<CString> CDialogParamsL::defaults;

//
CDialogParamsL::CDialogParamsL(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogParamsL::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogParamsL)
	m_sSeq = _T("");
	//}}AFX_DATA_INIT
}


void CDialogParamsL::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogParamsL)
	DDX_Control(pDX, IDC_SEQ, m_cSeq);
	DDX_CBString(pDX, IDC_SEQ, m_sSeq);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogParamsL, CDialog)
	//{{AFX_MSG_MAP(CDialogParamsL)
	ON_CBN_EDITCHANGE(IDC_SEQ, OnUpdateEdit)
	ON_CBN_SELCHANGE(IDC_SEQ, OnUpdateSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogParamsL message handlers
BOOL CDialogParamsL::OnInitDialog() 
{
	CDialog::OnInitDialog();

  if (defaults.size())
  {
    m_cSeq.ResetContent();

    for (uint i=0; i<defaults.size(); i++)
      m_cSeq.AddString(defaults[i]);
  }

  m_cSeq.LimitText(63);
  m_cSeq.SetCurSel(0);
  OnUpdateSel();
  return TRUE;
}

BOOL CDialogParamsL::DestroyWindow() 
{
  defaults.clear();

  if (m_cSeq.FindStringExact(0, m_sSeq) == CB_ERR)
    defaults.push_back(m_sSeq);

  for (uint i=0, n=m_cSeq.GetCount(); i<n; i++)
  {
    CString str;
    m_cSeq.GetLBText(i, str);
    defaults.push_back(str);
  }

  return CDialog::DestroyWindow();
}

void CDialogParamsL::OnUpdateSel()
{
  m_cSeq.GetLBText(m_cSeq.GetCurSel(), m_sSeq); 
  OnUpdate();
}

void CDialogParamsL::OnUpdateEdit()
{
  m_cSeq.GetWindowText(m_sSeq);
  OnUpdate();
}

void CDialogParamsL::OnUpdate()
{
  GetDlgItem(IDOK)->EnableWindow(m_sSeq.GetLength() &&
                                 m_sSeq.Find('a') != -1 && 
                                 m_sSeq.Find('b') != -1 );	
}