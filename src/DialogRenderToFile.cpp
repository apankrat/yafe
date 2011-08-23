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
#include "DialogRenderToFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// CDialogRenderToFile dialog
//

CDialogRenderToFile::CDialogRenderToFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRenderToFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRenderToFile)
	//}}AFX_DATA_INIT
  m_pFractal = 0;
  m_szImage = CSize(0,0);
  m_hThread = 0;
  m_bCancel = false;
  m_iLine = 0;
}


void CDialogRenderToFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRenderToFile)
	DDX_Control(pDX, IDC_PROGRESS, m_cProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRenderToFile, CDialog)
	//{{AFX_MSG_MAP(CDialogRenderToFile)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CDialogRenderToFile::OnInitDialog() 
{
  CDialog::OnInitDialog();
	
  DWORD dummy;
  
  assert(m_pFractal);
  assert(m_pPaletteIn && m_pPaletteOut);
  assert(m_szImage.cx && m_szImage.cy);

  m_hFile = BitmapFileInit(m_szImage.cx, m_sFilename);
  if (m_hFile == 0)
  {
    AfxMessageBox(IDS_E_RTF_FILE);
    EndDialog(IDCANCEL);
    return FALSE;
  }

  m_cProgress.SetRange32(0, m_szImage.cy);
  m_hThread = CreateThread(0,0,RenderThreadProxy,this,0,&dummy);

  SetTimer(0x1234,125,0);

  return TRUE;
}

void CDialogRenderToFile::OnCancel() 
{
  m_bCancel = true;
  WaitForSingleObject(m_hThread, -1);

  BitmapFileDone(m_hFile);
  DeleteFile("c:\\temp\\a.bmp");
  
  CDialog::OnCancel();
}

void CDialogRenderToFile::OnTimer(UINT nIDEvent) 
{
  CDialog::OnTimer(nIDEvent);

  m_cProgress.SetPos(m_iLine);

  if (m_iLine == m_szImage.cy-1)
  {
    BitmapFileDone(m_hFile);	
    EndDialog(IDOK);
  }
}

bool CDialogRenderToFile::RcbLine(uint w, uint h, uint y, PixelType * line)
{
  assert(w == m_szImage.cx && h == m_szImage.cy);

  {
    static COLORREF col[65536];
    uint x;

    for (x=0; x<w; x++)
    {
      _double f = (line[x] & PixelMax) / (_double)PixelMax;

      if (line[x] & PixelInside)
      {
        col[x] = m_pPaletteIn->Color(f);
      }
      else
      {
        col[x] = m_pPaletteOut->Color(f);
      }
    }

    BitmapFileLine(m_hFile, col);
  }
  
  m_iLine = y;        // yeah yeah ... need to synchronize, i know
  return !m_bCancel;
}

DWORD WINAPI CDialogRenderToFile::RenderThreadProxy(void * p)
{
  CDialogRenderToFile * dlg = (CDialogRenderToFile*)p;
  dlg->m_pFractal->Render(dlg);
  return 0;
}

