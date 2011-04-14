/*
	WallBrowserSizeDlg.cpp
	Dialogo per le opzioni.
	Luca Piergentili, 30/09/01
	lpiergentili@yahoo.com

	WallBrowser - the smart picture browser
	http://www.crawlpaper.com/
	copyright © 1998-2004 Luca Piergentili, all rights reserved
	crawlpaper is a registered name, all rights reserved
	
	This is a free software, released under the terms of the BSD license. Do not
	attempt to use it in any form which violates the license or you will be persecuted
	and charged for this.

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer. 

	- Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or other
	materials provided with the distribution. 

	- Neither the name of "crawlpaper" nor the names of its contributors may be used
	to endorse or promote products derived from this software without specific prior
	written permission. 

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
	OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "CImageParams.h"
#include "WallBrowserSizeDlg.h"
#include "resource.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWallBrowserSizeDlg,CDialog)
	ON_BN_CLICKED(IDOK,OnOK) 
	ON_WM_VSCROLL()
	ON_CBN_KILLFOCUS(IDC_COMBO_SUBVALUE,OnKillFocusCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SUBVALUE,OnSelChangeCombo)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHT,OnChangeHeight)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH,OnChangeWidth)
END_MESSAGE_MAP()

/*
	CWallBrowserSizeDlg
*/
CWallBrowserSizeDlg::CWallBrowserSizeDlg(CImage* pCImage,CImageParams* pCImageParams) : CDialog(IDD_DIALOG_SIZE)
{ 
	m_pCImage = pCImage;
	m_pCImageParams = pCImageParams;
	m_pCImageParams->Reset();
	pCImage->SetImageParamsMinMax(m_pCImageParams);
	pCImage->SetImageParamsDefaultValues(m_pCImageParams);
	
	m_nImgWidth = pCImage->GetWidth();
	m_nImgHeight = pCImage->GetHeight();
	m_pCImageParams->SetWidth((int)m_nImgWidth);
	m_pCImageParams->SetHeight((int)m_nImgHeight);
	
	m_nWidth = (int) m_nImgWidth;
	m_nHeight = (int) m_nImgHeight;
}

/*
	DoDataExchange()
*/
void CWallBrowserSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_WIDTH,m_pCImageParams->GetWidthByRef());
	DDV_MinMaxInt(pDX,m_pCImageParams->GetWidth(),m_pCImageParams->GetWidthMin(),m_pCImageParams->GetWidthMax());
	DDX_Text(pDX,IDC_EDIT_HEIGHT,m_pCImageParams->GetHeightByRef());
	DDV_MinMaxInt(pDX,m_pCImageParams->GetHeight(),m_pCImageParams->GetHeightMin(),m_pCImageParams->GetHeightMax());
	DDX_Control(pDX,IDC_CHECK_SIZE,m_btnRatio);
	DDX_Control(pDX,IDC_COMBO,m_ctrlCombo);

	if((m_pCImageParams->GetFlagsImage() & FLAG_RADIUS))
	{
		DDX_Text(pDX,IDC_EDIT_RADIUS,m_pCImageParams->GetRadiusByRef());
		DDV_MinMaxDouble(pDX,m_pCImageParams->GetRadius(),m_pCImageParams->GetRadiusMin(),m_pCImageParams->GetRadiusMax());
	}
/*	if((m_pCImageParams->GetFlagsImage() & FLAG_SIZEFILTER))
	{
		DDX_Text(pDX,IDC_EDIT_SIZEFILTER,m_pCImageParams->GetSizeFilterByRef());
		DDV_MinMaxInt(pDX,m_pCImageParams->GetSizeFilter(),m_pCImageParams->GetSizeFilterMin(),m_pCImageParams->GetSizeFilterMax());		
	}
	if((m_pCImageParams->GetFlagsImage() & FLAG_SIZEQUALITYCONTROL))
	{
		DDX_Text(pDX,IDC_EDIT_SIZEFILTER,m_pCImageParams->GetSizeQualityControlByRef());
		DDV_MinMaxInt(pDX,m_pCImageParams->GetSizeQualityControl(),m_pCImageParams->GetSizeQualityControlMin(),m_pCImageParams->GetSizeQualityControlMax());		
	}*/
}

