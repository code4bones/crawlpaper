/*
	CImageDialog.h
	Classi base per i dialoghi File|Apri|Salva per i formati relativi alle immagini (MFC).
	Luca Piergentili, 14/11/00
	lpiergentili@yahoo.com
*/
#ifndef _CIMAGEDIALOG_H
#define _CIMAGEDIALOG_H 1

#include "window.h"
#include "CImageFactory.h"
#include "CFileDialogEx.h"
#include "CNodeList.h"
#include "CPictureDialog.h"

/*
	FILEEXT
	struttura per l'elemento della lista
*/
struct FILEEXT {
	char	ext[_MAX_EXT+1];
	char	desc[_MAX_PATH+1];
};

/*
	CFileExtList
	classe per la gestione della lista
*/
class CFileExtList : public CNodeList
{
public:
	CFileExtList() : CNodeList() {}
	virtual ~CFileExtList() {CNodeList::DeleteAll();}
	
	void* Create(void)
	{
		FILEEXT* pData = new FILEEXT;
		return(pData);
	}
	
	void* Initialize(void* pVoid)
	{
		FILEEXT* pData = (FILEEXT*)pVoid;
		if(!pData)
			pData = (FILEEXT*)Create();
		if(pData)
			memset(pData,'\0',sizeof(FILEEXT));
		return(pData);
	}
	
	BOOL PreDelete(ITERATOR iter)
	{
		if((FILEEXT*)iter->data)
		{
			delete ((FILEEXT*)iter->data);
			iter->data = (FILEEXT*)NULL;
		}
		return(TRUE);
	}
	
	int Size(void) {return(sizeof(FILEEXT));}

#ifdef _DEBUG
	const char* Signature(void) {return("CFileExtList");}
#endif
};

/*
	CImageSaveAsDialog
	Classe per il dialogo "Save As" per i formati supportati.
*/
class CImageSaveAsDialog
{
public:
	CImageSaveAsDialog(	CWnd*	pParent = NULL,
					CImage*	pImage = NULL,
					LPCSTR	lpcszLibraryName = NULL,
					LPCSTR	lpcszInitialDir = ".\\",
					LPCSTR	lpcszDialogTitle = NULL
					);

	virtual ~CImageSaveAsDialog() {}

	int		DoModal		(void);
	BOOL		SaveAs		(LPCSTR lpcszInput,LPCSTR lpcszOutput,LPCSTR lpcszFormat);
	BOOL		SaveAs		(LPCSTR lpcszOutput,LPCSTR lpcszFormat);
	CString	GetFileName	(void) const {return(m_Dlg.GetFileName());}
	CString	GetPathName	(void) const {return(m_Dlg.GetPathName());}
	CString	GetFileExt	(void) const {return(m_Dlg.GetFileExt());}

private:
	CWnd*			m_pParent;
	char				m_szLibraryName[_MAX_PATH+1];
	CImageFactory		m_ImageFactory;
	CImage*			m_pImage;
	char				m_szInitialDir[_MAX_PATH+1];
	char				m_szDialogTitle[_MAX_PATH+1];
	CFileSaveAsDialog	m_Dlg;
};

/*
	CImageOpenDialog
	Classe per il dialogo "Open" per i formati supportati.
*/
class CImageOpenDialog : public CPictureDialog
{
public:
	CImageOpenDialog(	UINT		nDialogID,
					UINT		nStaticDibID,
					UINT		nStaticInfo,
					CWnd*	pParent = NULL,
					LPCSTR	lpcszLibraryName = NULL,
					LPCSTR	lpcszInitialDir = ".\\",
					LPCSTR	lpcszDialogTitle = NULL
					);

	virtual ~CImageOpenDialog() {}

	int	DoModal		(void);
	BOOL	AddFileType	(LPCSTR lpcszType,LPCSTR lpcszExt);

private:
	CWnd*		m_pParent;
	char			m_szLibraryName[_MAX_PATH+1];
	CImageFactory	m_ImageFactory;
	CImage*		m_pImage;
	char			m_szInitialDir[_MAX_PATH+1];
	char			m_szDialogTitle[_MAX_PATH+1];
	char			m_szFileName[_MAX_PATH+1];
	CFileExtList	m_FileExtList;
};

#endif // _CIMAGEDIALOG_H
