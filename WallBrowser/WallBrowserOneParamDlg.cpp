/*
	WallBrowserOneParamDlg.cpp
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
#include "WallBrowserOneParamDlg.h"
#include "CImageParams.h"
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

BEGIN_MESSAGE_MAP(CWallBrowserOneParamDlg,CDialog)
	ON_BN_CLICKED(IDOK,OnOK) 
	ON_EN_CHANGE(IDC_EDIT_VALUE,OnChangeValue)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

/*
	DoDataExchange()
*/
void CWallBrowserOneParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Text(pDX,IDC_EDIT_VALUE,m_nValue);
	DDV_MinMaxDouble(pDX,m_nValue,m_nMin,m_nMax);	
	
	DDX_Control(pDX,IDC_SLIDER_VALUE,m_ctrlSlider);
}

/*
	CWallBrowserOneParamDlg()
*/
CWallBrowserOneParamDlg::CWallBrowserOneParamDlg(CString strProperty,CString strOperation,double nDefaultValue/*0.0*/,double nMin/*0.0*/,double nMax/*100.0*/) : CDialog(IDD_DIALOG_ONEPARAM)
{
	m_strProperty = strProperty;
	m_strOperation = strOperation;
	m_nValue = m_nDefaultValue = nDefaultValue;
	m_nMin = nMin;
	m_nMax = nMax;
}

/*
	OnInitDialog()
*/
BOOL CWallBrowserOneParamDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);
	
	SetDlgItemText(IDC_STATIC_PROPERTY,m_strProperty);
	SetDlgItemText(IDC_STATIC_OPERATION,m_strOperation);

	CSpinButtonCtrl *pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_VALUE);
	pSpin->SetRange((int)m_nMin,(int)m_nMax); 
	pSpin->SetPos((int)m_nDefaultValue);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_VALUE));

	m_ctrlSlider.SetRangeMin((int)m_nMin,TRUE);
	m_ctrlSlider.SetRangeMax((int)m_nMax,TRUE);
	m_ctrlSlider.SetPos((int)m_nDefaultValue);

	CString strValue;
	strValue.Format("%d",(int)m_nMin);
	SetDlgItemText(IDC_STATIC_RANGE_MIN,strValue);
	strValue.Format("%d",(int)m_nMax);
	SetDlgItemText(IDC_STATIC_RANGE_MAX,strValue);

	UpdateData(FALSE);

	return(TRUE); 
}

/*
	OnOK()
*/
void CWallBrowserOneParamDlg::OnOK(void)
{
	if(UpdateData(TRUE))
		if(m_nValue >= m_nMin && m_nValue <= m_nMax)
			EndDialog(IDOK);
}

/*
	OnVScroll()
*/
void CWallBrowserOneParamDlg::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	if(nSBCode!=SB_ENDSCROLL)
	{
		CString strValue;
		strValue.Format("%d",(int)nPos);
		((CSpinButtonCtrl*)pScrollBar)->GetBuddy()->SetWindowText(strValue);
		((CSpinButtonCtrl*)pScrollBar)->SetPos((int)nPos);
		m_nValue = (double)nPos;
		UpdateData(FALSE);
	}

	CDialog::OnVScroll(nSBCode,nPos,pScrollBar);
}

/*
	OnHScroll()
*/
void CWallBrowserOneParamDlg::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar) 
{
	if(m_nValue!=(double)m_ctrlSlider.GetPos())
	{
		m_nValue = (double)m_ctrlSlider.GetPos();
		UpdateData(FALSE);	
	}

	CDialog::OnHScroll(nSBCode,nPos,pScrollBar);
}

/*
	OnChangeValue()
*/
void CWallBrowserOneParamDlg::OnChangeValue(void)
{
	UpdateData(TRUE);
	m_ctrlSlider.SetPos((int)m_nValue);
	UpdateData(FALSE);
}
