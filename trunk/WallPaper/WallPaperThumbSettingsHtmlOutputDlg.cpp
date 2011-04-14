/*
	WallPaperThumbSettingsHtmlOutputDlg.cpp
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
#include "CFileDialogEx.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
#include "WallPaperThumbSettingsHtmlOutputDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperThumbSettingsHtmlOutputDlg,CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_HTMLFILE,OnButtonHtmlFile)
	ON_BN_CLICKED(IDC_CHECK_GENERATEHTML,OnCheckGenerateHtml)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperThumbSettingsHtmlOutputDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperThumbSettingsHtmlOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_HTMLTITLE,m_strHtmlTitle);
	DDX_Text(pDX,IDC_EDIT_HTMLFILE,m_strHtmlOutput);
	DDX_Text(pDX,IDC_EDIT_IMAGEURL,m_strBaseUrlForPictures);
	DDX_Text(pDX,IDC_EDIT_THUMBURL,m_strBaseUrlForThumbnails);
	DDX_Check(pDX,IDC_CHECK_GENERATEHTML,m_bGenerateHtml);
}

/*
	CreateEx()
*/
BOOL CWallPaperThumbSettingsHtmlOutputDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// imposta i campi del dialogo con i valori della configurazione
	m_strHtmlOutput           = m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_HTMLFILE_KEY);
	m_strHtmlTitle            = m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_HTMLTITLE_KEY);
	m_strBaseUrlForPictures   = m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_IMAGEURL_KEY);
	m_strBaseUrlForThumbnails = m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_THUMBURL_KEY);

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperThumbSettingsHtmlOutputDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// aggiorna i campi del dialogo
	OnSetActive();
	
	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_HTMLTITLE),IDC_EDIT_HTMLTITLE,IDS_TOOLTIP_OPTIONS_THUMB_HTMLTITLE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_HTMLFILE),IDC_EDIT_HTMLFILE,IDS_TOOLTIP_OPTIONS_THUMB_HTMLFILE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_HTMLFILE),IDC_BUTTON_HTMLFILE,IDS_TOOLTIP_OPTIONS_THUMB_HTMLFILE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_IMAGEURL),IDC_EDIT_IMAGEURL,IDS_TOOLTIP_OPTIONS_THUMB_PICTURL);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_THUMBURL),IDC_EDIT_THUMBURL,IDS_TOOLTIP_OPTIONS_THUMB_THUMBURL);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_GENERATEHTML),IDC_CHECK_GENERATEHTML,IDS_TOOLTIP_OPTIONS_THUMB_GENERATEHTML);
	}
	
	return(FALSE);
}

/*
	OnSetActive()
*/
BOOL CWallPaperThumbSettingsHtmlOutputDlg::OnSetActive(void)
{
	// abilita i controlli
	GetDlgItem(IDC_EDIT_HTMLTITLE)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_EDIT_HTMLFILE)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_EDIT_IMAGEURL)->EnableWindow(m_bGenerateHtml);
	GetDlgItem(IDC_EDIT_THUMBURL)->EnableWindow(m_bGenerateHtml);
	UpdateData(FALSE);

	return(TRUE);
}

/*
	OnButtonHtmlFile()
*/
void CWallPaperThumbSettingsHtmlOutputDlg::OnButtonHtmlFile(void)
{
	CFileSaveAsDialog dlg("Select output file",
					m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDFILE_KEY),
					"*.htm",
					m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_HTMLFILE_KEY),
					"HTML files (*.htm;*.html)|*.htm;*.html|All files (*.*)|*.*||"
					);

	if(dlg.DoModal()==IDOK)
	{
		char szDir[_MAX_PATH+1];
		strcpyn(szDir,dlg.GetPathName(),sizeof(szDir));
		strrev(szDir);
		int i = strchr(szDir,'\\') - szDir;
		strrev(szDir);
		if(i > 0)
			szDir[strlen(szDir)-i-1] = '\0';
		m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDFILE_KEY,szDir);

		m_strHtmlOutput.Format("%s",dlg.GetPathName());
		UpdateData(FALSE);
	}
}

/*
	OnCheckGenerateHtml()
*/
void CWallPaperThumbSettingsHtmlOutputDlg::OnCheckGenerateHtml(void)
{
	m_bGenerateHtml = !m_bGenerateHtml;

	OnSetActive();
}
