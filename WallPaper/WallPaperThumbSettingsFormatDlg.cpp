/*
	WallPaperThumbSettingsFormatDlg.cpp
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
#include "strings.h"
#include "CRegistry.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
#include "WallPaperThumbSettingsFormatDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperThumbSettingsFormatDlg,CPropertyPage)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_EDIT_THUMBNAILWIDTH,OnEditChangeWidth)
	ON_EN_CHANGE(IDC_EDIT_THUMBNAILHEIGHT,OnEditChangeHeight)
	ON_CBN_KILLFOCUS(IDC_COMBO_THUMBNAILFORMAT,OnKillFocusComboFormat)
	ON_BN_CLICKED(IDC_RADIO_THUMBNAILNAMINGRULENUMERIC,OnRadioButtonNumeric)
	ON_BN_CLICKED(IDC_RADIO_THUMBNAILNAMINGRULEPREFIX,OnRadioButtonPrefix)
	ON_BN_CLICKED(IDC_RADIO_THUMBNAILNAMINGRULESUFFIX,OnRadioButtonSuffix)
	ON_BN_CLICKED(IDC_RADIO_THUMBNAILNAMINGRULEDEFAULT,OnRadioButtonDefault)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperThumbSettingsFormatDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperThumbSettingsFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_THUMBNAILWIDTH,m_nWidth);
	DDX_Text(pDX,IDC_EDIT_THUMBNAILHEIGHT,m_nHeight);
	DDX_Control(pDX,IDC_COMBO_THUMBNAILFORMAT,m_wndComboFormat);
	DDX_Radio(pDX,IDC_RADIO_THUMBNAILNAMINGRULENUMERIC,m_nNumeric);
	DDX_Radio(pDX,IDC_RADIO_THUMBNAILNAMINGRULEPREFIX,m_nPrefix);
	DDX_Radio(pDX,IDC_RADIO_THUMBNAILNAMINGRULESUFFIX,m_nSuffix);
	DDX_Radio(pDX,IDC_RADIO_THUMBNAILNAMINGRULEDEFAULT,m_nDefault);
	DDX_Text(pDX,IDC_EDIT_THUMBNAILNAMINGRULE,m_strName);
}

/*
	CreateEx()
*/
BOOL CWallPaperThumbSettingsFormatDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// crea l'oggetto immagine per l'elenco dei formati supportati
	char szLibraryName[_MAX_PATH+1];
	strcpyn(szLibraryName,m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY),sizeof(szLibraryName));
	m_pImage = m_ImageFactory.Create(szLibraryName,sizeof(szLibraryName));

	// imposta con i valori della configurazione
	m_nWidth = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_X_KEY);
	m_nHeight = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_Y_KEY);
	m_PictureType = NULL_PICTURE;
	m_strFormat.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_FORMAT_KEY));
	m_nNumeric = m_nPrefix = m_nSuffix = m_nDefault = 0;
	m_strName = "";
	m_strPrefixSuffix = "";
	switch(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAMING_KEY))
	{
		case	1:
			m_NamingRule = NUMERIC_NAMINGRULE;
			m_nNumeric = 0;
			m_nPrefix = 1;
			m_nSuffix = 1;
			m_nDefault = 1;
			m_strName = "0";
			break;
		case 2:
			m_NamingRule = PREFIX_NAMINGRULE;
			m_nNumeric = 1;
			m_nPrefix = 0;
			m_nSuffix = 1;
			m_nDefault = 1;
			m_strName.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAME_KEY));
			m_strPrefixSuffix = m_strName;
			break;
		case 3:
			m_NamingRule = SUFFIX_NAMINGRULE;
			m_nNumeric = 1;
			m_nPrefix = 1;
			m_nSuffix = 0;
			m_nDefault = 1;
			m_strName.Format("%s",m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAME_KEY));
			m_strPrefixSuffix = m_strName;
			break;
		case 0:
		default:
			m_NamingRule = DEFAULT_NAMINGRULE;
			m_nNumeric = 1;
			m_nPrefix = 1;
			m_nSuffix = 1;
			m_nDefault = 0;
			m_strName = "thumbnail.<filename>.<ext>";
			break;
	}

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperThumbSettingsFormatDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// imposta gli spin per le dimensioni
	CSpinButtonCtrl* pSpin;

	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_THUMBNAILWIDTH);
	pSpin->SetRange(0/*DEFAULT_DRAWRECTX_MIN*/,(short)::GetSystemMetrics(SM_CXSCREEN));
	pSpin->SetPos(m_nWidth);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_THUMBNAILWIDTH));

	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_THUMBNAILHEIGHT);
	pSpin->SetRange(0/*DEFAULT_DRAWRECTY_MIN*/,(short)::GetSystemMetrics(SM_CYSCREEN));
	pSpin->SetPos(m_nHeight);
	pSpin->SetBuddy(GetDlgItem(IDC_EDIT_THUMBNAILHEIGHT));

	// carica il combo con i formati supportati
	int i = 1;
	int nIndex = 0;
	LPIMAGETYPE p;
	char szComboEntry[_MAX_PATH+1];
	HICON hIcon;
	CRegistry registry;
	REGISTERFILETYPE registerfiletype;

	m_wndComboFormat.AddString("<same format as source>");
	while((p = m_pImage->EnumWritableImageFormats())!=(LPIMAGETYPE)NULL)
	{
		memset(&registerfiletype,'\0',sizeof(REGISTERFILETYPE));
		hIcon = registry.GetSafeIconForRegisteredFileType(p->ext,&registerfiletype);
		if(hIcon)
			::DestroyIcon(hIcon);
		else
			strcpyn(registerfiletype.description,p->desc,sizeof(registerfiletype.description));

		_snprintf(szComboEntry,sizeof(szComboEntry)-1,"%s (*%s)",registerfiletype.description,p->ext);
		m_wndComboFormat.AddString(szComboEntry);
		if(stricmp(m_strFormat,p->ext+1)==0)
			nIndex = i;
		i++;
	}
	m_wndComboFormat.SetCurSel(nIndex);

	// abilita per prefisso/suffisso
	GetDlgItem(IDC_EDIT_THUMBNAILNAMINGRULE)->EnableWindow((m_NamingRule==PREFIX_NAMINGRULE || m_NamingRule==SUFFIX_NAMINGRULE) ? TRUE : FALSE);
	
	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_THUMBNAILWIDTH),IDC_EDIT_THUMBNAILWIDTH,IDS_TOOLTIP_OPTIONS_THUMB_WIDTH);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_THUMBNAILWIDTH),IDC_SPIN_THUMBNAILWIDTH,IDS_TOOLTIP_OPTIONS_THUMB_WIDTH);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_THUMBNAILHEIGHT),IDC_EDIT_THUMBNAILHEIGHT,IDS_TOOLTIP_OPTIONS_THUMB_HEIGHT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_SPIN_THUMBNAILHEIGHT),IDC_SPIN_THUMBNAILHEIGHT,IDS_TOOLTIP_OPTIONS_THUMB_HEIGHT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_COMBO_THUMBNAILFORMAT),IDC_COMBO_THUMBNAILFORMAT,IDS_TOOLTIP_OPTIONS_THUMB_FORMAT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_RADIO_THUMBNAILNAMINGRULENUMERIC),IDC_RADIO_THUMBNAILNAMINGRULENUMERIC,IDS_TOOLTIP_OPTIONS_THUMB_NAMERULE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_RADIO_THUMBNAILNAMINGRULESUFFIX),IDC_RADIO_THUMBNAILNAMINGRULESUFFIX,IDS_TOOLTIP_OPTIONS_THUMB_NAMERULE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_RADIO_THUMBNAILNAMINGRULEPREFIX),IDC_RADIO_THUMBNAILNAMINGRULEPREFIX,IDS_TOOLTIP_OPTIONS_THUMB_NAMERULE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_RADIO_THUMBNAILNAMINGRULEDEFAULT),IDC_RADIO_THUMBNAILNAMINGRULEDEFAULT,IDS_TOOLTIP_OPTIONS_THUMB_NAMERULE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_THUMBNAILNAMINGRULE),IDC_EDIT_THUMBNAILNAMINGRULE,IDS_TOOLTIP_OPTIONS_THUMB_NAMERULE);
	}

	UpdateData(FALSE);

	return(FALSE);
}