/* 
	OnInitDialog()
*/ 
BOOL CWallBrowserSizeDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	GetDlgItem(IDC_EDIT_RADIUS)->EnableWindow((m_pCImageParams->GetFlagsImage() & FLAG_RADIUS) ? TRUE : FALSE);

	// Height
	GetDlgItem(IDC_SPIN_HEIGHT)->EnableWindow(TRUE);
	m_pSpinHeight = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_HEIGHT);
	m_pSpinHeight->SetRange((int)m_pCImageParams->GetHeightMin(),(int)m_pCImageParams->GetHeightMax()); 
	m_pSpinHeight->SetPos((int)m_pCImage->GetHeight());
	m_pSpinHeight->SetBuddy(GetDlgItem (IDC_EDIT_HEIGHT));
	m_pCImageParams->SetHeight((int)m_pCImage->GetHeight());

	//Width
	GetDlgItem(IDC_SPIN_WIDTH)->EnableWindow(TRUE);
	m_pSpinWidth = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_WIDTH);
	m_pSpinWidth->SetRange((int)m_pCImageParams->GetWidthMin(),(int)m_pCImageParams->GetWidthMax()); 
	m_pSpinWidth->SetPos((int)m_pCImage->GetWidth());
	m_pSpinWidth->SetBuddy(GetDlgItem(IDC_EDIT_WIDTH));
	m_pCImageParams->SetWidth((int)m_pCImage->GetWidth());

	m_btnRatio.EnableWindow(TRUE);
	m_btnRatio.SetCheck(TRUE);
	
	//SPIN RADIUS
	if((m_pCImageParams->GetFlagsImage() & FLAG_RADIUS))
    	{
		GetDlgItem(IDC_SPIN_RADIUS)->EnableWindow(TRUE);
		CSpinButtonCtrl *pSpinRadius = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_RADIUS);
		pSpinRadius->SetRange((int)m_pCImageParams->GetRadiusMin(),(int)m_pCImageParams->GetRadiusMax());
		pSpinRadius->SetPos((int)m_pCImageParams->GetRadius());
		pSpinRadius->SetBuddy(GetDlgItem (IDC_EDIT_RADIUS));
		
		CString rango;
		rango.Format("(%d/%d)",(int)m_pCImageParams->GetRadiusMin(),(int)m_pCImageParams->GetRadiusMax());
		SetDlgItemText(IDC_STATIC_RANGE_RADIUS,rango);
	}

	//SPIN SIZEFILTER
	if((m_pCImageParams->GetFlagsImage() & FLAG_SIZEFILTER))
    	{
		m_ctrlCombo.AddString("box");
		m_ctrlCombo.AddString("bilinear");
		m_ctrlCombo.AddString("gaussian");
		m_ctrlCombo.AddString("hamming");
		m_ctrlCombo.AddString("blackman");
		m_ctrlCombo.SetCurSel(0);
	}
	
	//SPIN SIZEQUALITYCONTROL
	if((m_pCImageParams->GetFlagsImage() & FLAG_SIZEQUALITYCONTROL))
    	{
		m_ctrlCombo.AddString("normal");
		m_ctrlCombo.AddString("favorblack");
		m_ctrlCombo.AddString("resample (interpolation)");
		m_ctrlCombo.AddString("favorblack & resample (interpolation)");
		m_ctrlCombo.SetCurSel(0);
	}
	
	UpdateData(FALSE);
	
	return(TRUE); 
}

