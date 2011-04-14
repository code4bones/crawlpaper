/*
	CSelfExtractor.cpp
	James Spibey, 04/08/1998
	spib@bigfoot.com
	You are free to use, distribute or modify this code as long as this header is not removed or modified.

	Rimossi i riferimenti a MFC (solo SDK) e cambi vari.
	Aggiunto il supporto per la compressione dei files via GZW.
	Luca Piergentili, 24/08/00
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
#include "CSEFileInfo.h"
#include "CGzw.h"
#include "CSelfExtractor.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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
	CSelfExtractor()
*/
CSelfExtractor::CSelfExtractor()
{
	m_nFiles = 0L;
	m_nTOCSize = 0L;
	m_nErrorCode = 0;
	memset(m_szErrorCode,'\0',sizeof(m_szErrorCode));
}

/*
	Create()

	Creates the sfx from the specified (executable) file, adding all data files.
*/
int CSelfExtractor::Create(LPCSTR lpcszExecutable,LPCSTR lpcszSelfExtractor,FPCALLBACK pfnCallback/* = NULL */,void* pData/* = NULL */)
{
	int nRet = SFX_SUCCESS;

	// the data file list must not be empty
	if(m_FileInfoList.Count() <= 0)
	{
		nRet = SFX_NOTHING_TO_DO;
		goto done;
	}

	// input and output files cannot be the same
	if(strnicmp(lpcszExecutable,lpcszSelfExtractor,strlen(lpcszExecutable))==0 && strnicmp(lpcszExecutable,lpcszSelfExtractor,strlen(lpcszSelfExtractor))==0)
	{
		nRet = SFX_SAME_FILES;
		goto done;
	}

	// gets a copy of the original executable
	::CopyFile(lpcszExecutable,lpcszSelfExtractor,FALSE);

	// creates the sfx adding all data files
	HANDLE hSfxHandle;
	if((hSfxHandle = ::CreateFile(lpcszSelfExtractor,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		// all data is written starting from the end of file
		::SetFilePointer(hSfxHandle,0L,NULL,FILE_END);
		Archive(hSfxHandle,pfnCallback,pData);
		::CloseHandle(hSfxHandle);
	}
	else
	{
		nRet = SFX_OUTPUT_FILE_ERROR;
		goto done;
	}

done:

	return(SetLastError(nRet));
}

/*
	Create()

	Creates the sfx from the specified resource id, adding all data files.
	The resource id, which identifies the executable file, must be named "SFX_EXE".
*/
int CSelfExtractor::Create(UINT nResourceID,LPCSTR lpcszSelfExtractor,FPCALLBACK pfnCallback/* = NULL */,void* pData/* = NULL */)
{
	int nRet = SFX_SUCCESS;

	// the data file list must not be empty
	if(m_FileInfoList.Count() <= 0)
	{
		nRet = SFX_NOTHING_TO_DO;
		goto done;
	}

	// loads the executable from resources
	HRSRC hrSrc;
	if((hrSrc = ::FindResource(NULL,MAKEINTRESOURCE(nResourceID),"SFX_EXE"))==NULL)
	{
		nRet = SFX_RESOURCE_ERROR;
		goto done;
	}
	HGLOBAL hGlobal;
	if((hGlobal = ::LoadResource(NULL,hrSrc))==NULL)
	{
		nRet = SFX_RESOURCE_ERROR;
		goto done;
	}
	LPVOID lpExe;
	if((lpExe = ::LockResource(hGlobal))==NULL)
	{
		nRet = SFX_RESOURCE_ERROR;
		goto done;
	}

	// creates the sfx adding all data files
	HANDLE hSfxHandle;
	if((hSfxHandle = ::CreateFile(lpcszSelfExtractor,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		DWORD dw;
		
		// first, writes the executable into the sfx
		::WriteFile(hSfxHandle,lpExe,(UINT)SizeofResource(NULL,hrSrc),&dw,NULL);
		
		// next, writes the content of data files into the sfx
		Archive(hSfxHandle,pfnCallback,pData);

		::CloseHandle(hSfxHandle);
	}
	else
	{
		nRet = SFX_OUTPUT_FILE_ERROR;
		goto done;
	}

done:

	return(SetLastError(nRet));
}

/*
	Archive()

	Writes the content of data files into the sfx.
*/
int CSelfExtractor::Archive(HANDLE hSfxHandle,FPCALLBACK pfnCallback,void* pData)
{
	int nRet = SFX_SUCCESS;
	CSEFileInfo* pSEFileInfo;
	HANDLE hDataHandle;
	char szBuffer[MAX_BUF];
	DWORD dwTot;
	DWORD dwRead;
	DWORD dw;
	int i;

	// for all data files into the list
	for(i = 0; i < (int)m_nFiles; i++)
	{
		pSEFileInfo = (CSEFileInfo*)m_FileInfoList.GetAt(i);

		// opens the data file
		if((hDataHandle = ::CreateFile(pSEFileInfo->GetPathName(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
		{
			// stores the data file offset into the sfx
			pSEFileInfo->SetFileOffset(::SetFilePointer(hSfxHandle,0L,NULL,FILE_CURRENT));

			// puts the data file into the sfx
			for(dwTot = 0L; dwTot < pSEFileInfo->GetFileSize();)
			{
				::ReadFile(hDataHandle,szBuffer,sizeof(szBuffer),&dwRead,NULL);
				::WriteFile(hSfxHandle,szBuffer,dwRead,&dw,NULL);
				dwTot += dw;
			}

			::CloseHandle(hDataHandle);
			
			// callback
			if(pfnCallback)
				pfnCallback(pSEFileInfo,pData);
		}
		else
		{
			nRet = SFX_INPUT_FILE_ERROR;
			goto done;
		}
	}

	// writes the sfx TOC
	for(i = 0; i < (int)m_nFiles; i++)
	{
		// saves info about the data file (offset, size, filename, etc.)
		pSEFileInfo = (CSEFileInfo*)m_FileInfoList.GetAt(i);

		DWORD dwOffset = pSEFileInfo->GetFileOffset();
		::WriteFile(hSfxHandle,&dwOffset,sizeof(DWORD),&dw,NULL);

		DWORD dwSize = pSEFileInfo->GetFileSize();
		::WriteFile(hSfxHandle,&dwSize,sizeof(DWORD),&dw,NULL);
			
		strncpy(szBuffer,pSEFileInfo->GetFileName(),sizeof(szBuffer));
		::WriteFile(hSfxHandle,szBuffer,strlen(szBuffer),&dw,NULL);
	
		DWORD dwLen = strlen(pSEFileInfo->GetFileName());
		::WriteFile(hSfxHandle,&dwLen,sizeof(DWORD),&dw,NULL);
	}
	
	// number of data files into the sfx
	::WriteFile(hSfxHandle,&m_nFiles,sizeof(DWORD),&dw,NULL);
	
	// writes the signature
	strcpyn(szBuffer,SIGNATURE,sizeof(szBuffer));
	::WriteFile(hSfxHandle,szBuffer,strlen(SIGNATURE),&dw,NULL);

done:

	return(SetLastError(nRet));
}

/*
	Add()

	Adds a data file to the internal list.
	The content of the list (all data files) will be inserted into the sfx by the Archive() member.
*/
BOOL CSelfExtractor::Add(LPCSTR lpcszFileName/* aggiungere parametro con default per nome file di output*/)
{
	BOOL bAdded = FALSE;

	// builds the .gzw name adding the default extension
	char szFileGzw[_MAX_PATH+1];
	_snprintf(szFileGzw,sizeof(szFileGzw)-1,"%s.gzw",lpcszFileName[0]=='@' ? lpcszFileName+1 : lpcszFileName);
	
	// compress the data file
	CGzw gzw;
	gzw.SetOperation(GZW_COMPRESS);
	gzw.SetRatio(9);
	gzw.SetAbsolutePathname(FALSE);
	gzw.SetRelativePathname(FALSE);
	gzw.SetRecursive(FALSE);
	gzw.SetInputFile(lpcszFileName);
	gzw.SetOutputFile(szFileGzw);
	
	if(gzw.Gzw()==GZW_SUCCESS)
	{
		// adds file info to the internal list
		CSEFileInfo* pSEFileInfo = new CSEFileInfo();
		if(pSEFileInfo)
		{
			if(pSEFileInfo->SetData(szFileGzw))
			{
				m_FileInfoList.Add(pSEFileInfo);
				m_nFiles++;
				bAdded = TRUE;
			}
		}
		else
			SetLastError(SFX_MEMORY_ERROR);
	}
	else
		SetLastError(SFX_COMPRESS_ERROR);
		
	return(bAdded);
}

/*
	Extract()

	Extracts a data file from the sfx.
*/
int CSelfExtractor::Extract(int nIndex,LPSTR lpszDir,UINT nDirSize,FPCALLBACK pfnCallback,void* pData)
{
	int nRet = SFX_SUCCESS;
	char szThisModule[_MAX_PATH+1];
	::GetThisModuleFileName(szThisModule,sizeof(szThisModule));
	
	// reads the sfx TOC, loading the list with the data files stored into the sfx
	if((nRet = ReadTOC(szThisModule))==SFX_SUCCESS)
	{
		// opens the sfx
		HANDLE hSfxHandle;
		if((hSfxHandle = ::CreateFile(szThisModule,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
		{
			// gets info about data file
			CSEFileInfo* pSEFileInfo = (CSEFileInfo*)m_FileInfoList.GetAt(nIndex);

			// callback
			if(pfnCallback!=NULL)
				pfnCallback(pSEFileInfo,pData);

			// extracts the data file
			ExtractData(hSfxHandle,nIndex,lpszDir,nDirSize);

			::CloseHandle(hSfxHandle);
		}
		else
			nRet = SFX_INPUT_FILE_ERROR;
	}

	return(SetLastError(nRet));
}

/*
	ExtractAll()

	Extracts all data files from the sfx.
*/
int CSelfExtractor::ExtractAll(LPSTR lpszDir,UINT nDirSize,FPCALLBACK pfnCallback,void* pData)
{
	int nRet = SFX_SUCCESS;
	char szThisModule[_MAX_PATH+1];
	::GetThisModuleFileName(szThisModule,sizeof(szThisModule));

	// reads the sfx TOC, loading the list with the data files stored into the sfx
	if((nRet = ReadTOC(szThisModule))==SFX_SUCCESS)
	{
		// opens the sfx
		HANDLE hSfxHandle;
		if((hSfxHandle = ::CreateFile(szThisModule,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
		{
			int nErrorCount = 0;
			CSEFileInfo* pSEFileInfo;
			
			// extracts all data files
			for(int i = (int)(m_nFiles - 1); i >= 0; i--)
			{
				pSEFileInfo = (CSEFileInfo*)m_FileInfoList.GetAt(i);

				// callback
				if(pfnCallback!=NULL)
					pfnCallback(pSEFileInfo,pData);

				// extraction
				if(ExtractData(hSfxHandle,i,lpszDir,nDirSize)!=SFX_SUCCESS)
					nErrorCount++;
			}

			::CloseHandle(hSfxHandle);

			if(nErrorCount!=0 && (GetLastErrorCode()==SFX_SUCCESS || GetLastErrorCode()==SFX_UNKNOWN_ERROR))
				nRet = SFX_OUTPUT_FILE_ERROR;
		}
		else
			nRet = SFX_INPUT_FILE_ERROR;
	}

	return(SetLastError(nRet));
}

/*
	ExtractData()

	Extracts data into the output file.
*/
int CSelfExtractor::ExtractData(HANDLE hSfxHandle,int nIndex,LPSTR lpszDir,UINT nDirSize)
{
	int nRet = SFX_SUCCESS;
	char szFileName[_MAX_PATH+1];
	char szBuffer[MAX_BUF];
	HANDLE hDataHandle;
	
	CSEFileInfo* pSEFileInfo = (CSEFileInfo*)m_FileInfoList.GetAt(nIndex);
	DWORD dwSize = pSEFileInfo->GetFileSize();

	// creates the data file
	::EnsureBackslash(lpszDir,nDirSize);
	_snprintf(szFileName,sizeof(szFileName)-1,"%s%s",lpszDir,pSEFileInfo->GetFileName());
	if((hDataHandle = ::CreateFile(szFileName,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		// goes to the offset (into the sfx) relative to the data file
		::SetFilePointer(hSfxHandle,(LONG)pSEFileInfo->GetFileOffset(),NULL,FILE_BEGIN);
			
		// extracts the data file
		DWORD dwWritten = 0L;
		DWORD dwRead = 0L;
		DWORD dwAmountToRead = 0L;

		while(TRUE)
		{
			dwAmountToRead = dwSize - dwWritten;
			
			if(dwAmountToRead > MAX_BUF)
				dwAmountToRead = MAX_BUF;
			else if(dwAmountToRead==0L)
				break;

			::ReadFile(hSfxHandle,szBuffer,dwAmountToRead,&dwRead,NULL);
			::WriteFile(hDataHandle,szBuffer,dwRead,&dwRead,NULL);
			
			dwWritten += dwRead;
		}
		
		::CloseHandle(hDataHandle);

		// uncompress the data file
		CGzw gzw;
		gzw.SetOperation(GZW_UNCOMPRESS);
		gzw.SetAbsolutePathname(FALSE);
		gzw.SetRelativePathname(FALSE);
		gzw.SetRecursive(FALSE);
		gzw.SetInputFile(szFileName);
		gzw.SetOutputFile(lpszDir);
		if(gzw.Gzw()!=GZW_SUCCESS)
			nRet = SFX_UNCOMPRESS_ERROR;
		
		::DeleteFile(szFileName);
	}
	else
		nRet = SFX_OUTPUT_FILE_ERROR;

	return(SetLastError(nRet));
}

/*
	ReadTOC()

	Reads the sfx TOC.
*/
int CSelfExtractor::ReadTOC(LPCSTR lpcszFileName)
{
	int nRet = SFX_SUCCESS;
	HANDLE hSfxHandle;
	char szBuffer[MAX_BUF];

	// resets the internal list
	Reset();

	// opens the sfx
	if((hSfxHandle = ::CreateFile(lpcszFileName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))!=INVALID_HANDLE_VALUE)
	{
		DWORD dw = 0L;
		DWORD dwLen = strlen(SIGNATURE);

		// checks the signature
		::SetFilePointer(hSfxHandle,(LONG)dwLen * -1L,NULL,FILE_END);
		::ReadFile(hSfxHandle,szBuffer,dwLen,&dw,NULL);
		if(memcmp(szBuffer,SIGNATURE,strlen(SIGNATURE))==0)
		{
			// gets the number of data files into the sfx
			DWORD dwLastOffset = strlen(SIGNATURE) + sizeof(DWORD);
			::SetFilePointer(hSfxHandle,(LONG)dwLastOffset * -1L,NULL,FILE_END);
			::ReadFile(hSfxHandle,&m_nFiles,sizeof(DWORD),&dw,NULL);
			if(m_nFiles > 0)
			{
				// reads from the sfx TOC, loading the internal list with info about data files
				for(int i = (int)(m_nFiles - 1); i >= 0; i--)
				{
					// reads info about the data file (offset, size, filename, etc.)
					DWORD dwSize = 0L;
					DWORD dwOffset = 0L;
					DWORD dwLen = 0;
					dwLastOffset += sizeof(DWORD);
					
					// len of filename
					::SetFilePointer(hSfxHandle,(LONG)dwLastOffset * -1L,NULL,FILE_END);
					::ReadFile(hSfxHandle,&dwLen,sizeof(DWORD),&dw,NULL);
					dwLastOffset += dwLen;
					
					// filename
					::SetFilePointer(hSfxHandle,(LONG)dwLastOffset * -1L,NULL,FILE_END);
					memset(szBuffer,'\0',sizeof(szBuffer));
					::ReadFile(hSfxHandle,szBuffer,dwLen,&dw,NULL);
					dwLastOffset += sizeof(DWORD);

					// size
					::SetFilePointer(hSfxHandle,(LONG)dwLastOffset * -1L,NULL,FILE_END);
					::ReadFile(hSfxHandle,&dwSize,sizeof(DWORD),&dw,NULL);
					dwLastOffset += sizeof(DWORD);

					// offset into sfx
					::SetFilePointer(hSfxHandle,(LONG)dwLastOffset * -1L,NULL,FILE_END);
					::ReadFile(hSfxHandle,&dwOffset,sizeof(DWORD),&dw,NULL);

					// adds to the internal list
					CSEFileInfo* pSEFileInfo = new CSEFileInfo();
					pSEFileInfo->SetFileSize(dwSize);
					pSEFileInfo->SetFileOffset(dwOffset);
					char szTemp[_MAX_PATH+1];
					strcpyn(szTemp,szBuffer,sizeof(szTemp));
					strrev(szTemp);
					char* pp;
					if((pp = strchr(szTemp,'\\'))!=(char*)NULL)
						*pp = '\0';
					strrev(szTemp);
					pSEFileInfo->SetFileName(szTemp);
					m_FileInfoList.Add(pSEFileInfo);
				}

				m_nTOCSize = dwLastOffset;
			}
			else
				nRet = SFX_NOTHING_TO_DO;
		}
		else
			nRet = SFX_INVALID_SIG;
	
		::CloseHandle(hSfxHandle);
	}
	else
		nRet = SFX_NO_SOURCE;

	return(SetLastError(nRet));
}

/*
	GetFileSize()
*/
DWORD CSelfExtractor::GetFileSize(int nIndex)
{
	CSEFileInfo* pSEFileInfo;
	DWORD dwSize = 0L;

	if((pSEFileInfo = (CSEFileInfo*)m_FileInfoList.GetAt(nIndex))!=(CSEFileInfo*)NULL)
		dwSize = pSEFileInfo->GetFileSize();

	return(dwSize);
}

/*	
	SetLastError()
*/
int CSelfExtractor::SetLastError(int nError)
{
	m_nErrorCode = nError;

	switch(m_nErrorCode)
	{
		case SFX_SUCCESS:
			strcpyn(m_szErrorCode,"",sizeof(m_szErrorCode));
			break;
		case SFX_NO_SOURCE:
			strcpyn(m_szErrorCode,"file not found",sizeof(m_szErrorCode));
			break;
		case SFX_INVALID_SIG:
			strcpyn(m_szErrorCode,"invalid signature",sizeof(m_szErrorCode));
			break;
		case SFX_COPY_FAILED:
			strcpyn(m_szErrorCode,"copy failed",sizeof(m_szErrorCode));
			break;
		case SFX_NOTHING_TO_DO:
			strcpyn(m_szErrorCode,"nothing to do",sizeof(m_szErrorCode));
			break;
		case SFX_OUTPUT_FILE_ERROR:
			strcpyn(m_szErrorCode,"error with output file",sizeof(m_szErrorCode));
			break;
		case SFX_INPUT_FILE_ERROR:
			strcpyn(m_szErrorCode,"error with input file",sizeof(m_szErrorCode));
			break;
		case SFX_RESOURCE_ERROR:
			strcpyn(m_szErrorCode,"resource id not found",sizeof(m_szErrorCode));
			break;
		case SFX_COMPRESS_ERROR:
			strcpyn(m_szErrorCode,"compression error",sizeof(m_szErrorCode));
			break;
		case SFX_UNCOMPRESS_ERROR:
			strcpyn(m_szErrorCode,"uncompression error",sizeof(m_szErrorCode));
			break;
		case SFX_SAME_FILES:
			strcpyn(m_szErrorCode,"files are the same",sizeof(m_szErrorCode));
			break;
		case SFX_MEMORY_ERROR:
			strcpyn(m_szErrorCode,"memory allocation failed",sizeof(m_szErrorCode));
			break;
		case SFX_UNKNOWN_ERROR:
		default:
			strcpyn(m_szErrorCode,"unknown error",sizeof(m_szErrorCode));
			break;
	}

	return(m_nErrorCode);
}

/*
	Reset()
*/
void CSelfExtractor::Reset()
{
	m_FileInfoList.DeleteAll();
	m_nFiles = m_nTOCSize = 0L;
}
