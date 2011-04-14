/*
	CImageDialog.cpp
	Classi base per i dialoghi File|Apri|Salva per i formati relativi alle immagini (MFC).
	Luca Piergentili, 14/11/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "strings.h"
#include "window.h"
#include "mfcapi.h"
#include "CFileDialogEx.h"
#include "CImageFactory.h"
#include "CImageDialog.h"

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

/*
	CImageSaveAsDialog()
*/
CImageSaveAsDialog::CImageSaveAsDialog(CWnd* pParent/*=NULL*/,CImage* pImage/*=NULL*/,LPCSTR lpcszLibraryName/*=NULL*/,LPCSTR lpcszInitialDir/*=".\\"*/,LPCSTR lpcszDialogTitle/*=NULL*/)
{
	m_pParent = pParent;

	// usa l'oggetto immagine ricevuto in input
	if(pImage)
	{
		m_pImage = pImage;
		memset(m_szLibraryName,'\0',sizeof(m_szLibraryName));
	}
	else // crea l'immagine in proprio
	{
		if(lpcszLibraryName)
			strcpyn(m_szLibraryName,lpcszLibraryName,sizeof(m_szLibraryName));
		else
			memset(m_szLibraryName,'\0',sizeof(m_szLibraryName));
		m_pImage = m_ImageFactory.Create(m_szLibraryName,sizeof(m_szLibraryName));
		
		strcpyn(m_szInitialDir,lpcszInitialDir,sizeof(m_szInitialDir));
	}

	if(lpcszDialogTitle)
		strcpyn(m_szDialogTitle,lpcszDialogTitle,sizeof(m_szDialogTitle));
	else
		strcpyn(m_szDialogTitle,"Save Picture As...",sizeof(m_szDialogTitle));
}

/*
	DoModal()
*/
int CImageSaveAsDialog::DoModal(void)
{
	IMAGE_TYPE type = NULL_PICTURE;
	IMAGE_TYPE prevtype = NULL_PICTURE;
	LPIMAGETYPE p;
	CString strDesc = "";
	CString strExt = "";
	CString strDescFilter = "";
	CString strExtFilter = "";
	int nTotFormats = 0;

	// per i tipi supportati dalla libreria corrente
	while((p = m_pImage->EnumWritableImageFormats())!=(LPIMAGETYPE)NULL)
	{
		type = p->type;
		
		// costruisce il testo per il filtro
		if(prevtype!=NULL_PICTURE && type!=prevtype)
		{
			strDesc += ")";
			::AppendFileType(strDescFilter,strExtFilter,strDesc,strExt);
			m_Dlg.m_ofn.nMaxCustFilter++;
			prevtype = type;
			strDesc.Format("%s(*%s",p->desc,p->ext);
			strExt.Format("*%s",p->ext);
		}
		else
		{
			prevtype = type;
			
			if(strDesc!="")
			{
				strDesc += "; *";
				strDesc += p->ext;
			}
			else
				strDesc.Format("%s(*%s",p->desc,p->ext);
			
			if(strExt!="")
			{
				strExt += "; *";
				strExt += p->ext;
			}
			else
				strExt.Format("*%s",p->ext);
		}
		
		nTotFormats++;
	}

	if(nTotFormats > 0)
	{
		// chiude il testo per il filtro
		strDesc += ")";
		::AppendFileType(strDescFilter,strExtFilter,strDesc,strExt);
		m_Dlg.m_ofn.nMaxCustFilter++;

		// aggiunge il tipo di default
		CString cExtAll = strExtFilter;
		::AppendFileType(strDescFilter,strExtFilter,"All supported formats",(LPCTSTR)cExtAll);
		m_Dlg.m_ofn.nMaxCustFilter++;
		m_Dlg.m_ofn.nFilterIndex = m_Dlg.m_ofn.nMaxCustFilter;

		// aggiunge il tipo "*.*"
		CString cAllFilter;
		if(!cAllFilter.LoadString(AFX_IDS_ALLFILTER))
			cAllFilter = "All files";
		::AppendFileType(strDescFilter,strExtFilter,cAllFilter,"*.*");
		//m_Dlg.m_ofn.nMaxCustFilter++;

		m_Dlg.m_ofn.hwndOwner = m_pParent ? m_pParent->m_hWnd : NULL;
		m_Dlg.m_ofn.lpstrFilter = strDescFilter;
		m_Dlg.m_ofn.lpstrTitle = m_szDialogTitle;
		m_Dlg.m_ofn.lpstrInitialDir = m_szInitialDir;

		return(m_Dlg.DoModal());
	}
	else
	{
		::MessageBox(m_pParent ? m_pParent->m_hWnd : NULL,"No supported types.",m_szDialogTitle,MB_OK|MB_ICONERROR|(m_pParent ? MB_APPLMODAL : MB_TASKMODAL)|MB_SETFOREGROUND|MB_TOPMOST);
		return(IDCANCEL);
	}
}

