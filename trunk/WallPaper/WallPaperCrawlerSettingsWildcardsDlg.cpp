/*
	WallPaperCrawlerSettingsWildcardsDlg.cpp
	Dialogo per la pagina relativa alle opzioni per il crawler.
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
#include <stdio.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CListCtrlEx.h"
#include "CUrl.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
#include "WallPaperCrawlerSettingsWildcardsDlg.h"
#include "WallPaperMessages.h"
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

BEGIN_MESSAGE_MAP(CWallPaperCrawlerSettingsWildcardsDlg,CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_ADD,OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE,OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT,OnButtonDefault)
	ON_BN_CLICKED(IDC_CHECK_SAVELIST,OnCheckSaveList)
	ON_MESSAGE(WM_ONLBUTTONDBLCLK,OnListDoubleClick)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperCrawlerSettingsWildcardsDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperCrawlerSettingsWildcardsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_EDIT_VALUE,m_strValue);
	DDX_Control(pDX,IDC_LIST_WILDCARDS,m_wndWildcardsList);
	DDX_Check(pDX,IDC_CHECK_SAVELIST,m_bSaveList);
}

/*
	CreateEx()
*/
BOOL CWallPaperCrawlerSettingsWildcardsDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperCrawlerSettingsWildcardsDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// crea la lista
	m_wndWildcardsList.Create(this);
	m_wndWildcardsList.SetExStyle(LVS_EX_FULLROWSELECT);
	m_wndWildcardsList.SetIlcColor(ILC_MASK);
	m_wndWildcardsList.SetMultipleSelection(FALSE);
	m_wndWildcardsList.SetLDoubleClickMessage(WM_ONLBUTTONDBLCLK);
	m_wndWildcardsList.RightClickSelects(TRUE);

	m_wndWildcardsList.AddCol("Exclude string",'C');
	m_wndWildcardsList.AddIcon(::LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_INVALID)));

	// inserisce gli items nella lista
	char* p;
	char szKey[REGKEY_MAX_KEY_NAME+1];
	for(int i = 0;; i++)
	{
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_WILDCARDSEXCLUDE_KEY,i);
		if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_WILDCARDSEXCLUDELIST_KEY,szKey)),"")!=0)
			m_wndWildcardsList.AddItem(p,0);
		else
			break;
	}

	m_wndWildcardsList.Sort();
	m_wndWildcardsList.SelectItem(0);

	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_VALUE),IDC_EDIT_VALUE,IDS_TOOLTIP_OPTIONS_WILDCARDS_VALUE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_ADD),IDC_BUTTON_ADD,IDS_TOOLTIP_OPTIONS_WILDCARDS_ADD);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_REMOVE),IDC_BUTTON_REMOVE,IDS_TOOLTIP_OPTIONS_WILDCARDS_REMOVE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_DEFAULT),IDC_BUTTON_DEFAULT,IDS_TOOLTIP_OPTIONS_WILDCARDS_DEFAULT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_LIST_WILDCARDS),IDC_LIST_WILDCARDS,IDS_TOOLTIP_OPTIONS_WILDCARDS_LIST);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_SAVELIST),IDC_CHECK_SAVELIST,IDS_TOOLTIP_OPTIONS_WILDCARDS_SAVE);
	}

	UpdateData(FALSE);

	return(TRUE);
}

/*
	OnSetActive()
*/
BOOL CWallPaperCrawlerSettingsWildcardsDlg::OnSetActive(void)
{
	UpdateData(FALSE);
	return(TRUE);
}

/*
	OnButtonAdd()
*/
void CWallPaperCrawlerSettingsWildcardsDlg::OnButtonAdd(void)
{
	CUrl url;
	char szValue[MAX_URL+1];

	UpdateData(TRUE);
	strcpyn(szValue,m_strValue,sizeof(szValue));

	// controlla che contenga caratteri jolly
	if(!strchr(szValue,'?') && !strchr(szValue,'*'))
	{
		if(::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_ISNOT_WILDCARDS,szValue)==IDNO)
			return;
	}

	// controlla che non sia un url
	if(url.IsUrl(szValue))
	{
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_IS_HTTP_URL,szValue);
		return;
	}

	// inserisce la wildcard solo se non esiste gia' (notare che distingue tra maiuscole e minuscole)
	if(strlen(szValue) > 0)
	{
		if(m_wndWildcardsList.FindItem(szValue,FALSE) < 0)
			m_wndWildcardsList.SelectItem(m_wndWildcardsList.AddItem(szValue,0));
		else
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_VALUE_EXISTS,szValue);
	}
	else
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_VALUE);
}

/*
	OnButtonRemove()
*/
void CWallPaperCrawlerSettingsWildcardsDlg::OnButtonRemove(void)
{
	int nCurSel = m_wndWildcardsList.GetCurrentItem();

	if(nCurSel >= 0)
	{
		m_wndWildcardsList.DeleteItem(nCurSel);

		if(m_wndWildcardsList.GetItemCount() > 0)
			m_wndWildcardsList.SelectItem(0);
	}
	else
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_SELECTITEM);
}

/*
	OnButtonDefault()
*/
void CWallPaperCrawlerSettingsWildcardsDlg::OnButtonDefault(void)
{
	if(::MessageBoxResource(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_RESTORE_DEFAULT_FILE_TYPE_LIST)==IDYES)
	{
		m_wndWildcardsList.RemoveAllIcons();
		m_wndWildcardsList.DeleteAllItems();
		m_wndWildcardsList.AddItem("*/banner/*");
		m_wndWildcardsList.AddItem("*/banners/*");
		m_wndWildcardsList.AddItem("*/cgi/*");
		m_wndWildcardsList.AddItem("*/cgibin/*");
		m_wndWildcardsList.AddItem("*/cgi-bin/*");
		m_wndWildcardsList.AddItem("*/scripts/*");
		m_wndWildcardsList.SelectItem(0);
	}
}

/*
	OnListDoubleClick()
*/
LONG CWallPaperCrawlerSettingsWildcardsDlg::OnListDoubleClick(UINT /*wParam*/,LONG /*lParam*/)
{
	int nItem;
	
	if((nItem = m_wndWildcardsList.GetCurrentItem()) >= 0)
	{
		char szItem[MAX_ITEM_SIZE+1];
		memset(szItem,'\0',sizeof(szItem));
		m_wndWildcardsList.GetItemText(nItem,0,szItem,sizeof(szItem));
		m_strValue.Format("%s",szItem);
		UpdateData(FALSE);
	}
	
	return(0L);
}

/*
	OnCheckSaveList()
*/
void CWallPaperCrawlerSettingsWildcardsDlg::OnCheckSaveList(void)
{
	m_bSaveList = !m_bSaveList;
}
