/*
	CFindFile.cpp
	Classe base per la ricerca files (SDK/MFC).
	Luca Piergentili, 14/02/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CNodeList.h"
#include "CDateTime.h"
#include "CFindFile.h"

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
	CFindFile()
*/
CFindFile::CFindFile()
{
	m_enumDateFormat = UTC_TIME;
	m_nCount = 0;
	m_nInternalCount = -1;
	m_bFindFirstCall = TRUE;
	m_bFindExFirstCall = TRUE;
	memset(&m_stFindFile,'\0',sizeof(FINDFILE));
	m_stFindFile.handle = INVALID_HANDLE_VALUE;
	memset(&m_stWin32FindData,'\0',sizeof(WIN32_FIND_DATA));
	m_lpfnCallBack = NULL;
}

/*
	~CFindFile()
*/
CFindFile::~CFindFile()
{
	Reset();
}

/*
	ExistEx()
	
	Controlla se lo skeleton esiste.
	Passare il pathname completo + lo skeleton ("c:\*.exe" or "c:\file.exe").
*/
BOOL CFindFile::ExistEx(LPCSTR lpcszSkel)
{    
	BOOL bExist = FALSE;
    	
	if(CFindFile::First(lpcszSkel,_A_NORMAL|_A_RDONLY|_A_HIDDEN|_A_SYSTEM))
		bExist = TRUE;

	CFindFile::Close();

	return(bExist);
}

/*
	Find()
	
	Ricerca lo skeleton specificato.
	Passare il pathname completo + lo skeleton ("c:\*.exe" or "c:\file.exe").
	Chiamare in un ciclo fino a che non restituisce NULL.
*/
LPCSTR CFindFile::Find(LPCSTR lpcszSkel/* = "\\*.*" */)
{    
	char* p;
	LPSTR lpFile = NULL;
	static char szPathName[_MAX_PATH+1];
	static char szFileName[_MAX_PATH+1];

	// ricava il pathname
	memset(szPathName,'\0',sizeof(szPathName));
	strcpyn(szFileName,lpcszSkel,sizeof(szFileName));
	strrev(szFileName);
	if((p = strchr(szFileName,'\\'))==(char*)NULL)
		p = strchr(szFileName,':');
	if(p)
	{
		strcpyn(szPathName,p,sizeof(szPathName));
		strrev(szPathName);
	}
	memset(szFileName,'\0',sizeof(szFileName));
    	
	// cerca la prima istanza dello skeleton
	if(m_bFindFirstCall)
	{
		m_bFindFirstCall = FALSE;
		m_nCount = 0;
	     
		// i files trovati corrispondono allo skeleton
		if(CFindFile::First(lpcszSkel,_A_NORMAL|_A_RDONLY|_A_HIDDEN|_A_SYSTEM))
		{
			if(m_stFindFile.attrib & _A_SUBDIR)
				goto next;

			strcpyn(szFileName,m_stFindFile.name,sizeof(szFileName));
			strcat(szPathName,szFileName);
			lpFile = szPathName;
			m_nCount++;

			// se e' stato specificato un pathname completo e non uno skeleton ("c:\\file.ext"), termina la ricerca
			if(strchr(lpcszSkel,'?')==(char*)NULL && strchr(lpcszSkel,'*')==(char*)NULL)
			{
				CFindFile::Close();
				m_bFindFirstCall = TRUE;
			}
		}
		else
		{
			CFindFile::Close();
			m_bFindFirstCall = TRUE;
		}
    }
    else // cerca le istanze successive dello skeleton
    {
next:
		// trovato il file seguente
		if(CFindFile::Next())
		{
			// esclude "." e ".."
			if(m_stFindFile.attrib & _A_SUBDIR)
				goto next;

			strcpyn(szFileName,m_stFindFile.name,sizeof(szFileName));
			strcat(szPathName,szFileName);
			lpFile = szPathName;
			m_nCount++;
		}
		else
		{
			CFindFile::Close();
			m_bFindFirstCall = TRUE;
		}
	}

	return(lpFile);
}