/*
	SaveAs()

	Salva il file di input nel file di output secondo il formato specificato.
	Utilizza l'oggetto immagine creato in proprio per caricare l'immagine di input e salvarla nel formato di output.
*/
BOOL CImageSaveAsDialog::SaveAs(LPCSTR lpcszInput,LPCSTR lpcszOutput,LPCSTR lpcszFormat)
{
	BOOL bSaved = FALSE;

	if(m_pImage->Load(lpcszInput))
		bSaved = m_pImage->Save(lpcszOutput,lpcszFormat);

	return(bSaved);
}

/*
	SaveAs()

	Salva il file secondo il formato specificato.
	Utilizza l'oggetto immagine ricevuto in input.
*/
BOOL CImageSaveAsDialog::SaveAs(LPCSTR lpcszOutput,LPCSTR lpcszFormat)
{
	return(m_pImage ? m_pImage->Save(lpcszOutput,lpcszFormat) : FALSE);
}

/*
	CImageOpenDialog()
*/
CImageOpenDialog::CImageOpenDialog(UINT nDialogID,UINT nStaticDibID,UINT nStaticInfo,CWnd* pParent/*=NULL*/,LPCSTR lpcszLibraryName/*=NULL*/,LPCSTR lpcszInitialDir/*= ".\\"*/,LPCSTR lpcszDialogTitle/*=NULL*/) : CPictureDialog(nDialogID,nStaticDibID,nStaticInfo,lpcszLibraryName,TRUE/*bOpenFileDialog*/,NULL/*lpcszDefExt*/,NULL/*lpcszFileName*/,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY/*dwFlags*/)
{
	m_pParent = pParent;

	if(lpcszLibraryName)
		strcpyn(m_szLibraryName,lpcszLibraryName,sizeof(m_szLibraryName));
	else
		memset(m_szLibraryName,'\0',sizeof(m_szLibraryName));
	m_pImage = m_ImageFactory.Create(m_szLibraryName,sizeof(m_szLibraryName));
	
	strcpyn(m_szInitialDir,lpcszInitialDir,sizeof(m_szInitialDir));

	if(lpcszDialogTitle)
		strcpyn(m_szDialogTitle,lpcszDialogTitle,sizeof(m_szDialogTitle));
	else
		strcpyn(m_szDialogTitle,"Open File...",sizeof(m_szDialogTitle));

	memset(m_szFileName,'\0',sizeof(m_szFileName));
}

