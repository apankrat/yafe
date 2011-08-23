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
#include "yafe.h"
#include "DialogRenderToFileOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogRenderToFileOptions dialog


CDialogRenderToFileOptions::CDialogRenderToFileOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRenderToFileOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRenderToFileOptions)
	m_sFilename = _T("");
	m_sFilesize = _T("");
	m_sSize = _T("");
	//}}AFX_DATA_INIT
}


void CDialogRenderToFileOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRenderToFileOptions)
	DDX_Text(pDX, IDC_FILENAME, m_sFilename);
	DDX_Text(pDX, IDC_FILESIZE, m_sFilesize);
	DDX_Text(pDX, IDC_IMAGE_SIZE, m_sSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRenderToFileOptions, CDialog)
	//{{AFX_MSG_MAP(CDialogRenderToFileOptions)
	ON_EN_CHANGE(IDC_IMAGE_SIZE, OnChangeSize)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogRenderToFileOptions message handlers

BOOL CDialogRenderToFileOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_sSize.Format("%u", m_nSize);
  UpdateData(0);
  OnChangeSize();
	
	return TRUE;
}

void CDialogRenderToFileOptions::OnChangeSize() 
{
  UpdateData();
  
  if (sscanf(m_sSize, "%u", &m_nSize) == 1 &&
      m_nSize)
  {
    uint fs = BitmapFileSize(m_nSize, m_nSize);

    if (fs > 1024*1024*1024) m_sFilesize.Format("%.2f GB", fs/1024./1024./1024.);
    else
    if (fs > 1024*1024) m_sFilesize.Format("%.2f MB", fs/1024./1024.);
    else
    if (fs > 1024) m_sFilesize.Format("%.2f KB", fs/1024.);
    else           m_sFilesize.Format("%.2f bytes", fs);
  }
  else
  {
    m_sFilesize = "";
  }	

  UpdateData(0);
}

void CDialogRenderToFileOptions::OnOK() 
{
  UpdateData();

  if (! m_sFilename.GetLength())
  {
    GetDlgItem(IDC_FILENAME)->SetFocus();
    AfxMessageBox(IDS_E_RTF_FILENAME);
    return;
  }

  if (! m_sSize.GetLength() ||
      sscanf(m_sSize, "%u", &m_nSize) != 1 ||
      m_nSize < 16)
  {
    GetDlgItem(IDC_IMAGE_SIZE)->SetFocus();
    AfxMessageBox(IDS_E_RTF_SIZE);
    return;
  }
	
	CDialog::OnOK();
}


void CDialogRenderToFileOptions::OnBrowse() 
{
  CFileDialog dlg(FALSE, "*.bmp", 0, OFN_OVERWRITEPROMPT, 
                  "Windows Bitmap Files (*.bmp)|*.bmp|"
                  "All Files (*.*)|*.*||", this);

  UpdateData();
  dlg.m_ofn.lpstrFile = (char*)(const char*)m_sFilename;

  if (dlg.DoModal() != IDOK)
    return;

  m_sFilename = dlg.GetPathName();
  UpdateData(0);
}