/*
	OnEditChangeWidth()
*/
void CWallPaperThumbSettingsFormatDlg::OnEditChangeWidth(void)
{
	// per aggiornare lo spin quando si cambia il valore del campo manualmente
	CString strValue;
	GetDlgItem(IDC_EDIT_THUMBNAILWIDTH)->GetWindowText(strValue);
	CSpinButtonCtrl* pSpin;
	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_THUMBNAILWIDTH);
	pSpin->SetPos(atoi(strValue));
}

/*
	OnEditChangeHeight()
*/
void CWallPaperThumbSettingsFormatDlg::OnEditChangeHeight(void)
{
	// per aggiornare lo spin quando si cambia il valore del campo manualmente
	CString strValue;
	GetDlgItem(IDC_EDIT_THUMBNAILHEIGHT)->GetWindowText(strValue);
	CSpinButtonCtrl* pSpin;
	pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_THUMBNAILHEIGHT);
	pSpin->SetPos(atoi(strValue));
}

/*
	OnVScroll()
*/
void CWallPaperThumbSettingsFormatDlg::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
	// per aggiornare il valore del campo quando cambia la posizione dello spin
	if(nSBCode!=SB_ENDSCROLL)
	{
		CString strValue;
		strValue.Format("%d",(int)nPos);
		((CSpinButtonCtrl*)pScrollBar)->GetBuddy()->SetWindowText(strValue);
		((CSpinButtonCtrl*)pScrollBar)->SetPos((int)nPos);
	}
}

