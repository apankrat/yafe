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
#include "DialogMain.h"
#include "DialogAbout.h"
#include "DialogGoto.h"
#include "DialogRenderToFile.h"
#include "DialogRenderToFileOptions.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum WM_USER_EXTRAS_
{
  WMX_HISTORY_CLICK = WM_USER + 1,
  WMX_HISTORY_SCROLL,
  
  WMX_PICTURE_ZOOM,
  WMX_PICTURE_RCLICK,

  WMX_RENDER_PROGRESS,
  WMX_RENDER_COMPLETE,
};

//
CDialogMain::CDialogMain(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMain::IDD, pParent),
    m_layout(this)
{
	//{{AFX_DATA_INIT(CDialogMain)
	//}}AFX_DATA_INIT
  m_hRenderThread = 0;
  m_bRenderCancel = false;
  m_pFractal = 0;
  m_bAntialias = false;
  m_pPaletteIn = 0;
  m_pPaletteOut = 0;
}

void CDialogMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMain)
	DDX_Control(pDX, IDC_PROXY, m_cProxy);
	DDX_Control(pDX, IDC_HISTORY_UP, m_cHistoryUp);
	DDX_Control(pDX, IDC_HISTORY_DOWN, m_cHistoryDown);
	DDX_Control(pDX, IDC_HISTORY, m_cHistory);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDialogMain, CDialog)
	//{{AFX_MSG_MAP(CDialogMain)
	ON_BN_CLICKED(IDC_HISTORY_DOWN, OnHistoryDown)
	ON_BN_CLICKED(IDC_HISTORY_UP, OnHistoryUp)
	ON_WM_KEYUP()
	ON_WM_ACTIVATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WMX_HISTORY_CLICK,  OnHistoryClick)
	ON_MESSAGE(WMX_HISTORY_SCROLL, OnHistoryScroll)
	ON_MESSAGE(WMX_PICTURE_ZOOM,   OnPictureZoom)
	ON_MESSAGE(WMX_PICTURE_RCLICK, OnPictureRClick)
	ON_MESSAGE(WMX_RENDER_PROGRESS, OnRenderProgress)
	ON_MESSAGE(WMX_RENDER_COMPLETE, OnRenderComplete)
END_MESSAGE_MAP()

//
// CDialogMain message handlers
//
BOOL CDialogMain::OnInitDialog()
{
  CRect rWnd, rCli;
  int dy;
  
  CDialog::OnInitDialog();

  //
  SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
  SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

  //
  m_layout.Initialize(150, 100, TRUE);
  m_layout.AddControl(IDC_HISTORY_UP,   LME_LEFT | LME_RIGHT);
  m_layout.AddControl(IDC_HISTORY_DOWN, LME_ALL);
  m_layout.AddControl(IDC_HISTORY,      LME_LEFT | LME_RIGHT | LME_BOTTOM);

  m_layout.Remember();

  // adjust m_cProxy to 1:1 aspect ratio
  GetWindowRect(&rWnd);
  m_cProxy.GetWindowRect(&rCli);
  ScreenToClient(&rCli);

  dy = rCli.Height() - rCli.Width();

  rWnd.bottom -= dy;
  MoveWindow(&rWnd);

  rCli.bottom -= dy;
  m_cProxy.MoveWindow(&rCli);

  m_layout.AddControl(IDC_PROXY, LME_RIGHT | LME_BOTTOM | LME_CENTER | LMR_SIZE);
  m_layout.Remember();

  //
  m_cHistory.Initialize(WMX_HISTORY_CLICK, WMX_HISTORY_SCROLL);

  //
  m_cProxy.Initialize(WMX_PICTURE_ZOOM, WMX_PICTURE_RCLICK);
  m_cProxy.Resize();

  //
  m_colBack = RGB(0x1F,0x23,0x35);

  m_cProxy.GetClientRect(&rCli);
  WipeClean();

  OnHistoryScroll(0,0);

  m_pFractal = InstantiateMandelbrot();
  SetDefaultPalette();

  Render();
		
	return TRUE;
}

BOOL CDialogMain::DestroyWindow() 
{
  RenderCancel();	
	return CDialog::DestroyWindow();
}