/*
	DoModal()
*/
int CImageOpenDialog::DoModal(void)
{
	IMAGE_TYPE type = NULL_PICTURE;
	IMAGE_TYPE prevtype = NULL_PICTURE;
	LPIMAGETYPE p;
	CString strDesc = "";
	CString strExt = "";
	CString strDescFilter = "";
	CString strExtFilter = "";
	int nTotFormats = 0;

	// per i tipi supportati dalla libreria corrente
	while((p = m_pImage->EnumReadableImageFormats())!=(LPIMAGETYPE)NULL)
	{
		type = p->type;
		
		// costruisce il testo per il filtro
		if(prevtype!=NULL_PICTURE && type!=prevtype)
		{
			strDesc += ")";
			::AppendFileType(strDescFilter,strExtFilter,strDesc,strExt);
			CPictureDialog::m_ofn.nMaxCustFilter++;
			prevtype = type;
			strDesc.Format("%s(*%s",p->desc,p->ext);
			strExt.Format("*%s",p->ext);
		}
		else
		{
			prevtype = type;
			
			if(strDesc!="")
			{
				strDesc += "; *";
				strDesc += p->ext;
			}
			else
				strDesc.Format("%s(*%s",p->desc,p->ext);
			
			if(strExt!="")
			{
				strExt += "; *";
				strExt += p->ext;
			}
			else
				strExt.Format("*%s",p->ext);
		}
		
		nTotFormats++;
	}

	if(nTotFormats > 0)
	{
		// chiude il testo per il filtro
		strDesc += ")";
		::AppendFileType(strDescFilter,strExtFilter,strDesc,strExt);
		CPictureDialog::m_ofn.nMaxCustFilter++;

		if(m_FileExtList.Count() > 0)
		{
			ITERATOR iter;
			FILEEXT* f;

			if((iter = m_FileExtList.First())!=(ITERATOR)NULL)
			{
				do
				{
					f = (FILEEXT*)iter->data;
					if(f)
					{
						::AppendFileType(strDescFilter,strExtFilter,f->desc,f->ext);
						CPictureDialog::m_ofn.nMaxCustFilter++;
					}

					iter = m_FileExtList.Next(iter);
				
				} while(iter!=(ITERATOR)NULL);
			}
		}

		// aggiunge il tipo di default
		CString cExtAll = strExtFilter;
		::AppendFileType(strDescFilter,strExtFilter,"All supported formats",(LPCTSTR)cExtAll);
		CPictureDialog::m_ofn.nMaxCustFilter++;
		CPictureDialog::m_ofn.nFilterIndex = CPictureDialog::m_ofn.nMaxCustFilter;

		// aggiunge il tipo "*.*"
		CString cAllFilter;
		if(!cAllFilter.LoadString(AFX_IDS_ALLFILTER))
			cAllFilter = "All files";
		::AppendFileType(strDescFilter,strExtFilter,cAllFilter,"*.*");
		//m_Dlg.m_ofn.nMaxCustFilter++;

		CPictureDialog::m_ofn.hwndOwner = m_pParent ? m_pParent->m_hWnd : NULL;
		CPictureDialog::m_ofn.lpstrFilter = strDescFilter;
		CPictureDialog::m_ofn.lpstrFile = m_szFileName;
		CPictureDialog::m_ofn.lpstrTitle = m_szDialogTitle;
		CPictureDialog::m_ofn.lpstrInitialDir = m_szInitialDir;

		return(CPictureDialog::DoModal());
	}
	else
	{
		::MessageBox(m_pParent ? m_pParent->m_hWnd : NULL,"No supported types.",m_szDialogTitle,MB_OK|MB_ICONERROR|(m_pParent ? MB_APPLMODAL : MB_TASKMODAL)|MB_SETFOREGROUND|MB_TOPMOST);
		return(IDCANCEL);
	}
}

/*
	AddFileType()
*/
BOOL CImageOpenDialog::AddFileType(LPCSTR lpcszType,LPCSTR lpcszExt)
{
	BOOL bAdded = FALSE;
	FILEEXT* f = new FILEEXT;

	if(f)
	{
		strcpyn(f->ext,lpcszExt,_MAX_EXT+1);
		strcpyn(f->desc,lpcszType,_MAX_PATH+1);
		m_FileExtList.Add(f);
	}

	return(bAdded);
}
