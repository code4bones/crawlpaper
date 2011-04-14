/*
	WallBrowserTwoParamsDlg.cpp
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
#include "WallBrowserTwoParamsDlg.h"
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

BEGIN_MESSAGE_MAP(CWallBrowserTwoParamsDlg,CDialog)
	ON_BN_CLICKED(IDOK,OnOK) 
	ON_WM_VSCROLL()
	ON_CBN_KILLFOCUS(IDC_COMBO_SUBVALUE,OnKillFocusCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SUBVALUE,OnSelChangeCombo)
END_MESSAGE_MAP()

/*
	DoDataExchange()
*/
void CWallBrowserTwoParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_VALUE,m_nFirstValue);
	DDV_MinMaxInt(pDX,m_nFirstValue,m_nFirstMin,m_nFirstMax);	
	DDX_Text(pDX,IDC_EDIT_SUBVALUE,m_nSecondValue);
	DDV_MinMaxInt(pDX,m_nSecondValue,m_nSecondMin,m_nSecondMax);	
	DDX_Control(pDX,IDC_COMBO_SUBVALUE,m_CComboSubValue);
}

/*
	CWallBrowserTwoParamsDlg()
*/
CWallBrowserTwoParamsDlg::CWallBrowserTwoParamsDlg(CString strProperty,CString strOperation,CString strSubOperation,int nFirstDefaultValue, int nFirstMin, int nFirstMax,int nSecondDefaultValue,int nSecondMin, int nSecondMax) : CDialog(IDD_DIALOG_TWOPARAMS)
{
	m_strProperty = strProperty;
	m_strOperation = strOperation;
	m_strSubOperation = strSubOperation;
	
	m_nFirstValue = m_nFirstDefaultValue = nFirstDefaultValue;
	m_nFirstMin = nFirstMin; 
	m_nFirstMax = nFirstMax;
	
	m_nSecondValue = m_nSecondDefaultValue = nSecondDefaultValue; 
	m_nSecondMin = nSecondMin; 
	m_nSecondMax = nSecondMax;

	m_bHaveCombo = TRUE;
}

/*
	OnInitDialog()
*/
BOOL CWallBrowserTwoParamsDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	SetDlgItemText(IDC_STATIC_PROPERTY,m_strProperty);

	CSpinButtonCtrl* pSpinFirst = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_VALUE);
	pSpinFirst->SetRange(m_nFirstMin,m_nFirstMax); 
	pSpinFirst->SetPos(m_nFirstDefaultValue);
	pSpinFirst->SetBuddy(GetDlgItem(IDC_EDIT_VALUE));

	if(m_strSubOperation=="Halftone")
	{
		m_CComboSubValue.AddString("print");		// HT_PRINT
		m_CComboSubValue.AddString("display");		// HT_VIEW
	}
	else if(m_strSubOperation=="Emboss")
	{
		m_CComboSubValue.AddString("north");		// EMBOSS_N
		m_CComboSubValue.AddString("north/east");	// EMBOSS_NE
		m_CComboSubValue.AddString("east");		// EMBOSS_E
		m_CComboSubValue.AddString("south/east");	// EMBOSS_SE
		m_CComboSubValue.AddString("south");		// EMBOSS_S
		m_CComboSubValue.AddString("south/west");	// EMBOSS_SW
		m_CComboSubValue.AddString("west");		// EMBOSS_W
		m_CComboSubValue.AddString("north/west");	// EMBOSS_NW
	}
	else if(m_strSubOperation=="Noise")
	{
		m_CComboSubValue.AddString("all channels");	// NOISE_CHANNEL_MASTER
		m_CComboSubValue.AddString("red channel");	// NOISE_CHANNEL_RED
		m_CComboSubValue.AddString("green channel");	// NOISE_CHANNEL_GREEN
		m_CComboSubValue.AddString("blue channel");	// NOISE_CHANNEL_BLUE
	}
	else
		m_bHaveCombo = FALSE;

	CString str;
	str.Format("%s (%d/%d)",m_strOperation,m_nFirstMin,m_nFirstMax);
	SetDlgItemText(IDC_STATIC_OPERATION,str);

	if(m_bHaveCombo)
	{
		SetDlgItemText(IDC_STATIC_SUBOPERATION,m_strSubOperation);
		GetDlgItem(IDC_EDIT_SUBVALUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SUBVALUE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPIN_SUBVALUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SPIN_SUBVALUE)->ShowWindow(SW_HIDE);
		m_CComboSubValue.SetCurSel(0);
	}
	else
	{
		str.Format("%s (%d/%d)",m_strSubOperation,m_nSecondMin,m_nSecondMax);
		SetDlgItemText(IDC_STATIC_SUBOPERATION,str);
		GetDlgItem(IDC_COMBO_SUBVALUE)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_SUBVALUE)->ShowWindow(SW_HIDE);
		CSpinButtonCtrl *pSpinSecond = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SUBVALUE);
		pSpinSecond->SetRange(m_nSecondMin,m_nSecondMax); 
		pSpinFirst->SetPos(m_nSecondDefaultValue);
		pSpinSecond->SetBuddy(GetDlgItem (IDC_EDIT_SUBVALUE));
	}

	UpdateData(FALSE);

	return(TRUE); 
}