BOOL CDialogMain::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult)
{
  BOOL bRes;
  
  if (m_layout.OnWndMsg(message, wParam, lParam, pResult, 1))
	return TRUE;

  switch (message)
  {
  case WM_ENTERSIZEMOVE:
    m_szBeforeSize = m_cProxy.m_szBitmap;
    break;

  case WM_SIZING:
    RenderCancel();
    break;
  
  case WM_EXITSIZEMOVE:  
    if (m_szBeforeSize != m_cProxy.m_szBitmap)
      Render(); 
    break;
  }

  bRes = CDialog::OnWndMsg(message, wParam, lParam, pResult);

  m_layout.OnWndMsg(message, wParam, lParam, pResult, 0);

  return bRes;
}

void CDialogMain::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  switch (nChar)
  {
  case 0x74:  // F5
    Render();
    break;

  case 0x41:  // 'a'
    {
      CClientDC dc(&m_cProxy);
      CRect r;

      m_cProxy.GetClientRect(&r);
      for (uint x=0, w=r.Width(); x<w; x++)
      {
        dc.FillSolidRect(x,0,1,10, m_pPaletteOut->Color(x/(_double)w));
      }
    }
    break;
  }

	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CDialogMain::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
  m_bActive = (nState != WA_INACTIVE);  	
}

//
// History panel
//
void CDialogMain::OnHistoryDown() 
{
  m_cHistory.Down();
}

void CDialogMain::OnHistoryUp() 
{
  m_cHistory.Up();
}

LRESULT CDialogMain::OnHistoryClick(WPARAM wp, LPARAM lp)
{
  const CHistoryItem & base = m_cHistory.Item(wp);
  CBounds   quad = m_pFractal->GetBounds();
  CRect     rCli;
  CBitmap * top;
  BITMAP    bmp;
  CPen      pen;
  bool      render = false;

  CHistoryItem item;

  //
  RenderCancel();

  top = m_cProxy.Capture();

  m_cProxy.GetClientRect(&rCli);

  base.image->GetBitmap(&bmp);
  render = (bmp.bmWidth != rCli.Width());

  pen.CreatePen(PS_SOLID, 1, RGB(0xFF,0x3F,0x1F));
  m_cProxy.m_dc.SelectObject(&pen);
  m_cProxy.m_dc.SelectStockObject(NULL_BRUSH);

  for (int i=0, n=15; i<=n; i++)
  {
    _double a = (n-i)/(_double)n;
    CRect r;

    BitmapBlt(&m_cProxy.m_dc, &rCli, base.image, 0);

    r.left   = rCli.left + rCli.Width() * (quad.x1 - base.quad.x1)/(base.quad.x2 - base.quad.x1);
    r.right  = rCli.left + rCli.Width() * (quad.x2 - base.quad.x1)/(base.quad.x2 - base.quad.x1);

    r.top    = rCli.top + rCli.Height() * (quad.y1 - base.quad.y1)/(base.quad.y2 - base.quad.y1);
    r.bottom = rCli.top + rCli.Height() * (quad.y2 - base.quad.y1)/(base.quad.y2 - base.quad.y1);

    r = r + (rCli - r) * a;

    BitmapBlt(&m_cProxy.m_dc, &r, top, 0);
    m_cProxy.m_dc.Rectangle(&r);

    m_cProxy.RedrawWindow();
    Sleep(5);
  }

  BitmapBlt(&m_cProxy.m_dc, &rCli, base.image, 0);
  Sleep(250);
  m_cProxy.RedrawWindow();

  while (m_cHistory.Size() > wp)
  {
    item = m_cHistory.Item(-1);
    m_cHistory.Pop();
  }

  //
  m_pFractal->SetBounds(item.quad);
  render |= (m_pPaletteIn->Id() != item.palIn ||
             m_pPaletteOut->Id() != item.palOut);

  delete top;

  if (render)
    Render();

  return 0;
}

LRESULT CDialogMain::OnHistoryScroll(WPARAM wp, LPARAM lp)
{
  m_cHistoryUp.EnableWindow(m_cHistory.CanScrollUp());
  m_cHistoryDown.EnableWindow(m_cHistory.CanScrollDown());
  return 0;
}