/*
	FindEx()
	
	Cerca lo skeleton specificato a partire dalla directory iniziale (terminare con '\').
	Chiamare in un ciclo fino a che non restituisce NULL.
*/
LPCSTR CFindFile::FindEx(LPCSTR lpcszStartDir/* = "\\" */,LPCSTR lpcszSkel/* = "*.*" */,BOOL bRecursive/* = TRUE */,UINT uAttribute/* = _A_ALLFILES */)
{
	LPSTR lpFile = NULL;

	// creates the list
	if(m_bFindExFirstCall)
	{
		m_nCount = 0;
		m_listFileNames.RemoveAll();

		// cerca lo skeleton		
		HANDLE hHandle;
		if((hHandle = CFindFile::Search(lpcszStartDir,lpcszSkel,bRecursive,uAttribute))==INVALID_HANDLE_VALUE)
			return(NULL);
		
		m_bFindExFirstCall = FALSE;
		CFindFile::Close(hHandle);

		m_nCount = m_listFileNames.Count();
	}

	// restituisce quanto presente nella lista
	if(!m_bFindExFirstCall)
	{
		if(m_listFileNames.Count() > 0)
		{
			if(m_nInternalCount==-1)
				m_nInternalCount = 0;

			FINDFILE* f;
			if((f = (FINDFILE*)m_listFileNames.GetAt(m_nInternalCount))!=(FINDFILE*)NULL)
			{
				lpFile = f->name;
				m_nInternalCount++;
			}
			else
			{
				lpFile = NULL;
				m_listFileNames.RemoveAll();
				m_bFindExFirstCall = TRUE;
				m_nInternalCount = -1;
			}
		}
		else
		{
			m_bFindExFirstCall = TRUE;
			m_nInternalCount = -1;
		}
	}

	return(lpFile);
}

/*
	FindFile()
	
	Cerca lo skeleton specificato a partire dalla directory iniziale (terminare con '\').
	I files trovati vengono inseriti nella lista interna e devono essere recuperati con la Get...().
*/
UINT CFindFile::FindFile(LPCSTR lpcszStartDir/* = "\\" */,LPCSTR lpcszSkel/* = "*.*" */,BOOL bRecursive/* = TRUE */,UINT uAttribute/* = _A_ALLFILES */)
{
	m_listFileNames.RemoveAll();
		
	// cerca lo skeleton
	HANDLE hHandle;
	if((hHandle = CFindFile::Search(lpcszStartDir,lpcszSkel,bRecursive,uAttribute))!=INVALID_HANDLE_VALUE)
		CFindFile::Close(hHandle);

	m_nCount = m_listFileNames.Count();

	return(m_nCount);
}

/*
	GetFileName()
	
	Restituisce il nome file relativo all'elemento della lista interna.
	Chiamare in un ciclo che vada da 0 a Count().
*/
LPCSTR CFindFile::GetFileName(int nIndex)
{
	LPSTR lpFile = NULL;
	
	if(m_listFileNames.Count() > 0)
	{
		if(nIndex >= 0 && nIndex < m_listFileNames.Count())
		{
			FINDFILE* f;
			if((f = (FINDFILE*)m_listFileNames.GetAt(nIndex))!=(FINDFILE*)NULL)
				lpFile = f->name;
		}
	}

	return(lpFile);
}

/*
	GetFindFile()
	
	Restituisce l'elemento della lista interna.
	Chiamare in un ciclo che vada da 0 a Count().
*/
FINDFILE* CFindFile::GetFindFile(int nIndex)
{
	FINDFILE* f = NULL;
	
	if(m_listFileNames.Count() > 0)
		if(nIndex >= 0 && nIndex < m_listFileNames.Count())
			f = (FINDFILE*)m_listFileNames.GetAt(nIndex);

	return(f);
}

