/*
	WallPaperDrawSettingsAlphaDlg.cpp
	Dialogo per la pagina relativa alle opzioni per le miniature.
	Luca Piergentili, 20/09/01
	lpiergentili@yahoo.com

	WallPaper (alias crawlpaper) - the hardcore of Windows desktop
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
#include "CWindowsVersion.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
#include "WallPaperDrawSettingsAlphaDlg.h"
#include "resource.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
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

BEGIN_MESSAGE_MAP(CWallPaperDrawSettingsAlphaDlg,CPropertyPage)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_CHECK_LAYERED,OnCheckLayered)
	ON_EN_CHANGE(IDC_EDIT_LAYERED,OnEnChangeLayered)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperDrawSettingsAlphaDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperDrawSettingsAlphaDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX,IDC_CHECK_LAYERED,m_bLayered);
	DDX_Text(pDX,IDC_EDIT_LAYERED,m_nLayered);
}

/*
	CreateEx()
*/
BOOL CWallPaperDrawSettingsAlphaDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// ricava la versione del sistema operativo
	m_enumOsVer = m_winVer.GetVersionType();

	// imposta con i valori della configurazione
	if(m_enumOsVer==WINDOWS_2000 || m_enumOsVer==WINDOWS_XP || m_enumOsVer==WINDOWS_VISTA || m_enumOsVer==WINDOWS_SEVEN)
	{
		m_bLayered = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY);
		m_nLayered = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY);
	}
	else
	{
		m_bLayered = FALSE;
		m_nLayered = 0;
	}

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperDrawSettingsAlphaDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// imposta lo spin button per la trasparenza
	CSpinButtonCtrl* pSpin;
	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_LAYERED);
	pSpin->SetRange(LWA_ALPHA_INVISIBLE,LWA_ALPHA_OPAQUE);
	pSpin->SetPos(m_nLayered);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_LAYERED));

	// abilita i controlli
	GetDlgItem(IDC_CHECK_LAYERED)->EnableWindow(m_enumOsVer==WINDOWS_2000 || m_enumOsVer==WINDOWS_XP || m_enumOsVer==WINDOWS_VISTA || m_enumOsVer==WINDOWS_SEVEN);
	GetDlgItem(IDC_STATIC_LAYERED)->EnableWindow(m_enumOsVer==WINDOWS_2000 || m_enumOsVer==WINDOWS_XP || m_enumOsVer==WINDOWS_VISTA || m_enumOsVer==WINDOWS_SEVEN);
	GetDlgItem(IDC_EDIT_LAYERED)->EnableWindow(m_bLayered);
	GetDlgItem(IDC_SPIN_LAYERED)->EnableWindow(m_bLayered);

	char szBuffer[2048] = {"The transparency effect requires Windows 2000 or higher, currently you are running on:\r\n\r\n"};
	int n = strlen(szBuffer)-1;
	m_winVer.GetPlatformInfo(szBuffer+n,sizeof(szBuffer)-n);
	GetDlgItem(IDC_STATIC_OS)->SetWindowText(szBuffer);

	CString strLayered;
	strLayered.Format("(%d = invisible, %d = opaque)",LWA_ALPHA_INVISIBLE,LWA_ALPHA_OPAQUE);
	SetDlgItemText(IDC_STATIC_LAYERED,strLayered);

	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_LAYERED),IDC_CHECK_LAYERED,IDS_TOOLTIP_OPTIONS_DRAW_TRANSPARENCY);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_LAYERED),IDC_EDIT_LAYERED,IDS_TOOLTIP_OPTIONS_DRAW_TRANSPARENCYVALUE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_LAYERED),IDC_SPIN_LAYERED,IDS_TOOLTIP_OPTIONS_DRAW_TRANSPARENCYVALUE);
	}

	UpdateData(FALSE);

	return(FALSE);
}

/*
	OnVScroll()
*/
void CWallPaperDrawSettingsAlphaDlg::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
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
void CWallPaperDrawSettingsAlphaDlg::OnCheckLayered(void)
{
	m_bLayered = !m_bLayered;

	GetDlgItem(IDC_EDIT_LAYERED)->EnableWindow(m_bLayered);
	GetDlgItem(IDC_SPIN_LAYERED)->EnableWindow(m_bLayered);
}

/*
	OnEnChangeLayered()
*/
void CWallPaperDrawSettingsAlphaDlg::OnEnChangeLayered(void)
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_LAYERED,strValue);
	int nValue = atoi(strValue);

	if(nValue < LWA_ALPHA_INVISIBLE || nValue > LWA_ALPHA_OPAQUE)
	{
		strValue.Format("%d",DEFAULT_LAYERED_VALUE);
		m_nLayered = DEFAULT_LAYERED_VALUE;
		UpdateData(FALSE);
	}
}