LRESULT CDialogMain::OnPictureZoom(WPARAM wp, LPARAM lp)
{
  CRect rSel = m_cProxy.m_rZoom, rCli;
  CPen  pen;

  CHistoryItem item;

  RenderCancel();

  item.quad   = m_pFractal->GetBounds();
  item.sel    = rSel;
  item.image  = m_cProxy.Capture();
  item.palIn  = m_pPaletteIn->Id();
  item.palOut = m_pPaletteOut->Id();
  
  m_cProxy.GetClientRect(&rCli);

  pen.CreatePen(PS_SOLID, 1, RGB(0xFF,0x3F,0x1F));
  m_cProxy.m_dc.SelectObject(&pen);
  m_cProxy.m_dc.SelectStockObject(NULL_BRUSH);

  for (int i=0, n=20; i<=n; i++)
  {
    _double a = i/(_double)n;
    CRect r;

    r = rSel + (rCli - rSel) * a;

    BitmapBlt(&m_cProxy.m_dc, &r, item.image, &rSel);

    if (i<10)
      m_cProxy.m_dc.Rectangle(&r);
    
    m_cProxy.RedrawWindow();
    Sleep(5);
  }

  m_cHistory.Push(item);

  {
    CBounds qOld = m_pFractal->GetBounds(), qNew;

    qNew.x1 = qOld.x1 + (rSel.left - rCli.left) * (qOld.x2 - qOld.x1) / rCli.Width();
    qNew.x2 = qOld.x1 + (rSel.right - rCli.left) * (qOld.x2 - qOld.x1) / rCli.Width();

    qNew.y1 = qOld.y1 + (rSel.top - rCli.top) * (qOld.y2 - qOld.y1) / rCli.Height();
    qNew.y2 = qOld.y1 + (rSel.bottom - rCli.top) * (qOld.y2 - qOld.y1) / rCli.Height();

    m_pFractal->SetBounds(qNew);
  }

  Render();

  return 0;
}

