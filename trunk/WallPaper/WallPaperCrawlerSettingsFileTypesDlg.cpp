/*
	WallPaperCrawlerSettingsFileTypesDlg.cpp
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
#include "CImageFactory.h"
#include "CListCtrlEx.h"
#include "CRegistry.h"
#include "CArchive.h"
#include "CUrl.h"
#include "CAudioInfo.h"
#include "CAudioPlayer.h"
#include "CImageObject.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
#include "WallPaperCrawlerSettingsFileTypesDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperCrawlerSettingsFileTypesDlg,CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_ADD,OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE,OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT,OnButtonDefault)
	ON_MESSAGE(WM_ONLBUTTONDBLCLK,OnListDoubleClick)
	ON_BN_CLICKED(IDC_CHECK_FILETYPESACCEPTANCE,OnCheckAskFileTypes)
	ON_BN_CLICKED(IDC_CHECK_MULTITHREAD,OnCheckMultiThread)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperCrawlerSettingsFileTypesDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperCrawlerSettingsFileTypesDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_VALUE,m_strValue);
	DDX_Control(pDX,IDC_LIST_FILETYPES,m_wndFileTypesList);
	DDX_Check(pDX,IDC_CHECK_FILETYPESACCEPTANCE,m_bAskFileTypes);
	DDX_Check(pDX,IDC_CHECK_MULTITHREAD,m_bMultiThread);
}

/*
	CreateEx()
*/
BOOL CWallPaperCrawlerSettingsFileTypesDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// imposta con i valori della configurazione
	m_bAskFileTypes = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_FILETYPESACCEPTANCE_KEY);
	m_bMultiThread = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY);
	
	char* p = (char*)m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_CURRENTLIBRARY_KEY);
	if(strcmp(p,"")!=0)
	{
		strcpyn(m_szLibraryName,p,sizeof(m_szLibraryName));
		m_pImage = m_ImageFactory.Create(m_szLibraryName,sizeof(m_szLibraryName));
	}
	else
		m_pImage = m_ImageFactory.Create();

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperCrawlerSettingsFileTypesDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// crea la lista	
	m_wndFileTypesList.Create(this);
	m_wndFileTypesList.SetExStyle(LVS_EX_FULLROWSELECT);
	m_wndFileTypesList.SetIlcColor(ILC_COLOR16);
	m_wndFileTypesList.SetMultipleSelection(FALSE);
	m_wndFileTypesList.SetLDoubleClickMessage(WM_ONLBUTTONDBLCLK);
	m_wndFileTypesList.RightClickSelects(TRUE);

	m_wndFileTypesList.AddCol("Extension",'C');
	m_wndFileTypesList.AddCol("Description",'C');
	m_wndFileTypesList.AddCol("Content Type",'C');
	m_wndFileTypesList.AddCol("Associated Program",'C');

	char* p;
	int nIconID = 0;
	char szKey[REGKEY_MAX_KEY_NAME+1];
	char szItem[MAX_ITEM_SIZE+1];
	REGISTERFILETYPE registerfiletype;
	CRegistry registry;

	for(int i = 0;; i++)
	{
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_FILETYPE_KEY,i);
		if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_FILETYPELIST_KEY,szKey)),"")!=0)
		{
			memset(&registerfiletype,'\0',sizeof(REGISTERFILETYPE));
			m_wndFileTypesList.AddIcon(registry.GetSafeIconForRegisteredFileType(p,&registerfiletype,GetIconID(p)));
			_snprintf(szItem,
					sizeof(szItem)-1,
					"%s;%s;%s;%s",
					p,
					registerfiletype.description[0]!='\0' ? registerfiletype.description : "?",
					registerfiletype.contenttype[0]!='\0' ? registerfiletype.contenttype : "?",
					registerfiletype.shell[0]!='\0' ? registerfiletype.shell : "?"
					);
			m_wndFileTypesList.AddItem(szItem,nIconID);
			nIconID++;
		}
		else
			break;
	}

	m_wndFileTypesList.Sort(SORT_ASC);
	m_wndFileTypesList.SelectItem(0);

	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_VALUE),IDC_EDIT_VALUE,IDS_TOOLTIP_OPTIONS_FILETYPE_VALUE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_ADD),IDC_BUTTON_ADD,IDS_TOOLTIP_OPTIONS_FILETYPE_ADD);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_REMOVE),IDC_BUTTON_REMOVE,IDS_TOOLTIP_OPTIONS_FILETYPE_REMOVE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_DEFAULT),IDC_BUTTON_DEFAULT,IDS_TOOLTIP_OPTIONS_FILETYPE_DEFAULT);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_LIST_FILETYPES),IDC_LIST_FILETYPES,IDS_TOOLTIP_OPTIONS_FILETYPE_LIST);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_FILETYPESACCEPTANCE),IDC_CHECK_FILETYPESACCEPTANCE,IDS_TOOLTIP_OPTIONS_FILETYPE_FILETYPESACCEPTANCE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_MULTITHREAD),IDC_CHECK_MULTITHREAD,IDS_TOOLTIP_OPTIONS_FILETYPE_MULTITHREAD);
	}

	UpdateData(FALSE);

	return(TRUE);
}

