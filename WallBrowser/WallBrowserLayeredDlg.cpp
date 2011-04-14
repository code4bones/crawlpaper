/*
	WallBrowserLayeredDlg.cpp
	Dialogo per la trasparenza.
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
#include "CWndLayered.h"
#include "WallBrowserConfig.h"
#include "WallBrowserLayeredDlg.h"
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

BEGIN_MESSAGE_MAP(CWallBrowserLayeredDlg,CDialog)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDOK,OnOK)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_BN_CLICKED(IDC_CHECK_LAYERED,OnCheckLayered)
	ON_EN_CHANGE(IDC_EDIT_LAYERED,OnEnChangeLayered)
END_MESSAGE_MAP()

/*
	DoDataExchange()
*/
void CWallBrowserLayeredDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX,IDC_CHECK_LAYERED,m_bLayered);
	DDX_Text(pDX,IDC_EDIT_LAYERED,m_nLayered);
}

/*
	CWallBrowserLayeredDlg()
*/
CWallBrowserLayeredDlg::CWallBrowserLayeredDlg(CWnd* pParent,CWallBrowserConfig* pConfig) : CDialog(IDD_DIALOG_TRANSPARENCY)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// imposta con i valori della configurazione
	m_bLayered = m_pConfig->GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYERED_KEY);
	m_nLayered = m_pConfig->GetNumber(WALLBROWSER_OPTIONS_KEY,WALLBROWSER_LAYEREDVALUE_KEY);
}

/*
	OnInitDialog()
*/
BOOL CWallBrowserLayeredDlg::OnInitDialog(void)
{
	// classe base
	CDialog::OnInitDialog();

	// imposta lo spin button per la trasparenza
	CSpinButtonCtrl* pSpin;
	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_LAYERED);
	pSpin->SetRange(LWA_ALPHA_MIN,LWA_ALPHA_MAX);
	pSpin->SetPos(m_nLayered);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_LAYERED));

	// abilita i controlli
	GetDlgItem(IDC_EDIT_LAYERED)->EnableWindow(m_bLayered);
	GetDlgItem(IDC_SPIN_LAYERED)->EnableWindow(m_bLayered);

	CString strLayered;
	strLayered.Format("(%d = invisible, %d = opaque)",LWA_ALPHA_MIN,LWA_ALPHA_MAX);
	SetDlgItemText(IDC_STATIC_LAYERED,strLayered);

	UpdateData(FALSE);

	return(TRUE);
}

/*
	OnOK()
*/
void CWallBrowserLayeredDlg::OnOK(void)
{
	if(UpdateData(TRUE))
		EndDialog(IDOK);
}

/*
	OnCancel()
*/
void CWallBrowserLayeredDlg::OnCancel(void)
{
	EndDialog(IDCANCEL);
}

/*
	OnVScroll()
*/
void CWallBrowserLayeredDlg::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	if(nSBCode!=SB_ENDSCROLL)
	{
		CString strValue;
		strValue.Format("%d",(int)nPos);
		((CSpinButtonCtrl*)pScrollBar)->GetBuddy()->SetWindowText(strValue);
		((CSpinButtonCtrl*)pScrollBar)->SetPos((int)nPos);
		UpdateData(TRUE);
	}
}

/*
	OnCheckLayered()
*/
void CWallBrowserLayeredDlg::OnCheckLayered(void)
{
	m_bLayered = !m_bLayered;

	GetDlgItem(IDC_EDIT_LAYERED)->EnableWindow(m_bLayered);
	GetDlgItem(IDC_SPIN_LAYERED)->EnableWindow(m_bLayered);
}

/*
	OnEnChangeLayered()
*/
void CWallBrowserLayeredDlg::OnEnChangeLayered(void)
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_LAYERED,strValue);
	int nValue = atoi(strValue);

	if(nValue < LWA_ALPHA_MIN || nValue > LWA_ALPHA_MAX)
	{
		strValue.Format("%d",DEFAULT_LAYEREDVALUE);
		m_nLayered = DEFAULT_LAYEREDVALUE;
		UpdateData(FALSE);
	}
}