LRESULT CDialogMain::OnPictureRClick(WPARAM x, LPARAM y)
{
  CMenu menu, * popup;
  POINT pt = { x, y };
  int   cmd, cur;

  m_cProxy.ClientToScreen(&pt);

  //
  menu.LoadMenu(IDR_POPUP);
  popup = menu.GetSubMenu(0);

  //
  switch (m_pFractal->Id())
  {
  case eFractalLyapunov:   cmd = ID_FRACTAL_L; break;
  case eFractalMandelbrot: cmd = ID_FRACTAL_M; break;
  case eFractalNewton:     cmd = ID_FRACTAL_N; break;
  default: cmd = 0;
  }

  popup->CheckMenuRadioItem(ID_FRACTAL_L, ID_FRACTAL_N, cmd, MF_BYCOMMAND);
  cur = cmd;

  //
  switch (m_pFractal->GetQuality())
  {
  case eQualityDraft:  cmd = ID_QUALITY_DRAFT; break;
  case eQualityNormal: cmd = ID_QUALITY_NORMAL; break;
  case eQualityHigh:   cmd = ID_QUALITY_HIGH; break;
  default: cmd = 0;
  }

  popup->CheckMenuRadioItem(ID_QUALITY_DRAFT, ID_QUALITY_HIGH, cmd, MF_BYCOMMAND);

  //
  if (m_bAntialias)
    popup->CheckMenuItem(ID_ANTIALIAS, MF_CHECKED);

  //
  if (m_pPaletteIn->Id() == m_uPaletteIdIn &&
      m_pPaletteOut->Id() == m_uPaletteIdOut)
  
    popup->CheckMenuRadioItem(ID_PALETTE_DEFAULT, ID_PALETTE_DEFAULT, 
                              ID_PALETTE_DEFAULT, MF_BYCOMMAND);

  //
  if (m_hRenderThread)
  {
    popup->ModifyMenu(ID_RENDER, MF_BYCOMMAND, ID_RENDER, "Cancel &rendering");
  }

  //
  cmd = popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
                              pt.x, pt.y, this);

  //
  switch (cmd)
  {
  // -- fractal --
  case ID_FRACTAL_M:
  case ID_FRACTAL_N:
    if (cmd == cur)
      break;

    // fallthrough

  case ID_FRACTAL_L:
    {
      CFractal * fNew;
      bool       bPal = false;
      
      RenderCancel();
      switch (cmd)
      {
      case ID_FRACTAL_L: fNew = InstantiateLyapunov(); break;
      case ID_FRACTAL_M: fNew = InstantiateMandelbrot(); break;
      case ID_FRACTAL_N: fNew = InstantiateNewton(); break;
      }

      if (! fNew)
        break;

      if (fNew->Id() != m_pFractal->Id())
      {
        bPal = true;
      }
      else
      {
        fNew->SetQuality(m_pFractal->GetQuality());
      }

      delete m_pFractal;
      m_pFractal = fNew;

      if (bPal)
        SetDefaultPalette();

      WipeClean();
      Render();       
    }  
    break;    

  // -- quality --
  case ID_QUALITY_DRAFT:
    m_pFractal->SetQuality(eQualityDraft);
    Render();
    break;

  case ID_QUALITY_NORMAL:
    m_pFractal->SetQuality(eQualityNormal);
    Render();
    break;

  case ID_QUALITY_HIGH:
    m_pFractal->SetQuality(eQualityHigh);
    Render();
    break;

  case ID_ANTIALIAS:
    m_bAntialias = !m_bAntialias;
    Render();
    break;

  // -- palette --
  case ID_PALETTE_DEFAULT:

    RenderCancel();
    SetDefaultPalette();
    Render();
    break;

  case ID_PALETTE_IMPORT:

    RenderCancel();
    {
      CFileDialog dlg(TRUE, "*.bmp", 0, 0, 
                      "Fractint Color Map Files (*.map)|*.map|"
                      "Yafe Color Map Files (*.yaf)|*.yaf||",
                      this);

      CPalette * pIn, * pOut;
      COLORREF   c0;

      if (dlg.DoModal() != IDOK)
        break;

      pOut = PaletteFromMapFile(dlg.GetPathName());
      if (!pOut)
      {
        AfxMessageBox(IDS_E_IMPORT_PALETTE_MAP, MB_OK | MB_ICONSTOP);
        break;
      }

      pIn = PaletteLinear(false);
      c0  = pOut->Color(1/256.);
      pIn->Add(0.5, GetRValue(c0)/255., GetGValue(c0)/255., GetBValue(c0)/255.);

      SetPalette(pIn, pOut);
      Render();
    }
    break;

  // -- misc --
  case ID_RENDER:

    if (m_hRenderThread)
      RenderCancel();
    else
      Render();

    break;

  case ID_GOTO:
    {
      CDialogGoto d(this);

      d.m_quad = m_pFractal->GetBounds(); 

      if (d.DoModal() != IDOK)
        break;

      //
      CHistoryItem item;

      RenderCancel();

      item.quad   = m_pFractal->GetBounds();
      item.sel    = CRect();
      item.image  = m_cProxy.Capture();
      item.palIn  = m_pPaletteIn->Id();
      item.palOut = m_pPaletteOut->Id();
  
      m_cHistory.Push(item);

      m_pFractal->SetBounds(d.m_quad);

      Render();
    }
    break;

  case ID_SAVE_IMAGE: // aka RENDER_TO_FILE

    RenderCancel();

    {
      CDialogRenderToFileOptions dOptions(this);
      CDialogRenderToFile        dRender(this);

      dOptions.m_nSize = m_cProxy.m_szBitmap.cx;
      dOptions.m_sFilename = m_sFilename;

      if (dOptions.DoModal() != IDOK)
        break;

      m_sFilename = dOptions.m_sFilename;

      dRender.m_pFractal    = m_pFractal;
      dRender.m_pPaletteIn  = m_pPaletteIn;
      dRender.m_pPaletteOut = m_pPaletteOut;
      dRender.m_sFilename   = dOptions.m_sFilename;
      dRender.m_szImage.cx = 
      dRender.m_szImage.cy = dOptions.m_nSize;

      dRender.DoModal();
    }
    break;

  case ID_ABOUT:
    {
      CDialogAbout  dlg(this);
      dlg.DoModal();
    }
    break;
  }

  return 0;
}