/*
	Search()
	
	Cerca lo skeleton specificato a partire dalla directory iniziale (terminare con '\').
	I files trovati vengono inseriti nella lista interna.
*/
HANDLE CFindFile::Search(LPCSTR lpcszStartDir,LPCSTR lpcszSkel,BOOL bRecursive,UINT uAttribute)
{
	FINDFILE f;
	char szPathName[_MAX_FILEPATH+1];

	if(lpcszStartDir[strlen(lpcszStartDir)-1]!='\\')
		return(INVALID_HANDLE_VALUE);

	if((strlen(lpcszStartDir) + strlen(lpcszSkel)) < sizeof(szPathName))
		_snprintf(szPathName,sizeof(szPathName)-1,"%s%s",lpcszStartDir,lpcszSkel);
	else
		return(INVALID_HANDLE_VALUE);

	// cerca la prima istanza dello skeleton
	if(CFindFile::First(szPathName,uAttribute,&f))
	{
		do
		{
			// considera solo i nomi file
			if(f.name[0]!='.')
			{
				// pathname completo
				if((strlen(lpcszStartDir) + strlen(f.name)) < sizeof(szPathName))
					_snprintf(szPathName,sizeof(szPathName)-1,"%s%s",lpcszStartDir,f.name);
				else
					return(INVALID_HANDLE_VALUE);

				// inserisce il nome file nella lista
				FINDFILE* ff = (FINDFILE*)m_listFileNames.Add();
				if(ff)
				{
					ff->attrib = f.attrib;
					memcpy(&ff->datetime,&f.datetime,sizeof(SYSTEMTIME));
					ff->size = f.size;
					strcpyn(ff->name,szPathName,_MAX_FILEPATH+1);

					if(m_lpfnCallBack)
						m_lpfnCallBack(0,(LPARAM)ff);
					else
						::PeekAndPump();
				}
			}
		}
		while(CFindFile::Next(&f));
	}

	if(bRecursive)
	{
		// pathname completo
		if((strlen(lpcszStartDir) + 3) < sizeof(szPathName))
			_snprintf(szPathName,sizeof(szPathName)-1,"%s*.*",lpcszStartDir);
		else
			return(INVALID_HANDLE_VALUE);
		
		// cerca la prima istanza dello skeleton
		if(CFindFile::First(szPathName,_A_SUBDIR,&f))
		{
			do
			{
				// cerca nelle subdir
				if((f.attrib & _A_SUBDIR) && f.name[0]!='.')
				{
					// pathname completo
					if((strlen(lpcszStartDir) + strlen(f.name) + 1) < sizeof(szPathName))
						_snprintf(szPathName,sizeof(szPathName)-1,"%s%s\\",lpcszStartDir,f.name);
					else
						return(INVALID_HANDLE_VALUE);
					
					// ricerca ricorsivamente
					CFindFile::Search(szPathName,lpcszSkel,bRecursive,uAttribute);
				}
			}
			while(CFindFile::Next(&f));
		}
	}

	if(m_lpfnCallBack)
		m_lpfnCallBack(1,NULL);

	return(f.handle);
}

/*
	First()

	Cerca la prima istanza del nome file.
	Il terzo parametro e' opzionale perche per la ricerca ricorsiva deve essere usata la struttura del chiamante, non quella della classe.
*/
BOOL CFindFile::First(LPCSTR lpcszFileName,UINT /*uAttribute*/,FINDFILE* f/* = NULL */)
{
	HANDLE hHandle = INVALID_HANDLE_VALUE;

	if(!f)
	{
		memset(&m_stFindFile,'\0',sizeof(FINDFILE));
		m_stFindFile.handle = INVALID_HANDLE_VALUE;
	}
	else
	{
		memset(f,'\0',sizeof(FINDFILE));
		f->handle = INVALID_HANDLE_VALUE;
	}

	if((hHandle = ::FindFirstFile(lpcszFileName,&m_stWin32FindData))!=INVALID_HANDLE_VALUE)
	{
		FILETIME localfiletime;
		
		if(!f)
		{
			// struttura della classe
			strcpyn(m_stFindFile.name,m_stWin32FindData.cFileName,sizeof(m_stFindFile.name));
			m_stFindFile.handle  = hHandle;
			m_stFindFile.size    = m_stWin32FindData.nFileSizeLow;
			if(m_enumDateFormat==LOCAL_TIME)
			{
				::FileTimeToLocalFileTime(&m_stWin32FindData.ftLastWriteTime,&localfiletime);
				::FileTimeToSystemTime(&localfiletime,&m_stFindFile.datetime);
			}
			else if(m_enumDateFormat==UTC_TIME)
			{
				::FileTimeToSystemTime(&m_stWin32FindData.ftLastWriteTime,&m_stFindFile.datetime);
			}
			m_stFindFile.attrib  = (UINT)m_stWin32FindData.dwFileAttributes;
		}
		else
		{
			// struttura del chiamante
			strcpyn(f->name,m_stWin32FindData.cFileName,_MAX_FILEPATH+1);
			f->handle  = hHandle;
			f->size    = m_stWin32FindData.nFileSizeLow;
			if(m_enumDateFormat==LOCAL_TIME)
			{
				::FileTimeToLocalFileTime(&m_stWin32FindData.ftLastWriteTime,&localfiletime);
				::FileTimeToSystemTime(&localfiletime,&f->datetime);
			}
			else if(m_enumDateFormat==UTC_TIME)
			{
				::FileTimeToSystemTime(&m_stWin32FindData.ftLastWriteTime,&f->datetime);
			}
			f->attrib  = (UINT)m_stWin32FindData.dwFileAttributes;
		}
	}

	return(hHandle!=INVALID_HANDLE_VALUE);
}

