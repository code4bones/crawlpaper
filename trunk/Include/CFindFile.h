/*
	CFindFile.h
	Classe base per la ricerca files (SDK/MFC).
	Luca Piergentili, 14/02/00
	lpiergentili@yahoo.com
*/
#ifndef _CFINDFILE_H
#define _CFINDFILE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "window.h"
#include "win32api.h"
#include "CNodeList.h"
#include "CDateTime.h"

#define _A_ALLFILES (_A_NORMAL|_A_RDONLY|_A_HIDDEN|_A_SYSTEM)
#define _A_ALLFILESANDDIRECTORIES (_A_NORMAL|_A_RDONLY|_A_HIDDEN|_A_SYSTEM|_A_SUBDIR|_A_ARCH)

/*
	PFNFINDFILECALLBACK
	tipo per la callback
*/
typedef LONG (CALLBACK* PFNFINDFILECALLBACK)(WPARAM,LPARAM);

/*
	FINDFILE
	struttura per l'elemento della lista dei files
*/
struct FINDFILE {
	UINT			attrib;				// attributo
	SYSTEMTIME	datetime;				// data/ora
	DWORD		size;				// dimensione (in bytes)
	char			name[_MAX_FILEPATH+1];	// nome file (pathname incluso)
	HANDLE		handle;				// handle per findfirst/next
	WPARAM		wParam;
	LPARAM		lParam;
};

/*
	CFindFileList
	classe per la lista dei files
*/
class CFindFileList : public CNodeList
{
public:
	CFindFileList() : CNodeList() {}
	virtual ~CFindFileList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new FINDFILE);
	}
	void* Initialize(void* pVoid)
	{
		FINDFILE* pData = (FINDFILE*)pVoid;
		if(!pData)
			pData = (FINDFILE*)Create();
		if(pData)
			memset(pData,'\0',sizeof(FINDFILE));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((FINDFILE*)iter->data)
			delete ((FINDFILE*)iter->data),iter->data = (FINDFILE*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(FINDFILE));}
#ifdef _DEBUG
	const char* Signature(void) {return("CFindFileList");}
#endif
};

/*
	CFindFile
*/
class CFindFile
{
public:
	CFindFile();
	virtual ~CFindFile();

	// controlla l'esistenza
	inline BOOL	Exist			(LPCSTR lpcszFileName) {return(::FileExist(lpcszFileName));} // nome file
	BOOL			ExistEx			(LPCSTR lpcszSkel);				// nome file/skeleton

	// ricerca semplice (while)
	LPCSTR		Find				(LPCSTR lpcszSkel = "\\*.*");		// skeleton

	// ricerca ricorsiva (while)
	LPCSTR		FindEx			(LPCSTR lpcszStartDir = "\\",		// dir iniziale (terminare con '\')
								 LPCSTR lpcszSkel = "*.*",		// skeleton
								 BOOL bRecursive = TRUE,			// ricorsivo
								 UINT uAttribute = _A_ALLFILES);	// attributi

	// ricerca ricorsiva (lista)
	UINT			FindFile			(LPCSTR lpcszStartDir = "\\",		// dir iniziale (terminare con '\')
								 LPCSTR lpcszSkel = "*.*",		// skeleton
								 BOOL bRecursive = TRUE,			// ricorsivo
								 UINT uAttribute = _A_ALLFILES);	// attributo
	LPCSTR		GetFileName		(int nIndex);
	FINDFILE*		GetFindFile		(int nIndex);

	// files trovati
	inline UINT	Count			(void) const {return(m_nCount);}

	// ripulisce
	inline void	Reset			(void) {m_listFileNames.EraseAll(); Close();}

	// data/ora
	inline void	SetFileTimeFormat	(DATEFORMAT DateFormat) {m_enumDateFormat = DateFormat;}
	BOOL			SetFileTime		(LPCSTR lpcszFileName,WORD uDate,WORD uTime);
	BOOL			GetFileTime		(LPCSTR lpcszFileName,LPWORD lpuDate,LPWORD lpuTime);

	// pathnames
	LPCSTR		EnsureValidFileName	(LPCSTR lpcszFileName,LPSTR lpszNewName,UINT cbNewName);
	void			SplitPathName		(LPCSTR lpcszPathName,LPSTR lpszDirectory,UINT cbDirectory,LPSTR lpszFileName,UINT cbFileName,BOOL bUseCurrenDirectory = TRUE);
	static BOOL	CreatePathName		(LPSTR lpszPathName,UINT cbPathName = (UINT)-1);

	void			SetCallback		(PFNFINDFILECALLBACK lpfnCallBack) {m_lpfnCallBack = lpfnCallBack;}

private:
	HANDLE		Search			(
								LPCSTR	lpcszStartDir,
								LPCSTR	lpcszSkel,
								BOOL		bRecursive,
								UINT		uAttribute
								);
	
	BOOL			First			(
								LPCSTR	lpcszFileName,
								UINT		uAttribute,
								FINDFILE*	f = NULL
								);
		
	BOOL			Next				(FINDFILE* f = NULL);

	BOOL			Close			(HANDLE hHandle = INVALID_HANDLE_VALUE);

	DATEFORMAT					m_enumDateFormat;
	int							m_nCount;
	int							m_nInternalCount;
	BOOL							m_bFindFirstCall;
	BOOL							m_bFindExFirstCall;
	WIN32_FIND_DATA				m_stWin32FindData;
	FINDFILE						m_stFindFile;
	CFindFileList					m_listFileNames;
	PFNFINDFILECALLBACK				m_lpfnCallBack;
};

#endif // _CFINDFILE_H