/*
	OnOK()
*/
void CWallBrowserTwoParamsDlg::OnOK(void)
{
	if(UpdateData(TRUE))
	{
		if(m_bHaveCombo)
		{
			if(strcmp(m_szComboSubValue,"print")==0)
				m_nSecondValue = HT_PRINT;
			else if(strcmp(m_szComboSubValue,"display")==0)
				m_nSecondValue = HT_VIEW;
			else if(strcmp(m_szComboSubValue,"north")==0)
				m_nSecondValue = EMBOSS_N;
			else if(strcmp(m_szComboSubValue,"north/east")==0)
				m_nSecondValue = EMBOSS_NE;
			else if(strcmp(m_szComboSubValue,"east")==0)
				m_nSecondValue = EMBOSS_E;
			else if(strcmp(m_szComboSubValue,"south/east")==0)
				m_nSecondValue = EMBOSS_SE;
			else if(strcmp(m_szComboSubValue,"south")==0)
				m_nSecondValue = EMBOSS_S;
			else if(strcmp(m_szComboSubValue,"south/west")==0)
				m_nSecondValue = EMBOSS_SW;
			else if(strcmp(m_szComboSubValue,"west")==0)
				m_nSecondValue = EMBOSS_W;
			else if(strcmp(m_szComboSubValue,"north/west")==0)
				m_nSecondValue = EMBOSS_NW;
			else if(strcmp(m_szComboSubValue,"all channels")==0)
				m_nSecondValue = NOISE_CHANNEL_MASTER;
			else if(strcmp(m_szComboSubValue,"red channel")==0)
				m_nSecondValue = NOISE_CHANNEL_RED;
			else if(strcmp(m_szComboSubValue,"green channel")==0)
				m_nSecondValue = NOISE_CHANNEL_GREEN;
			else if(strcmp(m_szComboSubValue,"blue channel")==0)
				m_nSecondValue = NOISE_CHANNEL_BLUE;
		}

		EndDialog(IDOK);
	}
}

/*
	OnKillFocusCombo()
*/
void CWallBrowserTwoParamsDlg::OnKillFocusCombo(void)
{
	m_CComboSubValue.GetWindowText(m_szComboSubValue,sizeof(m_szComboSubValue));
}

/*
	OnSelChangeCombo()
*/
void CWallBrowserTwoParamsDlg::OnSelChangeCombo(void)
{
	m_CComboSubValue.GetLBText(m_CComboSubValue.GetCurSel(),m_szComboSubValue);
}

/*
	OnVScroll()
*/
void CWallBrowserTwoParamsDlg::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	if(nSBCode!=SB_ENDSCROLL)
	{
		CString strValue;
		strValue.Format("%d",(int)nPos);
		((CSpinButtonCtrl*)pScrollBar)->GetBuddy()->SetWindowText(strValue);
		((CSpinButtonCtrl*)pScrollBar)->SetPos((int)nPos);
	}

	CDialog::OnVScroll(nSBCode,nPos,pScrollBar);
}
