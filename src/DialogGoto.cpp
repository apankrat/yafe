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
#include "DialogGoto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogGoto dialog


CDialogGoto::CDialogGoto(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogGoto::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogGoto)
	m_sx1 = _T("");
	m_sx2 = _T("");
	m_sy1 = _T("");
	m_sy2 = _T("");
	//}}AFX_DATA_INIT
}


void CDialogGoto::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogGoto)
	DDX_Text(pDX, IDC_X1, m_sx1);
	DDX_Text(pDX, IDC_X2, m_sx2);
	DDX_Text(pDX, IDC_Y1, m_sy1);
	DDX_Text(pDX, IDC_Y2, m_sy2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogGoto, CDialog)
	//{{AFX_MSG_MAP(CDialogGoto)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogGoto message handlers

BOOL CDialogGoto::OnInitDialog() 
{
  TOOLINFO ti = { sizeof(TOOLINFO), TTF_SUBCLASS, };
  
  CDialog::OnInitDialog();

  Assign();
  return TRUE;
}

void CDialogGoto::OnOK() 
{
  if (! Parse())
    return;

  // Normalize
  if (m_quad.x1 > m_quad.x2)
    std::swap(m_quad.x1, m_quad.x2);
	
  if (m_quad.y1 > m_quad.y2)
    std::swap(m_quad.y1, m_quad.y2);

	CDialog::OnOK();
}


/*void CDialogGoto::OnExpand() 
{
	if (! Parse())
    return;

  _double xc = (m_quad.x1 + m_quad.x2) / 2,
         xl = (m_quad.x2 - m_quad.x1) / 2,
         yc = (m_quad.y1 + m_quad.y2) / 2,
         yl = (m_quad.y2 - m_quad.y1) / 2;

  m_quad.x1 = xc - 1.2 * xl;
  m_quad.x2 = xc + 1.2 * xl;
  m_quad.y1 = yc - 1.2 * yl;
  m_quad.y2 = yc + 1.2 * yl;

  Assign();
}

void CDialogGoto::OnShrink() 
{
	if (! Parse())
    return;
	
  _double xc = (m_quad.x1 + m_quad.x2) / 2,
         xl = (m_quad.x2 - m_quad.x1) / 2,
         yc = (m_quad.y1 + m_quad.y2) / 2,
         yl = (m_quad.y2 - m_quad.y1) / 2;

  m_quad.x1 = xc - xl / 1.2;
  m_quad.x2 = xc + xl / 1.2;
  m_quad.y1 = yc - yl / 1.2;
  m_quad.y2 = yc + yl / 1.2;

  Assign();
}
*/

void CDialogGoto::Assign()
{
  m_sx1 = Convert(m_quad.x1);
  m_sx2 = Convert(m_quad.x2);
  m_sy1 = Convert(m_quad.y1);
  m_sy2 = Convert(m_quad.y2);
	UpdateData(0);
}

bool CDialogGoto::Parse()
{
	UpdateData();

  if (! Convert(m_sx1, IDC_X1, m_quad.x1) ||
      ! Convert(m_sx2, IDC_X2, m_quad.x2) ||
      ! Convert(m_sy1, IDC_Y1, m_quad.y1) ||
      ! Convert(m_sy2, IDC_Y2, m_quad.y2))
    return false;

  return true;
}

CString CDialogGoto::Convert(_double v)
{
  CString r;
  int n;

  r.Format("%.15lf", v);
  n = r.GetLength();

  while (n > 1 && 
         r[n-1] == '0' &&
         r[n-2] != '.') n--;
  return r.Left(n);
}

bool CDialogGoto::Convert(CString val, uint id, _double & v)
{
  int n;
  UINT eID;

  val.TrimLeft();
  val.TrimRight();
  
  if (val == "")
  {
    eID = IDS_E_GOTO_EMPTY;
    goto failed;
  }

  if (sscanf(val, "%lf%n", &v, &n) != 1 ||
      n != val.GetLength())
  {
    eID = IDS_E_GOTO_SSCANF;
    goto failed;
  }

  return true;
    
failed:

  GetDlgItem(id)->SetFocus();
  AfxMessageBox(eID);
  return false;
}

void CDialogGoto::OnOptions() 
{
  CMenu menu, * popup;
  CRect r;
  int   cmd;

  GetDlgItem(IDC_OPTIONS)->GetWindowRect(&r);

  //
  menu.LoadMenu(IDR_POPUP);
  popup = menu.GetSubMenu(1);

  //
  cmd = popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
                              r.right, r.top, this);

  if (! Parse())
    return;
	
  _double xc = (m_quad.x1 + m_quad.x2) / 2,
         xl = (m_quad.x2 - m_quad.x1) / 2,
         yc = (m_quad.y1 + m_quad.y2) / 2,
         yl = (m_quad.y2 - m_quad.y1) / 2;

  //
  if (ID_GOTO_EXP_5 <= cmd && cmd <= ID_GOTO_EXP_50 ||
      ID_GOTO_CON_5 <= cmd && cmd <= ID_GOTO_CON_50)
  {
	  static _double factor[] = { 1.05, 1.10, 1.20, 1.50,
                               1/1.05, 1/1.10, 1/1.20, 1/1.50 };

    if (ID_GOTO_EXP_5 <= cmd && cmd <= ID_GOTO_EXP_50)
      cmd -= ID_GOTO_EXP_5;
    else
      cmd -= ID_GOTO_CON_5 - 4;

    m_quad.x1 = xc - xl * factor[cmd];
    m_quad.x2 = xc + xl * factor[cmd];
    m_quad.y1 = yc - yl * factor[cmd];
    m_quad.y2 = yc + yl * factor[cmd];
  }

  //
  if (cmd == ID_GOTO_ASPECT_MIN)
  {
    cmd = (abs(xl) < abs(yl)) ? ID_GOTO_ASPECT_X : ID_GOTO_ASPECT_Y;
  }

  //
  if (cmd == ID_GOTO_ASPECT_X)
  {
    m_quad.x1 = xc - yl;
    m_quad.x2 = xc + yl;
  }
  else
  if (cmd == ID_GOTO_ASPECT_Y)
  {
    m_quad.y1 = yc - xl;
    m_quad.y2 = yc + xl;
  }

  Assign();
}