/*
	OnButtonAdd()
*/
void CWallPaperCrawlerSettingsFileTypesDlg::OnButtonAdd(void)
{
	UpdateData(TRUE);
	
	char szValue[MAX_URL+1];
	strcpyn(szValue,m_strValue,sizeof(szValue));

	if(szValue[0]!='.')
	{
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_FILE_EXT);
		return;
	}

	if(strcmp(szValue,".*")==0)
	{
		if(::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_IS_WILDCARDS,szValue)==IDNO)
			return;
	}
	else if(strchr(szValue,'?') || strchr(szValue,'*'))
	{
		if(::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_CONTAINS_WILDCARDS,szValue)==IDNO)
			return;
	}

	if(strlen(szValue) > 0)
	{
		if(m_wndFileTypesList.FindItem(szValue) < 0)
		{
			CUrl url;
			char* pExt;
			BOOL bIsARegisteredHtmlExtension = FALSE;
			
			while((pExt = (char*)url.EnumHtmlFileTypes())!=NULL)
				if(stricmp(szValue,pExt)==0)
					bIsARegisteredHtmlExtension = TRUE;
			
			if(bIsARegisteredHtmlExtension)
			{
				::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_REG_HTML_TYPE,szValue);
			}
			else
			{
				int nIconID = 0;
				char szItem[MAX_ITEM_SIZE+1];
				REGISTERFILETYPE registerfiletype;
				CRegistry registry;

				memset(&registerfiletype,'\0',sizeof(REGISTERFILETYPE));
				nIconID = m_wndFileTypesList.AddIcon(registry.GetSafeIconForRegisteredFileType(szValue,&registerfiletype,GetIconID(szValue)));
				
				_snprintf(szItem,
						sizeof(szItem)-1,
						"%s;%s;%s;%s",
						szValue,
						registerfiletype.description[0]!='\0' ? registerfiletype.description : "?",
						registerfiletype.contenttype[0]!='\0' ? registerfiletype.contenttype : "?",
						registerfiletype.shell[0]!='\0' ? registerfiletype.shell : "?"
						);

				int nItem = m_wndFileTypesList.AddItem(szItem,nIconID);
				m_wndFileTypesList.Sort(SORT_ASC);
				if((nItem = m_wndFileTypesList.FindItem(szValue)) >= 0)
					m_wndFileTypesList.SelectItem(nItem);
			}
		}
		else
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_VALUE_EXISTS,szValue);
	}
	else
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_VALUE);
}

/*
	OnButtonRemove()
*/
void CWallPaperCrawlerSettingsFileTypesDlg::OnButtonRemove(void)
{
	int nCurSel = m_wndFileTypesList.GetCurrentItem();

	if(nCurSel >= 0)
	{
		m_wndFileTypesList.DeleteItem(nCurSel);

		if(m_wndFileTypesList.GetItemCount() <= 0)
		{
			::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_FILE_TYPE_LIST);
			
			OnDefaultFileTypes();
			
			if(m_wndFileTypesList.GetItemCount() > 0)
				m_wndFileTypesList.SelectItem(0);
		}
		else
			m_wndFileTypesList.SelectItem(0);
	}
	else
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_SELECTITEM);
}

/*
	OnButtonDefault()
*/
void CWallPaperCrawlerSettingsFileTypesDlg::OnButtonDefault(void)
{
	if(::MessageBoxResource(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_RESTORE_DEFAULT_FILE_TYPE_LIST)==IDYES)
	{
		OnDefaultFileTypes();
				
		if(m_wndFileTypesList.GetItemCount() > 0)
			m_wndFileTypesList.SelectItem(0);
	}
}