//
void CDialogMain::WipeClean()
{
  CRect rCli;

  while (m_cHistory.Size())
    m_cHistory.Pop();
  
  m_cProxy.GetClientRect(&rCli);

  m_cProxy.m_dc.SetTextColor(RGB(0xFF,0xFF,0xFF));
  m_cProxy.m_dc.SelectObject(GetFont());

  m_cProxy.m_dc.FillSolidRect(&rCli, m_colBack);
  m_cProxy.m_dc.DrawText("Rendering, please wait ...", &rCli, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

  m_cProxy.Invalidate(0);
}

void CDialogMain::Render()
{
  DWORD dwDummy;

  RenderCancel();

  m_hRenderThread = CreateThread(0, 0, RenderThreadProxy, this, 0, &dwDummy);
}

void CDialogMain::RenderCancel()
{
  ASSERT(! m_bRenderCancel);

  if (! m_hRenderThread)
    return;
  
  m_bRenderCancel = true;

  WaitForSingleObject(m_hRenderThread, -1);
  Sleep(100);

  m_hRenderThread = 0;
  m_bRenderCancel = false;
  m_cProxy.RedrawWindow();
}

//
LRESULT CDialogMain::OnRenderProgress(WPARAM wp, LPARAM lp)
{
  if (m_hRenderThread == (HANDLE)lp)
  {
    CClientDC dc(&m_cProxy);
    uint      i = LOWORD(wp), n = HIWORD(wp);
    uint      w = m_cProxy.m_szBitmap.cx,
              x = (w-2)*i/n,
              y = m_cProxy.m_szBitmap.cy-4;

    COLORREF  colBack = RGB(0,0,32),
              colFore = RGB(63,255,127);

    dc.FillSolidRect(0,y+0,w,1, colBack);
    dc.FillSolidRect(0,y+3,w,1, colBack);
    dc.FillSolidRect(0,y+0,1,4, colBack);
    dc.FillSolidRect(1,y+1,x,2, colFore);
    dc.FillSolidRect(x+1,y+0,w-x-1,4, colBack);
  }  
  return 0;
}

LRESULT CDialogMain::OnRenderComplete(WPARAM wp, LPARAM lp)
{
  uint x, y,
       w = HIWORD(wp),
       h = LOWORD(wp);
  PixelType * data = (PixelType *)lp;
  PixelType * line = data;

  CBitmap * pTemp = 0;
  CDC     * pDC;
  
  if (w != m_cProxy.m_szBitmap.cx ||
      h != m_cProxy.m_szBitmap.cy)
  {
    pTemp = new CBitmap();
    pTemp->Attach(BitmapCreate(w,h));

    pDC = new CDC;
    pDC->CreateCompatibleDC(0);
    pDC->SelectObject(pTemp);
  }
  else
  {
    pDC = &m_cProxy.m_dc;
  }

  for (y=0; y<h; y++, line+=w)
    for (x=0; x<w; x++)
    {
      COLORREF col;
      _double f = (line[x] & PixelMax) / (_double)PixelMax;

      if (line[x] & PixelInside)
      {
        col = m_pPaletteIn->Color(f);
      }
      else
      {
        col = m_pPaletteOut->Color(f);
      }

      pDC->SetPixel(x,y,col);
    }

  delete [] data;

  if (pTemp)
  {
    CRect r(CPoint(0,0), m_cProxy.m_szBitmap);

    delete pDC;
  
    BitmapBlt(&m_cProxy.m_dc, &r, pTemp, 0);
    delete pTemp;
  }

  m_cProxy.Invalidate(0);

  if (! m_bActive)
  {
    FlashWindow(TRUE);
  }

  return 0;
}

//
void CDialogMain::RenderThread()
{
  m_pFractal->Render(this);
  m_hRenderThread = 0;
}

//
// RenderCallback
//
uint CDialogMain::RcbWidth()
{
  return m_cProxy.m_szBitmap.cx * (m_bAntialias ? 2.0 : 1.0);
}

uint CDialogMain::RcbHeight()
{
  return m_cProxy.m_szBitmap.cy * (m_bAntialias ? 2.0 : 1.0);
}

bool CDialogMain::RcbByLine()
{
  return false;
}

bool CDialogMain::RcbLine(uint w, uint h, uint y, PixelType *)
{
  PostMessage(WMX_RENDER_PROGRESS, MAKEWPARAM(y, h), (LPARAM)m_hRenderThread);
  return ! m_bRenderCancel;
}

void CDialogMain::RcbDone(uint w, uint h, PixelType * data)
{
  PostMessage(WMX_RENDER_COMPLETE, MAKEWPARAM(w,h), (LPARAM)data);
}

//
//  Misc methods
//
void CDialogMain::SetPalette(CPalette * in, CPalette * out)
{
  delete m_pPaletteIn;  m_pPaletteIn = in;
  delete m_pPaletteOut; m_pPaletteOut = out;
}

void CDialogMain::SetDefaultPalette()
{
  SetPalette(m_pFractal->DefaultPalette(true), m_pFractal->DefaultPalette(false));
  m_uPaletteIdIn = m_pPaletteIn->Id();
  m_uPaletteIdOut = m_pPaletteOut->Id();
}

//
// proxies
//
DWORD WINAPI CDialogMain::RenderThreadProxy(void * context)
{
  ((CDialogMain*)context)->RenderThread();
  return 0;
}



BOOL CDialogMain::OnEraseBkgnd(CDC* pDC) 
{
	return CDialog::OnEraseBkgnd(pDC);
}