/*
	OnKillFocusComboFormat()
*/
void CWallPaperThumbSettingsFormatDlg::OnKillFocusComboFormat(void)
{
	// ricava la selezione corrente del combo
	char szFormat[_MAX_PATH+1];
	char szComboEntry[_MAX_PATH+1];
	LPIMAGETYPE p;
	
	m_PictureType = NULL_PICTURE;
	m_strFormat.Format("%s",DEFAULT_THUMBNAILS_FORMAT);
	m_wndComboFormat.GetWindowText(szComboEntry,sizeof(szComboEntry));

	HICON hIcon;
	CRegistry registry;
	REGISTERFILETYPE registerfiletype;

	while((p = m_pImage->EnumWritableImageFormats())!=(LPIMAGETYPE)NULL)
	{
		memset(&registerfiletype,'\0',sizeof(REGISTERFILETYPE));
		hIcon = registry.GetSafeIconForRegisteredFileType(p->ext,&registerfiletype);
		if(hIcon)
			::DestroyIcon(hIcon);
		else
			strcpyn(registerfiletype.description,p->desc,sizeof(registerfiletype.description));

		_snprintf(szFormat,sizeof(szFormat)-1,"%s (*%s)",registerfiletype.description,p->ext);
		if(stricmp(szFormat,szComboEntry)==0)
		{
			m_PictureType = p->type;
			m_strFormat.Format("%s",p->ext+1);
		}
	}
}

/*
	OnRadioButtonNumeric()
*/
void CWallPaperThumbSettingsFormatDlg::OnRadioButtonNumeric(void)
{
	UpdateData(TRUE);

	m_NamingRule = NUMERIC_NAMINGRULE;
	m_nNumeric = 0;
	m_nPrefix = 1;
	m_nSuffix = 1;
	m_nDefault = 1;
	m_strName = "0";

	GetDlgItem(IDC_EDIT_THUMBNAILNAMINGRULE)->EnableWindow(FALSE);

	UpdateData(FALSE);
}

/*
	OnRadioButtonPrefix()
*/
void CWallPaperThumbSettingsFormatDlg::OnRadioButtonPrefix(void)
{
	UpdateData(TRUE);

	m_NamingRule = PREFIX_NAMINGRULE;
	m_nNumeric = 1;
	m_nPrefix = 0;
	m_nSuffix = 1;
	m_nDefault = 1;
	if(strcmp(m_strName,".suffix")==0 || strcmp(m_strName,"0")==0 || strcmp(m_strName,"thumbnail.<filename>.<ext>")==0)
		m_strName = m_strPrefixSuffix;
	if(m_strName.IsEmpty())
		m_strName = "prefix.";

	GetDlgItem(IDC_EDIT_THUMBNAILNAMINGRULE)->EnableWindow(TRUE);

	UpdateData(FALSE);
}

/*
	OnRadioButtonSuffix()
*/
void CWallPaperThumbSettingsFormatDlg::OnRadioButtonSuffix(void)
{
	UpdateData(TRUE);

	m_NamingRule = SUFFIX_NAMINGRULE;
	m_nNumeric = 1;
	m_nPrefix = 1;
	m_nSuffix = 0;
	m_nDefault = 1;
	if(strcmp(m_strName,"prefix.")==0 || strcmp(m_strName,"0")==0 || strcmp(m_strName,"thumbnail.<filename>.<ext>")==0)
		m_strName = m_strPrefixSuffix;
	if(m_strName.IsEmpty())
		m_strName = ".suffix";

	GetDlgItem(IDC_EDIT_THUMBNAILNAMINGRULE)->EnableWindow(TRUE);

	UpdateData(FALSE);
}

/*
	OnRadioButtonDefault()
*/
void CWallPaperThumbSettingsFormatDlg::OnRadioButtonDefault(void)
{
	UpdateData(TRUE);

	m_NamingRule = DEFAULT_NAMINGRULE;
	m_nNumeric = 1;
	m_nPrefix = 1;
	m_nSuffix = 1;
	m_nDefault = 0;
	m_strName = "thumbnail.<filename>.<ext>";

	GetDlgItem(IDC_EDIT_THUMBNAILNAMINGRULE)->EnableWindow(FALSE);

	UpdateData(FALSE);
}