/*
	Next()
	
	Cerca l'istanza successiva del nome file.
*/
BOOL CFindFile::Next(FINDFILE* f/* = NULL */)
{
	HANDLE hHandle = (f!=(FINDFILE*)NULL ? f->handle : m_stFindFile.handle);

	if(hHandle==INVALID_HANDLE_VALUE)
		return(FALSE);

	if(!::FindNextFile(hHandle,&m_stWin32FindData))
		return(FALSE);

	FILETIME localfiletime;

	if(!f)
	{
		// struttura della classe
		strcpyn(m_stFindFile.name,m_stWin32FindData.cFileName,sizeof(m_stFindFile.name));
		m_stFindFile.size    = m_stWin32FindData.nFileSizeLow;
		if(m_enumDateFormat==LOCAL_TIME)
		{
			::FileTimeToLocalFileTime(&m_stWin32FindData.ftLastWriteTime,&localfiletime);
			::FileTimeToSystemTime(&localfiletime,&m_stFindFile.datetime);
		}
		else if(m_enumDateFormat==UTC_TIME)
		{
			::FileTimeToSystemTime(&m_stWin32FindData.ftLastWriteTime,&m_stFindFile.datetime);
		}
		m_stFindFile.attrib  = (UINT)m_stWin32FindData.dwFileAttributes;
	}
	else
	{
		// struttura del chiamante
		strcpyn(f->name,m_stWin32FindData.cFileName,_MAX_FILEPATH+1);
		f->size    = m_stWin32FindData.nFileSizeLow;
		if(m_enumDateFormat==LOCAL_TIME)
		{
			::FileTimeToLocalFileTime(&m_stWin32FindData.ftLastWriteTime,&localfiletime);
			::FileTimeToSystemTime(&localfiletime,&f->datetime);
		}
		else if(m_enumDateFormat==UTC_TIME)
		{
			::FileTimeToSystemTime(&m_stWin32FindData.ftLastWriteTime,&f->datetime);
		}
		f->attrib  = (UINT)m_stWin32FindData.dwFileAttributes;
	}

	return(TRUE);
}

/*
	Close()
	
	Chiude la ricerca.
*/
BOOL CFindFile::Close(HANDLE h)
{
	BOOL bClosed = FALSE;

	HANDLE hHandle = (h!=INVALID_HANDLE_VALUE ? h : m_stFindFile.handle);

	if(hHandle!=INVALID_HANDLE_VALUE)
	{
		bClosed = ::FindClose(hHandle);
		m_stFindFile.handle = INVALID_HANDLE_VALUE;
	}
	
	return(bClosed);
}