/*
	OnOK()
*/
void CWallBrowserSizeDlg::OnOK(void)
{
	UpdateData(TRUE);

	CString strWidth,strHeight;
	GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(strWidth);
	GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(strHeight);
	int nWidth = atoi(strWidth);
	int nHeight = atoi(strHeight);
	
	if(nWidth  >= m_pCImageParams->GetWidthMin()  &&
	   nWidth  <= m_pCImageParams->GetWidthMax()  && 
	   nHeight >= m_pCImageParams->GetHeightMin() &&
	   nHeight <= m_pCImageParams->GetHeightMax())
	{
		BOOL bOk = TRUE;
		int nRadio = -1;
		int nFilter = -1;
		int nQuality = -1;
		
		if((m_pCImageParams->GetFlagsImage() & FLAG_RADIUS) && bOk)
    		{
			CString strRadio;
			GetDlgItem(IDC_EDIT_RADIUS)->GetWindowText(strRadio);
			nRadio = atoi(strRadio);	
			if(!(nRadio >= m_pCImageParams->GetRadiusMin() && nRadio <= m_pCImageParams->GetRadiusMax()))
				bOk = FALSE;
		}

		if((m_pCImageParams->GetFlagsImage() & FLAG_SIZEFILTER) && bOk)
    		{

			nFilter = m_pCImageParams->GetSizeFilter();
			if(!(nFilter >= m_pCImageParams->GetSizeFilterMin() && nFilter <= m_pCImageParams->GetSizeFilterMax()))
				bOk = FALSE;
		}

		if((m_pCImageParams->GetFlagsImage() & FLAG_SIZEQUALITYCONTROL) && bOk)
    		{
			nQuality = m_pCImageParams->GetSizeFilter();
			if(!(nQuality >= m_pCImageParams->GetSizeQualityControlMin() && nQuality <= m_pCImageParams->GetSizeQualityControlMax()))
				bOk = FALSE;
		}

		if(bOk)	
		{
			m_pCImageParams->SetWidth(nWidth);	
			m_pCImageParams->SetHeight(nHeight);
			
			if(nRadio!=-1)
				m_pCImageParams->SetRadius(nRadio);	
			if(nFilter!=-1)
				m_pCImageParams->SetSizeFilter(nFilter);
			if(nQuality!=-1)
				m_pCImageParams->SetSizeQualityControl(nQuality);
			
			EndDialog(IDOK);
		}
	}
}

/*
	OnKillFocusCombo()
*/
void CWallBrowserSizeDlg::OnKillFocusCombo(void)
{
	m_ctrlCombo.GetWindowText(m_szCombo,sizeof(m_szCombo));

	if(strcmp(m_szCombo,"box")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_BOX);
	else if(strcmp(m_szCombo,"bilinear")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_BILINEAR);
	else if(strcmp(m_szCombo,"gaussian")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_GAUSSIAN);
	else if(strcmp(m_szCombo,"hamming")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_HAMMING);
	else if(strcmp(m_szCombo,"blackman")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_BLACKMAN);
	else if(strcmp(m_szCombo,"normal")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_NORMAL);
	else if(strcmp(m_szCombo,"favorblack")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_FAVORBLACK);
	else if(strcmp(m_szCombo,"resample (interpolation)")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_RESAMPLE);
	else if(strcmp(m_szCombo,"favorblack & resample (interpolation)")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_FAVORBLACK_OR_RESAMPLE);
}

/*
	OnSelChangeCombo()
*/
void CWallBrowserSizeDlg::OnSelChangeCombo(void)
{
	m_ctrlCombo.GetLBText(m_ctrlCombo.GetCurSel(),m_szCombo);

	if(strcmp(m_szCombo,"box")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_BOX);
	else if(strcmp(m_szCombo,"bilinear")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_BILINEAR);
	else if(strcmp(m_szCombo,"gaussian")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_GAUSSIAN);
	else if(strcmp(m_szCombo,"hamming")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_HAMMING);
	else if(strcmp(m_szCombo,"blackman")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_BLACKMAN);
	else if(strcmp(m_szCombo,"normal")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_NORMAL);
	else if(strcmp(m_szCombo,"favorblack")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_FAVORBLACK);
	else if(strcmp(m_szCombo,"resample (interpolation)")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_RESAMPLE);
	else if(strcmp(m_szCombo,"favorblack & resample (interpolation)")==0)
		m_pCImageParams->SetSizeFilter(RESIZE_FAVORBLACK_OR_RESAMPLE);
}

