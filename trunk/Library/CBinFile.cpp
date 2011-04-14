/*
	CBinFile.cpp
	Classe base per l'accesso a files binari (SDK/MFC).
	Luca Piergentili, 31/08/98
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
#include "CDateTime.h"
#include "CFindFile.h"
#include "CBinFile.h"

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
	CBinFile()
*/
CBinFile::CBinFile()
{
//#ifdef _DEBUG
#if 0
	m_bShowErrors = TRUE;
#else
	m_bShowErrors = FALSE;
#endif
	m_hHandle = INVALID_HANDLE_VALUE;
	m_dwError = 0L;
	memset(m_szFileName,'\0',sizeof(m_szFileName));
	memset(m_szError,'\0',sizeof(m_szError));
}

/*
	~CBinFile()
*/
CBinFile::~CBinFile()
{
	CBinFile::Close();
}

/*
	Open()

	Apre il file.
	Notare che se non viene specificato il contrario, se il file non esiste lo crea.
*/
BOOL CBinFile::Open(LPCSTR	lpcszFileName,
				BOOL		bCreateIfNotExist/* = TRUE*/,
				DWORD	dwAccessMode/* = GENERIC_READ|GENERIC_WRITE*/,
				DWORD	dwShareMode/* = FILE_SHARE*/
				)
{
	BOOL bOpen = FALSE;

	if(m_hHandle==INVALID_HANDLE_VALUE)
	{
		strcpyn(m_szFileName,lpcszFileName,sizeof(m_szFileName));
		
		if((m_hHandle = ::CreateFile(lpcszFileName,dwAccessMode,dwShareMode,NULL,bCreateIfNotExist ? OPEN_ALWAYS : OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE)
			SetLastErrorCode(::GetLastError());
		else
			bOpen = TRUE;
	}

	return(bOpen);
}

/*
	Open()

	Apre il file.
	Il lock va fatto manualmente prima delle operazioni di scrittura.
*/
BOOL CBinFileLock::Open(	LPCSTR	lpcszFileName,
					BOOL		bCreateIfNotExist/* = FALSE*/,
					DWORD	dwAccessMode/* = GENERIC_READ|GENERIC_WRITE*/,
					DWORD	dwShareMode/* = FILE_SHARE*/
					)
{
	CSyncThreads::SetName(lpcszFileName);
	CSyncThreads::SetTimeout(SYNC_5_SECS_TIMEOUT);
	return(CBinFile::Open(lpcszFileName,bCreateIfNotExist,dwAccessMode,dwShareMode));
}

/*
	Create()

	Crea il file.
	Se il file gia' esiste non fallisce ma lo azzera.
	Se la directory presente nel pathname non esiste la crea.
*/
BOOL CBinFile::Create(	LPCSTR	lpcszFileName,
					DWORD	dwAccessMode/* = GENERIC_READ|GENERIC_WRITE*/,
					DWORD	dwShareMode/* = FILE_SHARE*/,
					DWORD	dwAttribute/* = FILE_ATTRIBUTE_NORMAL*/
					)
{
	BOOL bCreated = FALSE;

	if(m_hHandle==INVALID_HANDLE_VALUE || m_hHandle==NULL)
	{
		// se il nomefile contiene un pathname verifica che esista, in caso contrario lo crea
		char* p = NULL;
		char szDirectory[1024] = {0};
		strcpyn(szDirectory,lpcszFileName,sizeof(szDirectory));
		if((p = strrchr(szDirectory,'\\'))!=NULL)
		{
			if(*(p+1)) p++;
			if(*p) *p = '\0';
			CFindFile::CreatePathName(szDirectory,sizeof(szDirectory));
		}

		strcpyn(m_szFileName,lpcszFileName,sizeof(m_szFileName));

		if((m_hHandle = ::CreateFile(lpcszFileName,dwAccessMode,dwShareMode,NULL,CREATE_ALWAYS,dwAttribute,NULL))==INVALID_HANDLE_VALUE)
			SetLastErrorCode(::GetLastError());
		else
			bCreated = TRUE;
	}

	return(bCreated);
}

/*
	Close()

	Chiude il file.
*/
BOOL CBinFile::Close(void)
{
	BOOL bClose = FALSE;

	if(m_hHandle!=INVALID_HANDLE_VALUE)
	{
		if(::CloseHandle(m_hHandle))
		{
			bClose = TRUE;
			m_hHandle = INVALID_HANDLE_VALUE;
		}
		else
			SetLastErrorCode(::GetLastError());
	}

	return(bClose);
}

/*
	Read()

	Legge dal file il numero specificato di bytes nel buffer.
	Restituisce il numero di byte letti o FILE_EOF per errore.
*/
DWORD CBinFile::Read(LPVOID lpBuffer,DWORD dwToRead)
{
	DWORD dw = 0L;

	if(m_hHandle!=INVALID_HANDLE_VALUE)
	{
		if(!::ReadFile(m_hHandle,lpBuffer,dwToRead,&dw,NULL))
		{
			dw = FILE_EOF;
			SetLastErrorCode(::GetLastError());
		}
	}

	return(dw);
}

/*
	Write()

	Scrive nel file il contenuto del buffer per il numero specificato di bytes.
	Restituisce il numero di bytes scritti o FILE_EOF per errore.
*/
DWORD CBinFile::Write(LPCVOID lpcBuffer,DWORD dwToWrite)
{
	DWORD dw = 0L;

	if(dwToWrite > 0L)
		if(m_hHandle!=INVALID_HANDLE_VALUE)
		{
			if(!::WriteFile(m_hHandle,lpcBuffer,dwToWrite,&dw,NULL))
			{
				dw = FILE_EOF;
				SetLastErrorCode(::GetLastError());
			}
		}

	return(dw);
}

/*
	WriteEx()

	Scrive nel file il contenuto del buffer per il numero specificato di bytes.
	Restituisce il numero di bytes scritti o FILE_EEOF per errore.
*/
QWORD CBinFileEx::WriteEx(LPCVOID lpcBuffer,QWORD qwToWrite)
{
	QWORD qw = 0L;

	if(qwToWrite > 0L)
		if(m_hHandle!=INVALID_HANDLE_VALUE)
		{
			DWORD dw = 0L;
			QWORD qwTot = 0L;
			QWORD qwAmount = 32768; // il valore del buffer non puo' essere maggiore di DWORD, valore massimo per Write()
			char* pFileContent = (char*)lpcBuffer;
			
			if(qwAmount > qwToWrite)
			{
				qw = (QWORD)Write(pFileContent,(DWORD)qwToWrite);
			}
			else
			{
				do
				{
					if((dw = Write(pFileContent,(DWORD)qwAmount))!=FILE_EOF)
					{
						qwTot += dw;
						pFileContent += qwAmount;
						if(qwToWrite - qwTot <= qwAmount)
							qwAmount = qwToWrite - qwTot;
					}
					else
						break;
				}
				while(qwTot < qwToWrite);
			}
		}

	return(qw);
}

/*
	Seek()

	Posiziona il puntatore all'interno del file.
	Specificare l'offset ed il punto (FILE_BEGIN/CURRENT/END) a partire dal quale posizionare il puntatore.
	Restituisce FILE_EOF per errore.
*/
DWORD CBinFile::Seek(LONG lOffset,DWORD dwOrigin)
{
	DWORD dwOffset = FILE_EOF;

	if(m_hHandle!=INVALID_HANDLE_VALUE)
	{
		if((dwOffset = ::SetFilePointer(m_hHandle,lOffset,NULL,dwOrigin))==0xFFFFFFFF)
			SetLastErrorCode(::GetLastError());
	}

	return(dwOffset);
}

/*
	SeekEx()

	Posiziona il puntatore all'interno del file.
	Specificare l'offset ed il punto (FILE_BEGIN/CURRENT/END) a partire dal quale posizionare il puntatore.
	L'offset e' LONGLONG (e non QWORD) perche', come la Seek(), permette offset negativi.
	Restituisce FILE_EEOF per errore.
*/
LONGLONG CBinFileEx::SeekEx(LONGLONG llOffset,DWORD dwOrigin)
{
	QWORD qwOffset = FILE_EEOF;
	LARGE_INTEGER li = {0};
	LARGE_INTEGER ofs = {0};

	if(m_hHandle!=INVALID_HANDLE_VALUE)
	{
		li.QuadPart = llOffset;
		
		if(!::SetFilePointerEx(m_hHandle,li,&ofs,dwOrigin))
			SetLastErrorCode(::GetLastError());
		else
			qwOffset = ofs.QuadPart;
	}

	return(qwOffset);
}

/*
	GetFileSize()
*/
DWORD CBinFile::GetFileSize(void)
{
	return(m_hHandle!=INVALID_HANDLE_VALUE ? ::GetFileSize(m_hHandle,NULL) : 0L);
}

/*
	GetFileSizeEx()
*/
QWORD CBinFileEx::GetFileSizeEx(void)
{
	QWORD qwFileSize = 0L;
	
	if(m_hHandle!=INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER li = {0};
		if(::GetFileSizeExt(m_hHandle,&li))
			qwFileSize = li.QuadPart; // QWORD
	}
	
	return(qwFileSize);
}

/*
	GetFileTime()

	Ricava la data/ora UTC del file (assolute, in formato GMT). Se il secondo parametro e' TRUE,
	converte da GMT a local time, aggiungendo o sottraendo la differenza oraria rispetto a GMT:

	- data/ora assolute del file (UTC=GMT): Sun, 06 Nov 1994 08:49:37 GMT (0000)
	- zona oraria locale: GMT +1 (+0100)

	bConvertToLocalTime = FALSE -> Sun, 06 Nov 1994 08:49:37 GMT
	bConvertToLocalTime = TRUE  -> Sun, 06 Nov 1994 09:49:37 +0100
*/
BOOL CBinFile::GetFileTime(CDateTime& datetime,BOOL bConvertToLocalTime/* = FALSE*/)
{
	BOOL bGet = FALSE;

	if(m_hHandle!=INVALID_HANDLE_VALUE)
	{
		FILETIME   gmtfiletime = {0};
		FILETIME   filetime = {0};
		SYSTEMTIME systemtime = {0};

		// ricava la data/ora UTC del file (ossia assolute, GMT)
		if(::GetFileTime(m_hHandle,NULL,NULL,&gmtfiletime))
		{
			if(bConvertToLocalTime)
				::FileTimeToLocalFileTime(&gmtfiletime,&filetime); // converte in locale (aggiunge o sottrae la differenza rispetto a GMT)
			else
				memcpy(&filetime,&gmtfiletime,sizeof(FILETIME)); // nessuna conversione, data/ora assolute (UTC)

			// converte in formato di sistema
			::FileTimeToSystemTime(&filetime,&systemtime);
			
			// formatta la data/ora del file (Day, dd Mon yyyy hh:mm:ss [GMT])
			datetime.SetDateFormat(bConvertToLocalTime ? GMT : GMT_SHORT);
			datetime.SetYear(systemtime.wYear);
			datetime.SetMonth(systemtime.wMonth);
			datetime.SetDay(systemtime.wDay);
			datetime.SetHour(systemtime.wHour);
			datetime.SetMin(systemtime.wMinute);
			datetime.SetSec(systemtime.wSecond);
			
			bGet = TRUE;
		}
	}

	return(bGet);
}

/*
	SetLastErrorCode()

	Imposta il codice d'errore interno.
*/
void CBinFile::SetLastErrorCode(DWORD dwError)
{
	m_dwError = dwError;
	memset(m_szError,'\0',sizeof(m_szError));

	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				m_dwError,
				MAKELANGID(LANG_NEUTRAL,SUBLANG_SYS_DEFAULT),
				(LPSTR)m_szError,
				sizeof(m_szError)-1,
				NULL 
				);

	if(m_bShowErrors)
	{
		char szError[512];
		_snprintf(szError,sizeof(szError)-1,"%s:\n%s",m_szFileName,m_szError);
		::MessageBox(NULL,szError,"CBinFile::SetLastErrorCode()",MB_OK|MB_ICONERROR|MB_TASKMODAL|MB_SETFOREGROUND|MB_TOPMOST);
	}
}