/*
	SetFileTime()

	Imposta data/ora del file.
*/
BOOL CFindFile::SetFileTime(LPCSTR lpcszFileName,WORD uDate,WORD uTime)
{
	BOOL bSet = FALSE;
	HANDLE hHandle;
	FILETIME filetime = {0};

	if((hHandle = ::CreateFile(lpcszFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		if(::DosDateTimeToFileTime(uDate,uTime,&filetime))
			bSet = ::SetFileTime(hHandle,NULL,NULL,&filetime);

		::CloseHandle(hHandle);
	}

	return(bSet);
}

/*
	GetFileTime()

	Ricava data/ora del file.
*/
BOOL CFindFile::GetFileTime(LPCSTR lpcszFileName,LPWORD lpuDate,LPWORD lpuTime)
{
	BOOL bGet = FALSE;
	HANDLE hHandle;
	FILETIME filetime = {0};

	*lpuDate = *lpuTime = 0;

	if((hHandle = ::CreateFile(lpcszFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		if(::GetFileTime(hHandle,NULL,NULL,&filetime))
			bGet = (BOOL)::FileTimeToDosDateTime(&filetime,lpuDate,lpuTime);

		::CloseHandle(hHandle);
	}

	return(bGet);
}


/*
	EnsureValidFileName()
	
	Elimina i caratteri non consentiti.
	Considera solo il nome file, non usare per i pathnames.
*/
LPCSTR CFindFile::EnsureValidFileName(LPCSTR lpcszFileName,LPSTR lpszNewName,UINT cbNewName)
{
	int i=0,n=0,cb=0;
	
	memset(lpszNewName,'\0',cbNewName);
	if(isalpha(lpcszFileName[0]) && lpcszFileName[1]==':' && lpcszFileName[2]=='\\')
	{
		lpszNewName[i++] = lpcszFileName[n++];
		lpszNewName[i++] = lpcszFileName[n++];
		lpszNewName[i++] = lpcszFileName[n++];
		cb = i;
	}
	for(; lpcszFileName[n] && i < (int)(cbNewName-1-cb); n++)
		if(!strchr("\\/:*?\"'<>|",lpcszFileName[n]))
			lpszNewName[i++] = lpcszFileName[n];
	
	strltrim(lpszNewName);
	strrtrim(lpszNewName);
	strstrim(lpszNewName);
	
	return(lpszNewName);
}

/*
	SplitPathName()

	Divide il pathname nei suoi componenti.
*/
void CFindFile::SplitPathName(LPCSTR lpcszPathName,LPSTR lpszDirectory,UINT cbDirectory,LPSTR lpszFileName,UINT cbFileName,BOOL bUseCurrenDirectory/* = TRUE*/)
{
	char* p;
	
	memset(lpszDirectory,'\0',cbDirectory);
	memset(lpszFileName,'\0',cbFileName);

	// controlla se il pathname contiene una directory
	if((p = (char*)strchr(lpcszPathName,'\\'))==(char*)NULL)
		p = (char*)strchr(lpcszPathName,':');

	if(p)
	{
		char szBuffer[_MAX_PATH+1];
		
		strcpyn(szBuffer,lpcszPathName,sizeof(szBuffer));
		strrev(szBuffer);
		
		if((p = strchr(szBuffer,'\\'))==(char*)NULL)
			p = strchr(szBuffer,':');

		strcpyn(lpszDirectory,p,cbDirectory);
		strrev(lpszDirectory);
		*p = '\0';
		
		strcpyn(lpszFileName,szBuffer,cbFileName);
		strrev(lpszFileName);
	}
	else // nessuna directory, ricava la corrente
	{
		if(bUseCurrenDirectory)
			::GetCurrentDirectory(cbDirectory,lpszDirectory);
		strcpyn(lpszFileName,lpcszPathName,cbFileName);
	}

	if(!strnull(lpszDirectory))
		if((lpszDirectory[strlen(lpszDirectory)-1]!='\\') && ((int)(strlen(lpszDirectory)+1) < cbDirectory))
			strcat(lpszDirectory,"\\");
}

/*
	CreatePathName()

	Crea il pathname.
*/
BOOL CFindFile::CreatePathName(LPSTR lpszPathName,UINT cbPathName/* = (UINT)-1*/)
{    
	int i,n;
	char* p;
	FINDFILE* f;
	CFindFileList listDirectoryNames;
	char szDrive[_MAX_DRIVE+1+1];
	char szDirectory[1024] = {0};
	DWORD dwAttribute = 0L;
	BOOL bCreated = FALSE;

	// lavora sulla copia locale
	// se viene specificata la dimensione del buffer, effettua il controllo sulla validita' dei caratteri
	if(cbPathName!=(UINT)-1)
	{
		i = n = 0;
		if(isalpha(lpszPathName[0]) && lpszPathName[1]==':' && lpszPathName[2]=='\\')
		{
			szDirectory[i++] = lpszPathName[n++];
			szDirectory[i++] = lpszPathName[n++];
			szDirectory[i++] = lpszPathName[n++];
		}
		for(; lpszPathName[n] && i <= sizeof(szDirectory)-1; n++)
			if(!strchr("/:*?\"<>|",lpszPathName[n]))
				szDirectory[i++] = lpszPathName[n];
	}
	else
		strcpyn(szDirectory,lpszPathName,sizeof(szDirectory));
	
	strltrim(szDirectory);
	strrtrim(szDirectory);
	strstrim(szDirectory);
	::EnsureBackslash(szDirectory,sizeof(szDirectory));

	// non crea il pathname se gia' esiste, se si tratta di un file restituisce errore
	if((dwAttribute = ::GetFileAttributes(szDirectory))!=(DWORD)-1L)
	{
		bCreated = (dwAttribute & FILE_ATTRIBUTE_DIRECTORY) ? TRUE : FALSE;
		goto done;
	}
		
	memset(szDrive,'\0',sizeof(szDrive));
	if((p = strchr(szDirectory,':'))!=(char*)NULL)
	{
		szDrive[0] = *(p-1);
		szDrive[1] = *(p);
		
		if(*(p+1)=='\\')
		{
			szDrive[2] = '\\';
			szDrive[3] = '\0';
		}
		else
			szDrive[2] = '\0';
	}
	else
	{
		// imposta il drive solo per i pathname assoluti
		if(szDirectory[0]=='\\')
		{
			char szBuffer[_MAX_PATH+1];
			if(::GetCurrentDirectory(sizeof(szBuffer)-1,szBuffer)!=0)
			{
				szDrive[0] = szBuffer[0];
				szDrive[1] = ':';
				szDrive[2] = '\\';
				szDrive[3] = '\0';
			}
			else
			{
				bCreated = FALSE;
				goto done;
			}
		}
	}
	
	// rimuove l'identificatore del drive
	strrev(szDirectory);
	if((p = strchr(szDirectory,':'))!=(char*)NULL)
		*p = '\0';
	strrev(szDirectory);

	// crea la lista con le directory contenute nel pathname
	listDirectoryNames.RemoveAll();
	
	// il pathname deve contenere almeno una directory
	if(strchr(szDirectory,'\\')!=(char*)NULL)
	{
		char* token = strtok(szDirectory,"\\");
		
		for(int i = 0; token!=(char*)NULL; i++)
		{
			f = (FINDFILE*)listDirectoryNames.Add();
			if(f)
			{
				strcpyn(f->name,token,_MAX_FILEPATH+1);
				token = strtok((char*)NULL,"\\");
			}
		}

		szDirectory[0] = '\0';
	}

	// identificativo del drive
	if(*szDrive)
		strcpyn(szDirectory,szDrive,sizeof(szDirectory));
	
	// crea il pathname
	for(i = 0; i < listDirectoryNames.Count(); i++)
	{
		if((f = (FINDFILE*)listDirectoryNames.GetAt(i))!=(FINDFILE*)NULL)
		{
			strcat(szDirectory,f->name);

			if(::GetFileAttributes(szDirectory)==(DWORD)-1L)
				::CreateDirectory(szDirectory,NULL);
			
			strcat(szDirectory,"\\");
		}
	}

	listDirectoryNames.RemoveAll();

done:

	if(cbPathName!=(UINT)-1)
		strcpyn(lpszPathName,szDirectory,cbPathName);

	return(bCreated ? bCreated : (::GetFileAttributes(szDirectory)!=(DWORD)-1L));
}
