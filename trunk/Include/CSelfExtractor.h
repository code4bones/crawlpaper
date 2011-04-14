/*
	CSelfExtractor.h
	James Spibey, 04/08/1998
	spib@bigfoot.com
	This code was based on suggestions from :-
	Levente Farkas, Roger Allen, Günter (surname unknown)
	You are free to use, distribute or modify this code as long as this header is not removed or modified.

	Self Extractor (SFX) File Format
	---------------------------------
	Starting from the end of the archive and working backwards :-

	Header Info
	10 bytes		Signature - Identifier for SFX archive
	4 bytes		Number of files in archive

	Table of Contents
	Contains one record in the following format for each file
	4 bytes		Length of filename
	variable		Filename
	4 bytes		Length of File	
	4 bytes		Offset in archive to data

	Data Segment
	Each file is written (uncompressed) here in the order of the TOC
	After this is the extractor executable.

	Rimossi i riferimenti a MFC (solo SDK) e cambi vari.
	Aggiunto il supporto per la compressione dei files via GZW.
	Luca Piergentili, 24/08/00
	lpiergentili@yahoo.com
*/
#ifndef _CSELFEXTRACTOR_H
#define _CSELFEXTRACTOR_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include "typedef.h"
#include "window.h"
#include "CNodeList.h"
#include "CSEFileInfo.h"

#define SFX_SUCCESS				0
#define SFX_NO_SOURCE			1
#define SFX_INVALID_SIG			2
#define SFX_COPY_FAILED			3
#define SFX_NOTHING_TO_DO		4
#define SFX_OUTPUT_FILE_ERROR		5
#define SFX_INPUT_FILE_ERROR		6
#define SFX_RESOURCE_ERROR		7
#define SFX_COMPRESS_ERROR		8
#define SFX_UNCOMPRESS_ERROR		9
#define SFX_UNKNOWN_ERROR		10
#define SFX_SAME_FILES			11
#define SFX_MEMORY_ERROR			12

#define MAX_BUF				8192
#define SIGNATURE				"!LYME_SFX!"

/*
	CSEFileInfoList
	classe per la lista dei files
*/
class CSEFileInfoList : public CNodeList
{
public:
	CSEFileInfoList() : CNodeList() {}
	virtual ~CSEFileInfoList() {DeleteAll();}
	void* Create(void)
	{
		return(NULL);
	}
	void* Initialize(void* /*pVoid*/)
	{
		return(NULL);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CSEFileInfo*)iter->data)
		{
			delete ((CSEFileInfo*)iter->data);
			iter->data = (CSEFileInfo*)NULL;
		}
		return(TRUE);
	}
	int Size(void) {return(-1);}
#ifdef _DEBUG
	const char* Signature(void) {return("CSEFileInfoList");}
#endif
};

/*
	CSelfExtractor
*/
class CSelfExtractor
{
public:
	CSelfExtractor();
	virtual ~CSelfExtractor() {}

	// creation
	int			Create			(LPCSTR lpcszExecutable,LPCSTR lpcszSelfExtractor,FPCALLBACK pfnCallback = NULL,void* pData = NULL);
	int			Create			(UINT nResourceID,LPCSTR lpcszSelfExtractor,FPCALLBACK pfnCallback = NULL,void* pData = NULL);
	int			Archive			(HANDLE,FPCALLBACK pfnCallback,void* pData);
	BOOL			Add				(LPCSTR lpcszFileName);

	// extraction
	int			Extract			(int nIndex,LPSTR lpszDir,UINT nDirSize,FPCALLBACK pfnCallback = NULL,void* pData = NULL);
	int			ExtractAll		(LPSTR,UINT nDirSize,FPCALLBACK pfnCallback = NULL,void* pData = NULL);
	int			ExtractData		(HANDLE hSfxHandle,int nIndex,LPSTR lpszDir,UINT nDirSize);
	int			ReadTOC			(LPCSTR lpcszFileName);

	// data
	DWORD		GetFileSize		(int nIndex);
	inline DWORD	GetFileCount		(void) const		{return(m_nFiles);}
	inline CSEFileInfo* GetItem		(int nItem)		{return((CSEFileInfo*)m_FileInfoList.GetAt(nItem));}
	
	void			Reset			(void);
	int			SetLastError		(int nError);
	inline int	GetLastErrorCode	(void) const		{return(m_nErrorCode);}
	inline LPCSTR	GetLastErrorString	(void) const		{return(m_szErrorCode);}

private:
	CSEFileInfoList m_FileInfoList;
	DWORD		m_nFiles;
	DWORD		m_nTOCSize;
	int			m_nErrorCode;
	char			m_szErrorCode[_MAX_PATH+1];
};

#endif // _CSELFEXTRACTOR_H