/*
	OnVScroll()
*/
void CWallBrowserSizeDlg::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	if(nSBCode==SB_ENDSCROLL)
		return;

	if(pScrollBar->GetDlgCtrlID()==IDC_SPIN_HEIGHT)
		if(m_btnRatio.GetCheck())
			HeightRatio((double)nPos);
	
	if(pScrollBar->GetDlgCtrlID()==IDC_SPIN_WIDTH)
		if(m_btnRatio.GetCheck())
			WidthRatio((double)nPos);

	CString strValue;
	strValue.Format("%d",(int)nPos);
	((CSpinButtonCtrl*) pScrollBar)->GetBuddy()->SetWindowText(strValue);
	((CSpinButtonCtrl*) pScrollBar)->SetPos((int)nPos);

	UpdateData(TRUE);
}

/*
	OnChangeWidth()
*/
void CWallBrowserSizeDlg::OnChangeWidth(void)
{
	CString nWidth;
	GetDlgItem(IDC_EDIT_WIDTH)->GetWindowText(nWidth);
	if(m_btnRatio.GetCheck() && atoi(nWidth)!=m_nWidth)
		WidthRatio((double)atoi(nWidth));
}

/*
	OnChangeHeight()
*/
void CWallBrowserSizeDlg::OnChangeHeight(void)
{
	CString nHeight;
	GetDlgItem(IDC_EDIT_HEIGHT)->GetWindowText(nHeight);
	if(m_btnRatio.GetCheck() && atoi(nHeight)!= m_nHeight)
		HeightRatio((double)atoi(nHeight));
}
/*
	WidthRatio()
*/
void CWallBrowserSizeDlg::WidthRatio(double nWidth)
{
	m_pSpinWidth->SetPos((int)nWidth);
	
	double nRatio = m_nImgHeight / m_nImgWidth;
	double nHeight = nWidth * nRatio;
	
	int nHeightNew;
	if((nHeight - (int)nHeight) > 0.5)
		nHeightNew = (int)nHeight + 1;
	else
		nHeightNew = (int)nHeight;

	CString strValue;
	strValue.Format("%d",(int)nHeightNew);
	m_pSpinHeight->SetPos((int)nHeightNew);
	(m_pSpinHeight->GetBuddy())->SetWindowText(strValue);		

	strValue.Format("%d",(int)nWidth);
	(m_pSpinWidth->GetBuddy())->SetWindowText(strValue);	
	
	m_nWidth = (int)nWidth;
	m_nHeight = (int)nHeightNew;
}


/*
	HeightRatio()
*/
void CWallBrowserSizeDlg::HeightRatio(double nHeight)
{	
	m_pSpinHeight->SetPos((int)nHeight);

	double nRatio = m_nImgWidth / m_nImgHeight;
	double nWidth = nHeight * nRatio;

	int nWidthNew;
	if((nWidth - (int)nWidth) > 0.5)
		nWidthNew = (int)nWidth + 1;
	else
		nWidthNew = (int)nWidth;

	CString strValue;
	strValue.Format("%d",(int)nWidthNew);
	m_pSpinWidth->SetPos((int)nWidthNew);
	(m_pSpinWidth->GetBuddy())->SetWindowText(strValue);	

	strValue.Format("%d",(int)nHeight);
	(m_pSpinHeight->GetBuddy())->SetWindowText(strValue);		

	m_nWidth = nWidthNew;
	m_nHeight = nHeight;
}