/*
	OnListDoubleClick()
*/
LONG CWallPaperCrawlerSettingsFileTypesDlg::OnListDoubleClick(UINT /*wParam*/,LONG /*lParam*/)
{
	int nItem;
	
	if((nItem = m_wndFileTypesList.GetCurrentItem()) >= 0)
	{
		char szItem[MAX_ITEM_SIZE+1];
		memset(szItem,'\0',sizeof(szItem));
		m_wndFileTypesList.GetItemText(nItem,0,szItem,sizeof(szItem));
		m_strValue.Format("%s",szItem);
		UpdateData(FALSE);
	}
	
	return(0L);
}

/*
	OnCheckAskFileTypes()
*/
void CWallPaperCrawlerSettingsFileTypesDlg::OnCheckAskFileTypes(void)
{
	m_bAskFileTypes = !m_bAskFileTypes;
}

/*
	OnCheckMultiThread()
*/
void CWallPaperCrawlerSettingsFileTypesDlg::OnCheckMultiThread(void)
{
	m_bMultiThread = !m_bMultiThread;
}

/*
	OnDefaultFileTypes()
*/
void CWallPaperCrawlerSettingsFileTypesDlg::OnDefaultFileTypes(void)
{
	LPIMAGETYPE p;
	int nIconID = 0;
	char szItem[MAX_ITEM_SIZE+1];
	REGISTERFILETYPE registerfiletype;
	CRegistry registry;

	m_wndFileTypesList.RemoveAllIcons();
	m_wndFileTypesList.DeleteAllItems();

	while((p = m_pImage->EnumReadableImageFormats())!=(LPIMAGETYPE)NULL)
	{
		memset(&registerfiletype,'\0',sizeof(REGISTERFILETYPE));
		m_wndFileTypesList.AddIcon(registry.GetSafeIconForRegisteredFileType(p->ext,&registerfiletype,GetIconID(p->ext)));
		_snprintf(szItem,
				sizeof(szItem)-1,
				"%s;%s;%s;%s",
				p->ext,
				registerfiletype.description[0]!='\0' ? registerfiletype.description : "?",
				registerfiletype.contenttype[0]!='\0' ? registerfiletype.contenttype : "?",
				registerfiletype.shell[0]!='\0' ? registerfiletype.shell : "?"
				);
		m_wndFileTypesList.AddItem(szItem,nIconID);
		nIconID++;
	}

	// idem in WallPaperConfig.cpp
	DIRTY_DECLARATION_FOR_FILE_TYPES(szExtraExtArray)

	for(int i=0; szExtraExtArray[i]; i++)
	{
		memset(&registerfiletype,'\0',sizeof(REGISTERFILETYPE));
		m_wndFileTypesList.AddIcon(registry.GetSafeIconForRegisteredFileType(szExtraExtArray[i],&registerfiletype,GetIconID(szExtraExtArray[i])));
		_snprintf(szItem,
				sizeof(szItem)-1,
				"%s;%s;%s;%s",
				szExtraExtArray[i],
				registerfiletype.description[0]!='\0' ? registerfiletype.description : "?",
				registerfiletype.contenttype[0]!='\0' ? registerfiletype.contenttype : "?",
				registerfiletype.shell[0]!='\0' ? registerfiletype.shell : "?"
				);
		m_wndFileTypesList.AddItem(szItem,nIconID);
		nIconID++;
	}
}

/*
	GetIconID()
*/
UINT CWallPaperCrawlerSettingsFileTypesDlg::GetIconID(LPCSTR lpcszFileType)
{
	CUrl url;
	CArchiveFile arc;

	UINT nID = 0;
	if(CArchiveFile::IsArchiveFile(lpcszFileType))
	{
		if(arc.GetArchiveType(lpcszFileType)==ARCHIVETYPE_GZW)
			nID = IDI_ICON_GZW;
		else if(arc.GetArchiveType(lpcszFileType)==ARCHIVETYPE_ZIP)
			nID = IDI_ICON_ZIP_FILE;
		else if(arc.GetArchiveType(lpcszFileType)==ARCHIVETYPE_RAR)
			nID = IDI_ICON_RAR_FILE;
		else
			nID = IDI_ICON_RAR_FILE;
	}
	else if(url.IsHtmlFile(lpcszFileType))
		nID = IDI_ICON_URL_FILE;
	else if(CAudioPlayer::IsAudioFile(lpcszFileType))
		nID = IDI_ICON_AUDIO_FILE;
	else if(CImageObject::IsImageFile(lpcszFileType))
		nID = IDI_ICON_PICTURE_FILE;
	else
		nID = IDI_ICON_THUMB;

	return(nID);
}
